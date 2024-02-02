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

#define FLASH_APP_ADDR 0x8003000  //���APP�����ʼ��ַ

typedef void (*pFunction)(void);


/********************************************************************************
* ������ : IAP_LoadApp
* ��  �� : Bootloader��ת��APP
* ˵  �� : none
* ��  �� : none
* ��  �� : none
* ��  �� : Shatang                    ʱ  �� : 2020.06.23
* ��  �� : none                       ʱ  �� : none
********************************************************************************/
void IAP_LoadApp(uint32_t appxaddr)
{
    pFunction JumpApp;
    
	if(((*(__IO uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
        __disable_irq();
        JumpApp=(pFunction)*(__IO uint32_t*)(appxaddr+4);   //�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
        __set_MSP(*(__IO uint32_t*)appxaddr);               //��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
        JumpApp();              //��ת��APP.
	}
}



// ���� CRCCRC-16/MODBUS
// crc modbus У�����ѭ ��λ��ǰ ��λ�ں�
// https://blog.csdn.net/m0_37697335/article/details/113267780
uint16_t calculateCRC(uint8_t *data, int length) {
    unsigned int i;
    unsigned short crc = 0xFFFF;  //crc16λ�Ĵ�����ʼֵ
    // printf("crc16_modbusУ��->length:%d data:",length);
    while(length--)
    {
        // printf("%02x ",*data);
        crc ^= *data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001; //����ʽ POLY��0x8005)�ĸߵ�λ����ֵ������������ģ�͵�һЩ����������
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
// �ϲ��õ�
#define COMBINE_BYTES_TO_UINT16(high_byte, low_byte) (((uint16_t)(high_byte) << 8) | (low_byte))
#define FLASH_MAX_PAGE_SIZE ((1024*2))  // stm32f103ÿһҳ�Ĵ�С

#define USRT1_RX_BUFF_SIZE 1024
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
uint8_t uart1_rx_buff[USRT1_RX_BUFF_SIZE]; 
uint8_t data_length  = 0;

uint16_t firmware_size_cut = 0;  // �̼���С������ ÿ FLASH_MAX_PAGE_SIZE ��д��һ�Σ�
uint16_t firmware_buff[FLASH_MAX_PAGE_SIZE/2] = {0};   // �̼������� 2048bit
uint8_t firmware_page_cut = 0;
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite);

void uart1_dataBuffReset(void)
{
    memset(uart1_rx_buff,0,data_length);                                        //������ջ�����
    data_length = 0;
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart1_rx_buff, USRT1_RX_BUFF_SIZE); //������ʼDMA���� ÿ��255�ֽ�����

}
void USAR1_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(&huart1);      //ֹͣ����DMA����
    
    data_length  = USRT1_RX_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //������յ������ݳ��� buff_size - dmaʣ�µĿռ�
    
    /** ���� �������� **/
    
    SEGGER_RTT_printf(0, "Receive Data(length = %d): ",data_length);
    HAL_UART_Transmit(&huart1,uart1_rx_buff,data_length,USRT1_RX_BUFF_SIZE);            //���Ժ����������յ������ݴ�ӡ��ȥ
    // У��֡ͷ
    if(uart1_rx_buff[0] != UART_DATA_HEAD_1 || uart1_rx_buff[1] != UART_DATA_HEAD_2)
    {
        SEGGER_RTT_printf(0,"uart1 read head error!\r\n");
        uart1_dataBuffReset();
        return;
    }
    // ֡ͷ1 + ֡ͷ2 + cmd_id + isack + size_d + size_h + ....dataBlock
    // ����֡
    uint8_t  cmd_id = uart1_rx_buff[2];
    // �Ƿ���ҪӦ��
    uint8_t  isack  = uart1_rx_buff[3];
    // ���ݿ�ĳ���
    uint16_t data_block_size = ((uart1_rx_buff)[4] << 8) | (uart1_rx_buff)[5];  
    // ���ݿ�
    uint8_t *data_block = &(uart1_rx_buff[6]);
    // ���ڽ��պ������crc modbus У���
    uint16_t uart_calculateCRC = calculateCRC(uart1_rx_buff,data_length - 2);
    // ���ڽ��յ���crc modbusУ���
    uint16_t uart_read_crc = COMBINE_BYTES_TO_UINT16(uart1_rx_buff[data_length-1],uart1_rx_buff[data_length-2]);
        // ������������ڽ��յ���������������ȷ��
    SEGGER_RTT_printf(0, 
        "uart1 read success! cmd_id:%02x isack:%02x data_block_size:%d  uart_calculateCRC:%d uart_read_crc:%d uartCRC_L:%02x uartCRC_H:%02x\r\n",
        cmd_id,isack,data_block_size,uart_calculateCRC,uart_read_crc,uart1_rx_buff[data_length-2],uart1_rx_buff[data_length-1]
    );
    // crc modbus У���
    if(uart_calculateCRC != uart_read_crc)
    {
        SEGGER_RTT_printf(0,"uart1 read crc modbus error!\r\n");
        uart1_dataBuffReset();
        return;
    }


    if(cmd_id == 0x03)   // �̼�����
    {
        // ֡ͷ1 + ֡ͷ2 + cmd_id + isack + size_d + size_h + (dataBlock[0] = �ְ���) + (dataBlock[1] = һ���ж��ٸ��̼��� ) + ..dataBlock
        uint16_t uart_firmware_block_size = data_block_size - 2;// ��Ϊ�̼����ĵ�һλ�ǹ̼����ְ���
        
        uint8_t firmware_split_number = uart1_rx_buff[6];   // ��ǰ�ְ��� 
        uint8_t firmware_split_max = uart1_rx_buff[7];  // һ���ж��ٸ���
        
        uint8_t *firmware_block = &(uart1_rx_buff[8]);  // ���涼�ǹ̼�����
        

        // ������յ��˹̼�����


        firmware_size_cut += uart_firmware_block_size;
        SEGGER_RTT_printf(0,"firmware_Log: firmware_split_number:%d firmware_split_max:%d uart_firmware_block_size:%d\r\n",
            firmware_split_number,firmware_split_max,
            uart_firmware_block_size
        );
        for(int i = 0;i<64;i++)
        {
             // ����flash[2] = {0x58,0x0D} ���ȡ������ֵ���� uint16 = 0x0D58
            // firmware_size_cut - 128 ����������ָ��0��0д��
            // /2 uint8_t ת�� uint16 ��ֻ��Ҫд��һ��Ĵ���
            // +1 ����
            firmware_buff[ ((firmware_size_cut - 128) / 2)+ i ] = COMBINE_BYTES_TO_UINT16(firmware_block[(i*2)+1], firmware_block[(i*2)]);
            //SEGGER_RTT_printf(0,"%04x",COMBINE_BYTES_TO_UINT16(firmware_block[(i*2)+1], firmware_block[(i*2)]));

        }            
        SEGGER_RTT_printf(0,"\r\n");

        // ÿ�յ�FLASH_MAX_PAGE_SIZE���̼�����ȥд��һ��flash
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

        // ��ǰ�ְ��� = ���ְ��ŵ�ʱ�� ����������
        // ��������Ҳ��ȥдһ�ι̼���
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
            
    /** ���� �������� **/
       
 
    uart1_dataBuffReset();
}


void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //�ж��Ƿ��Ǵ���1�����˴�Ӧд(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
            USAR1_UART_IDLECallback(huart);                          //�����жϴ�����
        }
    }
}

// flash �������� begin
// stm32 flash����С�˴洢��ʽ �� ��λ��ǰ ��λ�ں�
// ����flash[2] = {0x58,0x0D} ���ȡ������ֵ���� uint16 = 0x0D58
/* �û������Լ���ѡ��оƬ���� */
#define STM32_FLASH_SIZE 256            //��ѡSTM32��FLASH������С(��λΪK)
 
/* FLASH��ʼ��ַ */
#define STM32_FLASH_BASE 0x08000000     //STM32 FLASH����ʼ��ַ
 
/* С��������������Ʒÿҳ1K�ֽڣ�1024 Byte����������ÿҳ2K�ֽڣ�2048 Byte��*/
#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024
#else 
#define STM_SECTOR_SIZE 2048
#endif
 
/* �����2K�ֽ� �������2����ΪSTM_SECTOR_SIZE���ֽ�����STMFLASH_BUF�����ǰ������� */
uint16_t  STMFLASH_BUF[STM_SECTOR_SIZE/2];  

/**
*@brief		��ȡָ����ַ�İ���(16λ����)
*@param		faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
*@return	��ȡ������.
*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
/**
*@brief     ��ָ����ַ��ʼ����ָ�����ȵ�����
*@param     ReadAddr  : ��ʼ��ַ
*           pBuffer   : ����ָ��
*           NumToWrite: ����(16λ)��
*@return    ��
*/
void STMFLASH_ReadMultipleBytes(uint32_t ReadAddr,uint16_t  *pBuffer,uint16_t  NumToRead)      
{
    uint16_t  i;
    for(i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);         // ��ȡ2���ֽ�.
        ReadAddr += 2;                                        // ƫ��2���ֽ�. 
    }
}
/**
*@brief   ��У��д�루�ú�����У��Flash��ַ�Ƿ��д���Ƿ���Ҫ���������Բ���ֱ������д������
*@param   WriteAddr : ��ʼ��ַ
*         pBuffer   : ����ָ��
*         NumToWrite: ����(16λ)��   
*@return  ��
*/
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)   
{                    
    uint16_t  i;
    for(i = 0;i < NumToWrite;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);  // д��flash FLASH_TYPEPROGRAM_HALFWORD�ǰ���16bitд���

        WriteAddr += 2;                              // ��ַ����2.
    }  
} 
/**
*@brief     ��ָ����ַ��ʼд��ָ�����ȵ�����
*@param     WriteAddr : ��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
*           pBuffer   : ����ָ��
*           NumToWrite: ����(16λ)��(����Ҫд���16λ���ݵĸ���.)
*@return    ��
*/
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flashҳ��ַ
    uint16_t  WordAddr;                                                                                            // Ҫд��ĵ�ַ��Flashҳ�е�λ��(16λ�ּ���)
    uint16_t  WordRemainder;                                                                                       // Flashҳ�е�ʣ���ַ(16λ�ּ���)
    uint32_t  ShiftingAddr;                                                                                        // ȥ��0X08000000��ĵ�ַ
    uint16_t  i;    
    __disable_irq();
    // ��֤�Ƿ��ܱ�2����
    if (WriteAddr % 2 != 0) 
    {
        return -1;
    } 

    /* ��ָ����ʼ��ַС��STM32_FLASH_BASE (0x0800000) ���ߴ���оƬ�����Flash����ʱ��д���ַ��Ч���������� */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return -2;                                                                                                    // �Ƿ���ַ
    }
    HAL_FLASH_Unlock(); 
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // ʵ��ƫ�Ƶ�ַ.Ҫд��������ʼ��ַ��λ��
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // Ҫд��ĵ�ַ���ڵ�Flashҳ��0~256��
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // ��Flashҳ�ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flashҳʣ��ռ��С
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // �����ڸ�Flashҳ��Χ
    }
 
    while(1) 
    {
        STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // ��������Flashҳ�����ݴ�ŵ�STMFLASH_BUF��
        
        /* �������ݣ���flashҳ�Ƿ���Ҫ���� */
        for(i = 0;i < WordRemainder;i++)                                                                           // У������
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // ��Ҫ����
            }     
        }
        /* ���Ҫд�����ݵ�Flashҳ���ϣ����е��ֶ�����0XFFFF����ô�������ѭ��֮��i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // ��Ҫ����
        {
            //FLASH_ErasePage(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // �������Flashҳ
            FLASH_EraseInitTypeDef FLASH_EraseInitType;
            FLASH_EraseInitType.TypeErase 	= FLASH_TYPEERASE_PAGES;
            FLASH_EraseInitType.PageAddress = PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE;
            FLASH_EraseInitType.NbPages 		= 1;
            uint32_t PageError = HAL_FLASH_GetError();
            HAL_FLASHEx_Erase(&FLASH_EraseInitType, &PageError);

            for(i = 0;i < WordRemainder;i++)                                                                       // ����
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // д������ҳ
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // д�Ѿ������˵�,ֱ��д������ʣ������. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // д�������
        }
        else                                                                                                       // д��δ����
        {
            PageAddr++;                                                                                            // ҳ��ַ����1
            WordAddr    = 0;                                                                                       // ƫ��λ��Ϊ0     
            pBuffer    += WordRemainder;                                                                           // ָ��ƫ��
            WriteAddr  += WordRemainder*2;                                                                         // д��ַƫ��(16λ����ַ,��Ҫ*2)
            NumToWrite -= WordRemainder;                                                                           // �ֽ�(16λ)���ݼ�
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
                WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // ��һ��Flashҳ����д����
            }
            else WordRemainder = NumToWrite;                                                                       // ��һ��Flashҳ����д����
        }    
    };  
    

	HAL_FLASH_Lock();
    __enable_irq();
    return 0;
    
}
// flash �������� end


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
        // bl ռ�ô�С 0x3000 
        // falshÿҳ 2048
        // ���� blռ���� 6ҳ
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

//    // stm32 flash ֻ��д 0 ����д 1 �����ٴ�д����Ҫ����
//    HAL_FLASH_Unlock();     //����Flash
//    uint16_t Write_Flash_Data = 0x6666;
//    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FLASH_APP_ADDR,Write_Flash_Data);  // д��flash FLASH_TYPEPROGRAM_HALFWORD�ǰ���16bitд���
//    HAL_FLASH_Lock();       //��סFlash
//    uint16_t data_[] = {0x6666,0x7777,0x8888};
//    STMFLASH_WriteMultipleBytes(0x8003002,data_,sizeof(data_));
//    
    IAP_LoadApp(FLASH_APP_ADDR); //������ת

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
         //IAP_LoadApp(FLASH_APP_ADDR); //������ת
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
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                    // �����ж� �����ж�
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart1_rx_buff, USRT1_RX_BUFF_SIZE);     //����DMA���䣬������1�����ݰ��˵�recvive_buff�У�
                                                                    //ÿ��255���ֽ�

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
