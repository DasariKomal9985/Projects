/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "lcd.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
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
UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osSemaphoreId myBinarySem01Handle;
osSemaphoreId myBinarySem02Handle;
osSemaphoreId myBinarySem03Handle;
/* USER CODE BEGIN PV */
uint8_t Time_Date[8];
uint8_t Mem_Write_data[8]={0x00,0x56,0x10,0x06,0x30,0x11,0x23};
 char raw_data[1024];
 char gprmc_buffer[128];
int temp,number=2,Temp,hum=75;
unsigned char Fetch[50];
uint8_t Tx_Buff[8];
uint8_t raw_data_index = 0;
uint8_t gprmc_index = 0;
char I2c_Number_Buf[10],Rx_Buff[100];;
char Temp_Value[2],value[7];
unsigned char str1[50];
char str[50];
long double lon,lat,ti;
double longitude;
double latitude;
double altitude;
double lat_hemisphere;
double lon_hemisphere;
char time[10]; // for HH:MM:SS format
char link[100];
int arr[6];
char rtc[7];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void Task1_GPS_Data(void const * argument);
void Task2_GPS_Data_Process(void const * argument);
void Task3_GSM_SMS(void const * argument);
void Task4_STS_Config(void const * argument);

/* USER CODE BEGIN PFP */
int response(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void extract_gprmc(char *raw_data, char *gprmc_buffer) {
  uint8_t raw_data_index = 0;
  uint8_t gprmc_index = 0;

  while (raw_data[raw_data_index] != '\0')
		{
    if (strncmp(raw_data + raw_data_index, "$GPRMC", 6) == 0) {
      strncpy(gprmc_buffer, raw_data + raw_data_index, strlen(raw_data) - raw_data_index);
      gprmc_index = strlen(raw_data) - raw_data_index;
      raw_data_index += gprmc_index;
      break;
    }
    raw_data_index++;
  }
  gprmc_buffer[gprmc_index] = '\0';
}

long double extract(double input)
{
	int a,b,c;
	int degrees = (int)input;
	a=degrees/100;
	printf("Degrees : %d\n", a); // Output: 78

	b=degrees%100;
	printf("Degrees: %d\n", b); // Output: 78

	float remaining = input - degrees;
	c=remaining*100;
	printf("seconds: %d\n", c); // Output: 78

	long double res=a+b/60.0+c/3600.0;
	return res;
}

void parseGPRMC(char *gprmc_sentence, char *time_utc, double *latitude, double *longitude)
	{
    char *token = strtok(gprmc_sentence, ",");
    int i = 0;

    while (token != NULL)
    {
        switch (i) {
            case 1:
                strcpy(time_utc, token);
                break;
            case 3:
                *latitude = atof(token);
                break;
            case 5:
                *longitude = atof(token);
                break;
        }

        token = strtok(NULL, ",");
        i++;
    }
	}



void addTime(int arr[])
{
    // Extract hours, minutes, and seconds from the input array
    int hours = arr[0] * 10 + arr[1];
    int minutes = arr[2] * 10 + arr[3];
    int seconds = arr[4] * 10 + arr[5];

    // Add 5 hours and 30 minutes
    hours += 5;
    minutes += 30;

    // Adjust hours and minutes if necessary
    if (minutes >= 60)
    {
        hours += minutes / 60;
        minutes %= 60;
    }
    if (hours >= 24)
    {
        hours %= 24;
    }

    // Update the input array with the new time
    arr[0] = hours / 10;
    arr[1] = hours % 10;
    arr[2] = minutes / 10;
    arr[3] = minutes % 10;
    arr[4] = seconds / 10;
    arr[5] = seconds % 10;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int Size=50,Timeout=300;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  RM_LCD_Init();
   RM_LCD_Goto(5,1);
      RM_LCD_PutStr("V1.0");
   	 const char message[] = "SMART TRACKING SYSTEM   ";
   for (int i = 0; message[i] != '\0'; i++)
   	{
   		RM_LCD_Goto(0,0);
   		if(message[i]=='M' && message[i+1]==' ')
   			break;
   		sprintf(str, "%s",&message[i]);
   			RM_LCD_PutStr(str);
   			HAL_Delay(200);
   }
     RM_LCD_Clear();
     HAL_Delay(500);
  memset(raw_data,'\0',1024);


  RM_LCD_Goto(0,0);
	     RM_LCD_PutStr("SW_ENT->For GPS ");
	      RM_LCD_Goto(0,1);
	      RM_LCD_PutStr("SW_UP->Chng M.No");
	      HAL_Delay(1000);
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* definition and creation of myBinarySem02 */
  osSemaphoreDef(myBinarySem02);
  myBinarySem02Handle = osSemaphoreCreate(osSemaphore(myBinarySem02), 1);

  /* definition and creation of myBinarySem03 */
  osSemaphoreDef(myBinarySem03);
  myBinarySem03Handle = osSemaphoreCreate(osSemaphore(myBinarySem03), 1);

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, Task1_GPS_Data, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, Task2_GPS_Data_Process, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, Task3_GSM_SMS, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, Task4_STS_Config, osPriorityIdle, 0, 128);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  huart1.Init.BaudRate = 9600;
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

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
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
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB12
                           PB13 PB14 PB3 PB4
                           PB5 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task1_GPS_Data */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task1_GPS_Data */
void Task1_GPS_Data(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	osSemaphoreWait(myBinarySem01Handle, osWaitForever);
  for(;;)
  {
	      if((HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)) == 0)
	      {
	        RM_LCD_Clear();
	        RM_LCD_PutStr("Getting GPS DATA");
	        HAL_Delay(100);
	        osSemaphoreRelease(myBinarySem02Handle);
	         // Release the semaphore to unlock Task1
	        osSemaphoreRelease(myBinarySem03Handle); // Release the semaphore for the next iteration
	        osDelay(2000);
	      }
  	  	 }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task2_GPS_Data_Process */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task2_GPS_Data_Process */
void Task2_GPS_Data_Process(void const * argument)
{
  /* USER CODE BEGIN Task2_GPS_Data_Process */
  /* Infinite loop */
	osSemaphoreWait(myBinarySem02Handle, osWaitForever);
  for(;;)
  {int j=0;

 HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_13);
	  	    HAL_UART_Receive(&huart1,raw_data,1024,500);
	  	    RM_LCD_Clear();
	  	  HAL_Delay(100);
	  	uint8_t raw_data_index = 0;
	  	  uint8_t gprmc_index = 0;

	  	  while (raw_data[raw_data_index] != '\0')
	  			{
	  	    if (strncmp(raw_data + raw_data_index, "$GPRMC", 6) == 0) {
	  	      strncpy(gprmc_buffer, raw_data + raw_data_index, strlen(raw_data) - raw_data_index);
	  	      gprmc_index = strlen(raw_data) - raw_data_index;
	  	      raw_data_index += gprmc_index;
	  	      break;
	  	    }
	  	    raw_data_index++;
	  	 }
	  	  gprmc_buffer[gprmc_index] = '\0';
	        RM_LCD_PutStr("TASK 2");
	        HAL_Delay(100);
	  		if (strlen(gprmc_buffer) > 0)
	  		{
	  			RM_LCD_Clear();
	  			RM_LCD_PutStr("DATA extracted");
	  			 HAL_Delay(100);
	  			parseGPRMC(gprmc_buffer, time, &latitude, &longitude);
	  			lat=extract(latitude);
	  			lon=extract(longitude);
	  			    for (int i = 0; i < 6; ++i)
	  			    {
	  			        arr[i] = time[i] - '0';
	  			    }
	  			    addTime(arr);
	  			    RM_LCD_Clear();
	  			  RM_LCD_Goto(0,0);
	  			    for (int i = 0,j=0; i < 6; ++i,j++)
	  			    {
	  			    	rtc[j] = arr[i] + '0';
	  			    	if(i==2 || i==4)
	  			    	{
	  			    		rtc[j]=':';
	  			    		j++;
	  			    		rtc[j] = arr[i] + '0';
	  			    	}


	  			    }
	  			RM_LCD_PutStr(rtc);

	  			 HAL_Delay(1000);
	  			sprintf(link,"https://www.google.com/maps/@%.7Lf,%.7Lf,15z?entry=ttu",lat,lon);
	  			 HAL_Delay(100);
	  			memset(raw_data,'\0',1024);
	  			osSemaphoreRelease(myBinarySem03Handle);
	  			osDelay(1000);
	       }
  }
  /* USER CODE END Task2_GPS_Data_Process */
}

/* USER CODE BEGIN Header_Task3_GSM_SMS */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task3_GSM_SMS */
void Task3_GSM_SMS(void const * argument)
{
  /* USER CODE BEGIN Task3_GSM_SMS */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreWait(myBinarySem03Handle, osWaitForever);
	  // Assuming link is a null-terminated string containing the URL

	  // AT command to set SMS text mode
	  HAL_UART_Transmit(&huart2, "AT+CMGF=1\r\n", strlen("AT+CMGF=1\r\n"), 300);
	  HAL_Delay(1000);

	  // AT command to set the recipient phone number
	  HAL_UART_Transmit(&huart2, "AT+CMGS=\"9985798499\"\r\n", strlen("AT+CMGS=\"9985798499\"\r\n"), 300);
	  HAL_Delay(1000);

	  // Sending the actual SMS content (the link)
	  HAL_UART_Transmit(&huart2, link, strlen(link), 300);

	  // Sending Ctrl+Z to indicate the end of the SMS
	  HAL_UART_Transmit(&huart2, "\x1A", 1, 300);

	  // Wait for the GSM module to process the SMS
	  HAL_Delay(2000);  // You may need to adjust this delay based on your GSM module and network
	  osDelay(1000);
  }
  /* USER CODE END Task3_GSM_SMS */
}

/* USER CODE BEGIN Header_Task4_STS_Config */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task4_STS_Config */
void Task4_STS_Config(void const * argument)
{
  /* USER CODE BEGIN Task4_STS_Config */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Task4_STS_Config */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#endif /* USE_FULL_ASSERT */
