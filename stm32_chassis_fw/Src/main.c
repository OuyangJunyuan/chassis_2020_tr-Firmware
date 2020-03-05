/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "adc.h"
#include "can.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANCommute.h"
#include "ChassisInit.h"
#include "Calculate.h"
#include "oled12864.h"
#include "led.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t pwm1=500,pwm2=0,pwm3=0,adcvalue;
uint8_t rxbuff[32];
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef   RxHeader;
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CanReceiveData) != HAL_OK)
  {
    Error_Handler();            //错误状濁回调函敿
  }
  CanDataReceive(RxHeader.StdId);   //根据电机号对CAN接收进行解码
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
	ChassisInit();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM5_Init();
  MX_SPI3_Init();
  MX_ADC3_Init();
  MX_CAN1_Init();
  MX_SPI2_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
//	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_2);
//	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
	holed.Init();
	hled.Init();
	NRF24L01_Init();
	CanFilterInit(&hcan1);  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_2);
//		HAL_Delay(200);
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_3);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_5);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_4);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_5);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_6);
//		HAL_Delay(20);
//		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_7);
//		HAL_Delay(20); 	
//		HAL_ADC_Start(&hadc3);
//		adcvalue=HAL_ADC_GetValue(&hadc3);
//		printf("adc:%d\n",adcvalue);
//		holed.Draw.BMP(bmp1);
			

		FourWheelVellControl(	2500,
												0,
												0);
		Calculate();
		SendMotor();
		CanDataReceive(CAN_CHASSIS_MOTOR1_ID);
		CanDataReceive(CAN_CHASSIS_MOTOR2_ID);
		CanDataReceive(CAN_CHASSIS_MOTOR3_ID);
		CanDataReceive(CAN_CHASSIS_MOTOR4_ID);
//		 hled.Doing();
//		 while(NRF24L01_Recieve(rxbuff)!=STATUS_RX_OK);
//		 printf("%d %d %d\n",rxbuff[0],(int)(rxbuff[1]),(int)(rxbuff[2]));
//		 holed.Set.Refresh();

		printf(" WTR_board-2.0 \n");
		printf("remote:%s",hnrf24l01.status!=NRF24L01_STATUS_DISCONNECT?"false":"true");
		printf("\nlinear:");
		printf("\nangular:");
		holed.Set.Refresh();
		holed.Draw.Clear();
		HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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
	while(1);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
