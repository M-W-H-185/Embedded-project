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
		JumpApp=(pFunction)*(__IO uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
    __set_MSP(*(__IO uint32_t*)appxaddr);				    //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
  
		JumpApp();								                //跳转到APP.
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

#define USRT1_RX_BUFF_SIZE 1024
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
uint8_t uart1_rx_buff[USRT1_RX_BUFF_SIZE]; 
uint8_t data_length  = 0;

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
    // crc modbus 校验和
    if(uart_calculateCRC != uart_read_crc)
    {
        SEGGER_RTT_printf(0,"uart1 read crc modbus error!\r\n");
        uart1_dataBuffReset();
        return;
    }

    // 到了这里代表串口接收到的数据是完整正确的
    SEGGER_RTT_printf(0, 
        "uart1 read success! cmd_id:%02x isack:%02x data_block_size:%d  uart_calculateCRC:%d uart_read_crc:%d uartCRC_L:%02x uartCRC_H:%02x\r\n",
        cmd_id,isack,data_block_size,uart_calculateCRC,uart_read_crc,uart1_rx_buff[data_length-2],uart1_rx_buff[data_length-1]
    );
            
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





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
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

// 注意写入过一次，下一次就得先擦除！不如写入无效的
HAL_FLASH_Unlock();     //解锁Flash
uint16_t Write_Flash_Data = 0x6666;
HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FLASH_APP_ADDR,Write_Flash_Data);  // 写入flash FLASH_TYPEPROGRAM_HALFWORD是按照16bit写入的
HAL_FLASH_Lock();       //锁住Flash

  SEGGER_RTT_printf(0, "Init RTT Log\r\n");  
  SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//    SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  
//    HAL_Delay(1111);
//    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
//   
//    //HAL_Delay(1111);
    // IAP_LoadApp(FLASH_APP_ADDR); //程序跳转
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
