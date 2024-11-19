void Cloud_Server(void const * argument)
{
  /* USER CODE BEGIN Cloud_Server */
  /* Infinite loop */
  for(;;)
  {
		if((inet_flag == 1) && (lm35_flag == 1))
		{
			inet_flag = 0;
			lm35_flag = 0;
		//	HAL_GPIO_TogglePin(GPIOB,Buzzer_Pin);
		//	HAL_Delay(2000);
			HAL_GPIO_TogglePin(GPIOB,RED_LED_Pin);
			HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CIPSTART=\"TCP\",\"142.93.218.33\",80\r\n", 44, 1000);
			osDelay(100);
			sprintf(Fetch,"GET /page.php?temp=%d&hum=55&dev=13\r\n\r\n",temp);
			HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CIPSEND=40\r\n", 17, 1000);
			osDelay(100);
			HAL_UART_Transmit(&huart6, (uint8_t *)Fetch, 43, 1000);
			HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CIPCLOSE\r\n", 15, 1000);
			osDelay(100);
			HAL_GPIO_TogglePin(GPIOB,GREEN_LED_Pin);

	}
  }
  /* USER CODE END Cloud_Server */
}