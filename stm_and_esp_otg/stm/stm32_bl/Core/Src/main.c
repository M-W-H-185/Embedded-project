/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal_flash.h"
#include "string.h"
#include "SEGGER_RTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
uint8_t rx_buff[300] = "Hello i is bootLoader !\r\n";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define FLASH_APP_ADDR 0x8003000  //你的APP存放起始地址

typedef void (*pFunction)(void);


/********************************************************************************
* 函数名 : IAP_LoadApp
* 功  能 : Bootloader跳转至APP
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_LoadApp(uint32_t appxaddr)
{
    pFunction JumpApp;
    
	if(((*(__IO uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
        __disable_irq();
        JumpApp=(pFunction)*(__IO uint32_t*)(appxaddr+4);   //用户代码区第二个字为程序开始地址(复位地址)		
        __set_MSP(*(__IO uint32_t*)appxaddr);               //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        JumpApp();              //跳转到APP.
	}
}



// 计算 CRCCRC-16/MODBUS
// crc modbus 校验和遵循 低位在前 高位在后
// https://blog.csdn.net/m0_37697335/article/details/113267780
uint16_t calculateCRC(uint8_t *data, int length) {
    unsigned int i;
    unsigned short crc = 0xFFFF;  //crc16位寄存器初始值
    // printf("crc16_modbus校验->length:%d data:",length);
    while(length--)
    {
        // printf("%02x ",*data);
        crc ^= *data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001; //多项式 POLY（0x8005)的高低位交换值，这是由于其模型的一些参数决定的
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    // printf("\r\n");
    return crc;
}
// 合并用的
#define COMBINE_BYTES_TO_UINT16(high_byte, low_byte) (((uint16_t)(high_byte) << 8) | (low_byte))
#define FLASH_MAX_PAGE_SIZE ((1024*2))  // stm32f103每一页的大小

#define USRT1_RX_BUFF_SIZE 1024
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
uint8_t uart1_rx_buff[USRT1_RX_BUFF_SIZE]; 
uint8_t data_length  = 0;

uint16_t firmware_size_cut = 0;  // 固件大小计数器 每 FLASH_MAX_PAGE_SIZE 就写入一次！
uint16_t firmware_buff[FLASH_MAX_PAGE_SIZE/2] = {0};   // 固件缓冲区 2048bit
uint8_t firmware_page_cut = 0;
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite);

void uart1_dataBuffReset(void)
{
    memset(uart1_rx_buff,0,data_length);                                        //清零接收缓冲区
    data_length = 0;
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart1_rx_buff, USRT1_RX_BUFF_SIZE); //重启开始DMA传输 每次255字节数据

}
void USAR1_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(&huart1);      //停止本次DMA传输
    
    data_length  = USRT1_RX_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //计算接收到的数据长度 buff_size - dma剩下的空间
    
    /** 处理 串口数据 **/
    
    SEGGER_RTT_printf(0, "Receive Data(length = %d): ",data_length);
    HAL_UART_Transmit(&huart1,uart1_rx_buff,data_length,USRT1_RX_BUFF_SIZE);            //测试函数：将接收到的数据打印出去
    // 校验帧头
    if(uart1_rx_buff[0] != UART_DATA_HEAD_1 || uart1_rx_buff[1] != UART_DATA_HEAD_2)
    {
        SEGGER_RTT_printf(0,"uart1 read head error!\r\n");
        uart1_dataBuffReset();
        return;
    }
    // 帧头1 + 帧头2 + cmd_id + isack + size_d + size_h + ....dataBlock
    // 命令帧
    uint8_t  cmd_id = uart1_rx_buff[2];
    // 是否需要应答
    uint8_t  isack  = uart1_rx_buff[3];
    // 数据块的长度
    uint16_t data_block_size = ((uart1_rx_buff)[4] << 8) | (uart1_rx_buff)[5];  
    // 数据块
    uint8_t *data_block = &(uart1_rx_buff[6]);
    // 串口接收后解析的crc modbus 校验和
    uint16_t uart_calculateCRC = calculateCRC(uart1_rx_buff,data_length - 2);
    // 串口接收到的crc modbus校验和
    uint16_t uart_read_crc = COMBINE_BYTES_TO_UINT16(uart1_rx_buff[data_length-1],uart1_rx_buff[data_length-2]);
        // 到了这里代表串口接收到的数据是完整正确的
    SEGGER_RTT_printf(0, 
        "uart1 read success! cmd_id:%02x isack:%02x data_block_size:%d  uart_calculateCRC:%d uart_read_crc:%d uartCRC_L:%02x uartCRC_H:%02x\r\n",
        cmd_id,isack,data_block_size,uart_calculateCRC,uart_read_crc,uart1_rx_buff[data_length-2],uart1_rx_buff[data_length-1]
    );
    // crc modbus 校验和
    if(uart_calculateCRC != uart_read_crc)
    {
        SEGGER_RTT_printf(0,"uart1 read crc modbus error!\r\n");
        uart1_dataBuffReset();
        return;
    }


    if(cmd_id == 0x03)   // 固件接收
    {
        // 帧头1 + 帧头2 + cmd_id + isack + size_d + size_h + (dataBlock[0] = 分包号) + (dataBlock[1] = 一共有多少个固件包 ) + ..dataBlock
        uint16_t uart_firmware_block_size = data_block_size - 2;// 因为固件包的第一位是固件包分包号
        
        uint8_t firmware_split_number = uart1_rx_buff[6];   // 当前分包号 
        uint8_t firmware_split_max = uart1_rx_buff[7];  // 一共有多少个包
        
        uint8_t *firmware_block = &(uart1_rx_buff[8]);  // 后面都是固件数据
        

        // 上面就收到了固件包了


        firmware_size_cut += uart_firmware_block_size;
        SEGGER_RTT_printf(0,"firmware_Log: firmware_split_number:%d firmware_split_max:%d uart_firmware_block_size:%d\r\n",
            firmware_split_number,firmware_split_max,
            uart_firmware_block_size
        );
        for(int i = 0;i<64;i++)
        {
             // 比如flash[2] = {0x58,0x0D} 则读取出来的值就是 uint16 = 0x0D58
            // firmware_size_cut - 128 就是让数组指向0从0写入
            // /2 uint8_t 转了 uint16 就只需要写入一半的次数
            // +1 。。
            firmware_buff[ ((firmware_size_cut - 128) / 2)+ i ] = COMBINE_BYTES_TO_UINT16(firmware_block[(i*2)+1], firmware_block[(i*2)]);
            //SEGGER_RTT_printf(0,"%04x",COMBINE_BYTES_TO_UINT16(firmware_block[(i*2)+1], firmware_block[(i*2)]));

        }            
        SEGGER_RTT_printf(0,"\r\n");

        // 每收到FLASH_MAX_PAGE_SIZE个固件包就去写入一次flash
        if (firmware_size_cut % FLASH_MAX_PAGE_SIZE == 0) {
           
//            for(int i =0;i<(FLASH_MAX_PAGE_SIZE/2);i++)
//            {
//                SEGGER_RTT_printf(0,"%04x ",firmware_buff[i]);
//            }
            SEGGER_RTT_printf(0,"\r\nread 2048 firmware %d  %08x\r\n",firmware_size_cut,0x8003000 + (firmware_page_cut * FLASH_MAX_PAGE_SIZE));
            
            STMFLASH_WriteMultipleBytes(0x8003000 + (firmware_page_cut * FLASH_MAX_PAGE_SIZE),firmware_buff,sizeof(firmware_buff));
            firmware_size_cut = 0;
            firmware_page_cut++;
        }

        // 当前分包号 = 最大分包号的时候 代表接受完成
        // 接收完了也得去写一次固件包
        if(firmware_split_number == firmware_split_max )
        {
            for(int i =0;i<(FLASH_MAX_PAGE_SIZE/2);i++)
            {
                SEGGER_RTT_printf(0,"%04x ",firmware_buff[i]);
            }
            SEGGER_RTT_printf(0,"\r\nread 2048 firmware %d  \r\n",firmware_size_cut);
            firmware_page_cut++;
            STMFLASH_WriteMultipleBytes(0x8003000 + (firmware_page_cut * FLASH_MAX_PAGE_SIZE),firmware_buff,sizeof(firmware_buff));
            firmware_size_cut = 0;
            SEGGER_RTT_printf(0,"read firmware finish ! %08x \r\n",0x8003000 + (firmware_page_cut * FLASH_MAX_PAGE_SIZE));
        }
     
      
    }
            
    /** 处理 串口数据 **/
       
 
    uart1_dataBuffReset();
}


void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //判断是否是串口1（！此处应写(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //清楚空闲中断标志（否则会一直不断进入中断）
            USAR1_UART_IDLECallback(huart);                          //调用中断处理函数
        }
    }
}

// flash 操作函数 begin
// stm32 flash采用小端存储方式 即 低位在前 高位在后
// 比如flash[2] = {0x58,0x0D} 则读取出来的值就是 uint16 = 0x0D58
/* 用户根据自己所选的芯片设置 */
#define STM32_FLASH_SIZE 256            //所选STM32的FLASH容量大小(单位为K)
 
/* FLASH起始地址 */
#define STM32_FLASH_BASE 0x08000000     //STM32 FLASH的起始地址
 
/* 小容量和中容量产品每页1K字节（1024 Byte）、大容量每页2K字节（2048 Byte）*/
#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024
#else 
#define STM_SECTOR_SIZE 2048
#endif
 
/* 最多是2K字节 这里除以2是因为STM_SECTOR_SIZE是字节数，STMFLASH_BUF数组是半字数组 */
uint16_t  STMFLASH_BUF[STM_SECTOR_SIZE/2];  

/**
*@brief		读取指定地址的半字(16位数据)
*@param		faddr:读地址(此地址必须为2的倍数!!)
*@return	读取的数据.
*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
/**
*@brief     从指定地址开始读出指定长度的数据
*@param     ReadAddr  : 起始地址
*           pBuffer   : 数据指针
*           NumToWrite: 半字(16位)数
*@return    无
*/
void STMFLASH_ReadMultipleBytes(uint32_t ReadAddr,uint16_t  *pBuffer,uint16_t  NumToRead)      
{
    uint16_t  i;
    for(i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);         // 读取2个字节.
        ReadAddr += 2;                                        // 偏移2个字节. 
    }
}
/**
*@brief   无校验写入（该函数不校验Flash地址是否可写，是否需要擦除，所以不能直接用于写操作）
*@param   WriteAddr : 起始地址
*         pBuffer   : 数据指针
*         NumToWrite: 半字(16位)数   
*@return  无
*/
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)   
{                    
    uint16_t  i;
    for(i = 0;i < NumToWrite;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);  // 写入flash FLASH_TYPEPROGRAM_HALFWORD是按照16bit写入的

        WriteAddr += 2;                              // 地址增加2.
    }  
} 
/**
*@brief     从指定地址开始写入指定长度的数据
*@param     WriteAddr : 起始地址(此地址必须为2的倍数!!)
*           pBuffer   : 数据指针
*           NumToWrite: 半字(16位)数(就是要写入的16位数据的个数.)
*@return    无
*/
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flash页地址
    uint16_t  WordAddr;                                                                                            // 要写入的地址在Flash页中的位置(16位字计算)
    uint16_t  WordRemainder;                                                                                       // Flash页中的剩余地址(16位字计算)
    uint32_t  ShiftingAddr;                                                                                        // 去掉0X08000000后的地址
    uint16_t  i;    
    __disable_irq();
    // 验证是否能被2整除
    if (WriteAddr % 2 != 0) 
    {
        return -1;
    } 

    /* 当指定起始地址小于STM32_FLASH_BASE (0x0800000) 或者大于芯片本身的Flash容量时，写入地址无效，跳出函数 */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return -2;                                                                                                    // 非法地址
    }
    HAL_FLASH_Unlock(); 
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // 实际偏移地址.要写入数据起始地址的位置
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // 要写入的地址所在的Flash页（0~256）
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // 在Flash页内的偏移(2个字节为基本单位.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flash页剩余空间大小
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // 不大于该Flash页范围
    }
 
    while(1) 
    {
        STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 读出整个Flash页的内容存放到STMFLASH_BUF中
        
        /* 查验数据，看flash页是否需要擦除 */
        for(i = 0;i < WordRemainder;i++)                                                                           // 校验数据
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // 需要擦除
            }     
        }
        /* 如果要写入数据的Flash页面上，所有的字都等于0XFFFF，那么在上面的循环之后，i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // 需要擦除
        {
            //FLASH_ErasePage(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // 擦除这个Flash页
            FLASH_EraseInitTypeDef FLASH_EraseInitType;
            FLASH_EraseInitType.TypeErase 	= FLASH_TYPEERASE_PAGES;
            FLASH_EraseInitType.PageAddress = PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE;
            FLASH_EraseInitType.NbPages 		= 1;
            uint32_t PageError = HAL_FLASH_GetError();
            HAL_FLASHEx_Erase(&FLASH_EraseInitType, &PageError);

            for(i = 0;i < WordRemainder;i++)                                                                       // 复制
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 写入整个页
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // 写已经擦除了的,直接写入扇区剩余区间. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // 写入结束了
        }
        else                                                                                                       // 写入未结束
        {
            PageAddr++;                                                                                            // 页地址增加1
            WordAddr    = 0;                                                                                       // 偏移位置为0     
            pBuffer    += WordRemainder;                                                                           // 指针偏移
            WriteAddr  += WordRemainder*2;                                                                         // 写地址偏移(16位数据址,需要*2)
            NumToWrite -= WordRemainder;                                                                           // 字节(16位)数递减
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
                WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // 下一个Flash页还是写不完
            }
            else WordRemainder = NumToWrite;                                                                       // 下一个Flash页可以写完了
        }    
    };  
    

	HAL_FLASH_Lock();
    __enable_irq();
    return 0;
    
}
// flash 操作函数 end


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
        // bl 占用大小 0x3000 
        // falsh每页 2048
        // 所以 bl占用了 6页
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    SEGGER_RTT_Init();

//    // stm32 flash 只能写 0 不能写 1 所以再次写入需要擦除
//    HAL_FLASH_Unlock();     //解锁Flash
//    uint16_t Write_Flash_Data = 0x6666;
//    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FLASH_APP_ADDR,Write_Flash_Data);  // 写入flash FLASH_TYPEPROGRAM_HALFWORD是按照16bit写入的
//    HAL_FLASH_Lock();       //锁住Flash
//    uint16_t data_[] = {0x6666,0x7777,0x8888};
//    STMFLASH_WriteMultipleBytes(0x8003002,data_,sizeof(data_));
//    
    IAP_LoadApp(FLASH_APP_ADDR); //程序跳转

    SEGGER_RTT_printf(0, "Init RTT Log \r\n");  
    SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  

//    uint16_t temp[5] = {0};
//    
//    flash_read(0x8000000, temp, sizeof(temp));
//    SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
         //   SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  
            HAL_Delay(1111);
        //    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        //   
        //    //HAL_Delay(1111);
         //IAP_LoadApp(FLASH_APP_ADDR); //程序跳转
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                    // 空闲中断 串口中断
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart1_rx_buff, USRT1_RX_BUFF_SIZE);     //设置DMA传输，讲串口1的数据搬运到recvive_buff中，
                                                                    //每次255个字节

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
