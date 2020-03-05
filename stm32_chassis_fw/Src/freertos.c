/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
/* --- task --- */
#include "chassis.h"
#include "emergency.h"
#include "period.h"
#include "command.h"
#include "support.h"
#include "signal.h"
/* --- signal --- */

/* --- user lib --- */
#include "nrf24l01.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for nAppTask */
osThreadId_t nAppTaskHandle;
const osThreadAttr_t nAppTask_attributes = {
  .name = "nAppTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTaskCreate(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  chassis_init();
  NRF24L01_Init();/* --- to do list : check false also unblock  --- */
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
  /* creation of nAppTask */
  nAppTaskHandle = osThreadNew(AppTaskCreate, NULL, &nAppTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  
	/* --- creat other task --- */
  //taskENTER_CRITICAL(); //进入临界
	emergency_init();
	canread_init();
	period_init();
	command_init();
	support_init();
  //taskEXIT_CRITICAL();	
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_AppTaskCreate */
/**
  * @brief  Function implementing the nAppTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_AppTaskCreate */
void AppTaskCreate(void *argument)
{
  /* USER CODE BEGIN AppTaskCreate */

//	taskENTER_CRITICAL(); //进入临界
//	/* --- creat other task --- */
//	chassis_init();
//	emergency_init();
//	canread_init();
//	period_init();
//	command_init();
//	support_init();
//	//vTaskDelete(AppTaskCreate);
//	taskEXIT_CRITICAL();	
  for(;;)
  {
    vTaskDelay(10);
  }
  /* USER CODE END AppTaskCreate */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
