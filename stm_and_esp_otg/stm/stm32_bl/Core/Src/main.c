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
#include <stdlib.h>

#include "string.h"
#include "SEGGER_RTT.h"
#include "user_flash.h"
#include "bsp_eeprom.h"
#include "app_otaConfig.h"
#include "utility_common.h"
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
// Bootloader��ת��APP
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



#define FLASH_MAX_PAGE_SIZE_8      (STM_SECTOR_SIZE)  // stm32f103ÿһҳ�Ĵ�С
#define FLASH_MAX_PAGE_SIZE_16     (STM_SECTOR_SIZE/2)  // stm32f103ÿһҳ�Ĵ�С

#define USRT1_RX_BUFF_SIZE 1024
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
uint8_t uart1_rx_buff[USRT1_RX_BUFF_SIZE]; 
uint8_t uart1_data_length  = 0;

uint16_t firmware_size_cut = 0;  // �̼���С������ ÿ FLASH_MAX_PAGE_SIZE ��д��һ�Σ�

uint8_t firmware_buff_8[FLASH_MAX_PAGE_SIZE_8] = {0};   // �̼������� 
uint16_t firmware_buff_16[FLASH_MAX_PAGE_SIZE_16] = {0};   // �̼������� 

uint8_t firmware_page_cut = 0;  // flash�ڼ�ҳ

uint8_t is_update_success = 0;
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
// uart_data ����һ����ָ�����
// cmd_id ����id
// isack �Ƿ�Ӧ��
// data ������
// data_length �������ĳ���
int uart_send_cmdid_data_handle(char **uart_rx_data, uint8_t cmd_id, uint8_t isack, char* data, int data_length)
{
    
    int uart_rx_data_length = data_length + 8;
    *uart_rx_data = malloc(uart_rx_data_length * sizeof(char));

    (*uart_rx_data)[0] = UART_DATA_HEAD_1;
    (*uart_rx_data)[1] = UART_DATA_HEAD_2;
    (*uart_rx_data)[2] = cmd_id;
    (*uart_rx_data)[3] = isack;

    (*uart_rx_data)[4] =  (data_length >> 8) & 0xFF; // ���ݳ��� ��λ
    (*uart_rx_data)[5] = data_length & 0xFF;       // ���ݳ��� ��λ

    char *uart_data_ = &((*uart_rx_data)[6]);
    memcpy(uart_data_, data, data_length * sizeof(char));

    // -2 ��Ҫ��У��λ����
    uint16_t crc = calculateCRC(*uart_rx_data, uart_rx_data_length - 2);
    (*uart_rx_data)[uart_rx_data_length - 2] = crc & 0xFF;       // У��� ��λ
    (*uart_rx_data)[uart_rx_data_length - 1] = (crc >> 8) & 0xFF; // У��� ��λ

    return uart_rx_data_length;
}
// ����ack����
void uart_send_ack_data(void)
{
    char *uart_rx_data = NULL;
    static char data[] = {255};
    int uart_rx_data_length = uart_send_cmdid_data_handle(&uart_rx_data, 0x00, 0x00, data, 1);
    HAL_UART_Transmit(&huart1,(uint8_t *)uart_rx_data,uart_rx_data_length,100);
    free(uart_rx_data);
}
// �����豸�汾��Ϣ
void ota_send_appVersionInfo(void)
{
    // ��ȡ�汾��Ϣ
    char version_str[255] = { 0 };
    memset(version_str, 0, 255);
    uint8_t version_length = app_otaGetVersion(version_str);
    
    // ƴ������ͳ�ȥ
    char data[300] = {0};
    uint16_t data_size = version_length + 2;
    data[0] = 0x01;     // ����BootLoader����
    data[1] = version_length; 
    memcpy((uint8_t*)&data[2] , version_str, version_length);
    
    char *uart_rx_data = NULL;
    int uart_rx_data_length = uart_send_cmdid_data_handle(&uart_rx_data, 0x01, 0x02, data, data_size);
    HAL_UART_Transmit(&huart1,(uint8_t *)uart_rx_data,uart_rx_data_length,100);
    free(uart_rx_data);

}

void uart1_dataBuffReset(void)
{
    memset(uart1_rx_buff,0,uart1_data_length);                                        //������ջ�����
    uart1_data_length = 0;

}
void USAR1_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(&huart1);      //ֹͣ����DMA����
    
    uart1_data_length  = USRT1_RX_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //������յ������ݳ��� buff_size - dmaʣ�µĿռ�
    
    /** ���� �������� **/
    
    SEGGER_RTT_printf(0, "Receive Data(length = %d): ",uart1_data_length);
    //HAL_UART_Transmit(&huart1,uart1_rx_buff,data_length,USRT1_RX_BUFF_SIZE);            //���Ժ����������յ������ݴ�ӡ��ȥ
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
    uint16_t uart_calculateCRC = calculateCRC(uart1_rx_buff,uart1_data_length - 2);
    // ���ڽ��յ���crc modbusУ���
    uint16_t uart_read_crc = COMBINE_BYTES_TO_UINT16(uart1_rx_buff[uart1_data_length-1],uart1_rx_buff[uart1_data_length-2]);
        // ������������ڽ��յ���������������ȷ��
    SEGGER_RTT_printf(0, 
        "uart1 read success! cmd_id:%02x isack:%02x data_block_size:%d  uart_calculateCRC:%d uart_read_crc:%d uartCRC_L:%02x uartCRC_H:%02x\r\n",
        cmd_id,isack,data_block_size,uart_calculateCRC,uart_read_crc,uart1_rx_buff[uart1_data_length-2],uart1_rx_buff[uart1_data_length-1]
    );
    // crc modbus У���
    if(uart_calculateCRC != uart_read_crc)
    {
        SEGGER_RTT_printf(0,"uart1 read crc modbus error!\r\n");
        uart1_dataBuffReset();
        return;
    }
    // ���������������ݾ��ǺϹ��

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
        // �����ݰ��浽buff��
        memcpy( (uint8_t*)(firmware_buff_8 + (firmware_size_cut - uart_firmware_block_size)) , firmware_block, uart_firmware_block_size) ;
        
//        SEGGER_RTT_printf(0,"data: ");
//        for(int i = 0 ; i<FLASH_MAX_PAGE_SIZE_8; i++)
//        {
//            SEGGER_RTT_printf(0,"%02x ",firmware_buff_8[i]);
//        }
//        SEGGER_RTT_printf(0,"\r\n ");

        
        uint32_t worte_address = 0;
        
        // ÿ�յ�FLASH_MAX_PAGE_SIZE���̼�����ȥд��һ��flash
        if (firmware_size_cut % FLASH_MAX_PAGE_SIZE_8 == 0) 
        {
            convertUint8ToUint16(firmware_buff_8, firmware_buff_16, FLASH_MAX_PAGE_SIZE_8, FLASH_MAX_PAGE_SIZE_16);
            worte_address = FLASH_APP_ADDR + (firmware_page_cut * (FLASH_MAX_PAGE_SIZE_8));
            STMFLASH_WriteMultipleBytes(worte_address, firmware_buff_16,FLASH_MAX_PAGE_SIZE_16);
            SEGGER_RTT_printf(0,"write address:%7X , write size:%d  \n", worte_address, FLASH_MAX_PAGE_SIZE_16);


            
            memset(firmware_buff_8,0xff,sizeof(firmware_buff_8));
            memset(firmware_buff_16,0xff,sizeof(firmware_buff_16));
            firmware_size_cut = 0;
            firmware_page_cut++;
        }

        // ��ǰ�ְ��� = ���ְ��ŵ�ʱ�� ����������
        // ��������Ҳ��ȥдһ�ι̼���
        if(firmware_split_number == firmware_split_max )
        {           

            convertUint8ToUint16(firmware_buff_8, firmware_buff_16, FLASH_MAX_PAGE_SIZE_8, FLASH_MAX_PAGE_SIZE_16);
            worte_address = FLASH_APP_ADDR + (firmware_page_cut * (FLASH_MAX_PAGE_SIZE_8));
            

            STMFLASH_WriteMultipleBytes(worte_address, firmware_buff_16,FLASH_MAX_PAGE_SIZE_16);
            
            SEGGER_RTT_printf(0,"write address:%7X , write size:%d \n", worte_address, FLASH_MAX_PAGE_SIZE_16);

            
            memset(firmware_buff_8,0xff,sizeof(firmware_buff_8));
            memset(firmware_buff_16,0xff,sizeof(firmware_buff_16));
            firmware_size_cut = 0;
            // �����������ܵ��˹̼�
            is_update_success = 1;
        }
     
      
    }
    if(cmd_id == 0x02)
    {
        ota_send_appVersionInfo();

    }
    /** ���� �������� **/
    if(isack == 0x01)
    {
        uart_send_ack_data();
        SEGGER_RTT_printf(0,"isackisackisack\r\n");

    }
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
            HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart1_rx_buff, USRT1_RX_BUFF_SIZE); //������ʼDMA���� ÿ��255�ֽ�����

        }
    }
}
// flicker_cut = 1 = 40ms
void led_flicker(uint16_t flicker_cut,uint32_t delay)
{
    for(uint16_t i = 0; i<flicker_cut; i++)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        HAL_Delay(delay);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        HAL_Delay(delay);
    }
}
#define INFLASH_START_ADDR      ((uint32_t)0x8000000)  // STM32 �ڲ�FLASH����ʼ��ַ
#define INFLASH_VTOR_OFFSET     ((uint32_t)0x8000000)  // APP�������ƫ�Ƶ�ַ����APP����ʼ��ַ����һ��
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
    memset(firmware_buff_8,0xff,sizeof(firmware_buff_8));
    memset(firmware_buff_16,0xff,sizeof(firmware_buff_16));

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    SEGGER_RTT_Init();
    
    // ���App�̼�״̬��־��crcУ��͡��̼���С
    /** ��һ���ϵ��bl��ȡ���������������0xff, 
            �Ǿͱ�ʾ bl��app��¼��ȥ�󣬵�һ�ν���bl ��ת����app����
    **/
    //  �ڶ����ϵ��bl��ȡ �̼�״̬��־0x00ֱ����ת����App
    /** App�̼�״̬��־
        0xffff ��ʾApp������¼�� ��ת��������
        0x0001 ��ʾApp��������
        0x0002 ��ʾApp������̼�����Ҫ�������ɹ���Ϣ���͸�esp���ϱ��汾��3��������ɺ�״̬����Ϊ0x0001
        0x0003 ��ʾApp�̼���Ҫ����  ������д�� �̼���־д��0xffff д��̼�У����Լ��̼���С
    **/



    if(app_otaGetFirmwareState() == 0xffff || app_otaGetFirmwareState() == 0x0001)
    {
        SEGGER_RTT_printf(0, "BootLoader jump to App...\r\n");         
        led_flicker(10,40);
        IAP_LoadApp(FLASH_APP_ADDR); //������ת
        // App���������־λΪ 0xffff ��ʱ��,App��Ҫд��汾�š��Լ���־λ����Ϊ0x0001
    }
    if(app_otaGetFirmwareState() == 0x0003)
    {
        // app ��ת������ ��Ҫ����
        for(uint8_t i = 0;i<3;i++)
        {
            ota_send_appVersionInfo();
            led_flicker(1,50);
        }
        SEGGER_RTT_printf(0, "App to BootLoader success\r\n");  
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        //SEGGER_RTT_printf(0, "Hello i is bootLoader !\r\n");  
        HAL_Delay(1000);
        if(is_update_success == 1)
        {
            is_update_success = 0;
            // App�̼�״̬��־,��App��Ҫ�ϱ������ɹ���Ϣ������Ϊ0x0001������������������
            app_otaSetFirmwareState(0x0002);         
            HAL_Delay(1000);
            IAP_LoadApp(FLASH_APP_ADDR); //������ת
        }
        //    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        //   
        //    //HAL_Delay(1111);
         
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
