/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "komal_lcd_driver.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define RED_LED_PIN       GPIO_PIN_13
#define GREEN_LED_PIN     GPIO_PIN_14
#define LED_PORT          GPIOB

#define LED_DELAY_MS    2000U
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Definitions for Led_Task_01 */
osThreadId_t Led_Task_01Handle;
const osThreadAttr_t Led_Task_01_attributes = { .name = "Led_Task_01",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for Lcd_Task_02 */
osThreadId_t Lcd_Task_02Handle;
const osThreadAttr_t Lcd_Task_02_attributes = { .name = "Lcd_Task_02",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for Uart_Task_03 */
osThreadId_t Uart_Task_03Handle;
const osThreadAttr_t Uart_Task_03_attributes = { .name = "Uart_Task_03",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void Start_Led_Task01(void *argument);
void Start_Lcd_Task02(void *argument);
void Start_Uart_Task03(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void UART_Debug(char *message) {
	HAL_UART_Transmit(&huart2, (uint8_t*) message, strlen(message),
			HAL_MAX_DELAY);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	LCD_Init();

	LCD_Clear();

	LCD_SetCursor(1, 1);
	LCD_String("RED LED: OFF   ");

	LCD_SetCursor(2, 1);
	LCD_String("GREEN LED: OFF   ");

	UART_Debug("\r\n");
	UART_Debug("Project_01 \r\n");
	UART_Debug("Red LED Pin 13 \r\n");
	UART_Debug("Green LED Pin 14 \r\n");
	UART_Debug("LCD Pins PB0 PB1 PB2 PB3 PB4 PB5 PB8 \r\n");
	UART_Debug("RTOS Tasks\r\n");
	UART_Debug("UART Debugging Pin PA2 and PA3 115200 baud rate \r\n");

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

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
	/* creation of Led_Task_01 */
	Led_Task_01Handle = osThreadNew(Start_Led_Task01, NULL,
			&Led_Task_01_attributes);

	/* creation of Lcd_Task_02 */
	Lcd_Task_02Handle = osThreadNew(Start_Lcd_Task02, NULL,
			&Lcd_Task_02_attributes);

	/* creation of Uart_Task_03 */
	Uart_Task_03Handle = osThreadNew(Start_Uart_Task03, NULL,
			&Uart_Task_03_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_13 | GPIO_PIN_14
					| GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PB0 PB1 PB2 PB13
	 PB14 PB3 PB4 PB5
	 PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_13
			| GPIO_PIN_14 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Start_Led_Task01 */
/**
 * @brief  Function implementing the Led_Task_01 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_Led_Task01 */
void Start_Led_Task01(void *argument) {
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	(void) argument;

	for (;;) {
		HAL_GPIO_WritePin(LED_PORT, RED_LED_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
		osDelay(LED_DELAY_MS);
		HAL_GPIO_WritePin(LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
		osDelay(LED_DELAY_MS);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Start_Lcd_Task02 */
/**
 * @brief Function implementing the Lcd_Task_02 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_Lcd_Task02 */
void Start_Lcd_Task02(void *argument) {
	/* USER CODE BEGIN Start_Lcd_Task02 */
	/* Infinite loop */
	GPIO_PinState redState;
	GPIO_PinState greenState;
	(void) argument;
	for (;;) {
		redState = HAL_GPIO_ReadPin(LED_PORT, RED_LED_PIN);
		greenState = HAL_GPIO_ReadPin(LED_PORT, GREEN_LED_PIN);
		LCD_SetCursor(1, 1);
		if (redState == GPIO_PIN_SET) {
			LCD_String("RED LED: ON OK  ");
		} else {
			LCD_String("RED LED: OFF  ");
		}
		LCD_SetCursor(2, 1);
		if (greenState == GPIO_PIN_SET) {
			LCD_String("GREEN LED: ON OK  ");
		} else {
			LCD_String("GREEN LED: OFF  ");
		}
		osDelay(100);
	}
	/* USER CODE END Start_Lcd_Task02 */
}

/* USER CODE BEGIN Header_Start_Uart_Task03 */
/**
 * @brief Function implementing the Uart_Task_03 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_Uart_Task03 */
void Start_Uart_Task03(void *argument) {
	/* USER CODE BEGIN Start_Uart_Task03 */
	/* Infinite loop */
	GPIO_PinState redState;
	GPIO_PinState greenState;

	(void) argument;

	for (;;) {
		redState = HAL_GPIO_ReadPin(LED_PORT, RED_LED_PIN);
		greenState = HAL_GPIO_ReadPin(LED_PORT, GREEN_LED_PIN);

		if (redState == GPIO_PIN_SET) {
			UART_Debug("[LED] RED LED    : ON\r\n");
		} else {
			UART_Debug("[LED] RED LED    : OFF\r\n");
		}
		if (greenState == GPIO_PIN_SET) {
			UART_Debug("[LED] GREEN LED    : ON\r\n");
		} else {
			UART_Debug("[LED] GREEN LED    : OFF\r\n");
		}
		osDelay(100);
	}
	/* USER CODE END Start_Uart_Task03 */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM9 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM9) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
