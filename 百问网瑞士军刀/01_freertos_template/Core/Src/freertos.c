/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro 私有的全局变量 -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
static StackType_t    g_puxLedStackBuffer[128]; // 静态分配的栈
static StaticTask_t   g_pxLedTaskBuffer;		// 静态分配任务的结构体 用它操作任务

static StackType_t    g_puxRGBStackBuffer[128]; // 静态分配的栈
static StaticTask_t   g_pxRGBTaskBuffer;		// 静态分配任务的结构体 用它操作任务


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
struct TaskPrintInfo{
	uint8_t x;
	uint8_t y;
	char name[16];
};
static struct TaskPrintInfo tpi1 = {0,0,"task1"};
static struct TaskPrintInfo tpi2 = {0,3,"task2"};
static struct TaskPrintInfo tpi3 = {0,6,"task3"};
static uint8_t isLcd = 1; // 简易互斥
void LCD_PrintTask(void *parame)
{
	struct TaskPrintInfo *tpi = parame;
	uint8_t len = 0;
	uint32_t count = 0;
	while(1)
	{
		if(isLcd)
		{
			isLcd = 0;
			len = LCD_PrintString(tpi->x, tpi->y, tpi->name);
			len += LCD_PrintString(len, tpi->y, ":");
			LCD_PrintSignedVal(len, tpi->y, count++);
			isLcd = 1;
		}
		mdelay(100);
	}
	
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
// 一个测试的任务
void vMusicTask(void *argument){
	Led_Test();
}
/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  LCD_Init();
  LCD_Clear();
 

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  // 5的课程
  // 默认创建的任务
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
//  //      任务函数  任务的名称    栈的深度  任务参数  优先级    句柄
//  // 声音 动态分配的任务
//  extern void PlayMusic(void *params);
//  xTaskCreate(PlayMusic,"PlayMusic", 128, NULL, osPriorityNormal, NULL);
//  // led 灯任务 静态分配的
//  xTaskCreateStatic(Led_Test,"ledTestTask",128,NULL,osPriorityNormal,g_puxLedStackBuffer,&g_pxLedTaskBuffer);
//  // rgb 灯任务 静态分配的
//  xTaskCreateStatic(ColorLED_Test,"rgbTestTask",128,NULL,osPriorityNormal,g_puxRGBStackBuffer,&g_pxRGBTaskBuffer);
  // 5的课程

// 6 的课程
	xTaskCreate(LCD_PrintTask,"LCD_PrintTask1", 128, &tpi1, osPriorityNormal, NULL);
	xTaskCreate(LCD_PrintTask,"LCD_PrintTask2", 128, &tpi2, osPriorityNormal, NULL);
	xTaskCreate(LCD_PrintTask,"LCD_PrintTask3", 128, &tpi3, osPriorityNormal, NULL);

// 6 的课程
  
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  LCD_Init();
  LCD_Clear();
  
  for(;;)
  {
    //Led_Test();
    LCD_Test();
	//MPU6050_Test(); 
	//DS18B20_Test();
	//DHT11_Test();
	//ActiveBuzzer_Test();
	//PassiveBuzzer_Test();
	//ColorLED_Test();
	//IRReceiver_Test();
	//IRSender_Test();
	//LightSensor_Test();
	//IRObstacle_Test();
	//SR04_Test();
	//W25Q64_Test();
	//RotaryEncoder_Test();
	//Motor_Test();
	//Key_Test();
	//UART_Test();
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

