void EEPROM(void const * argument)
{
  /* USER CODE BEGIN EEPROM */
	WiFi_Init();
  /* Infinite loop */
  for(;;)
  {
		HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CIPSTATUS\r\n", 16, 1000);
	  memset((uint8_t *)Rx_buff, 0, sizeof(Rx_buff));
	  HAL_UART_Receive(&huart6, (uint8_t *)Rx_buff, 100, 1000);
		ptr=strchr(Rx_buff, ':');
		//ptr = Rx_buff;
		
		if(ptr[1] =='5')
		{
			inet_flag=0;
			RM_LCD_Goto(11,1);
		  RM_LCD_PutStr("ERROR");
			HAL_I2C_Mem_Write(&hi2c1,(0x50<<1),0,1,Tx_Buff,14,1000);
			osDelay(200);
			HAL_I2C_Mem_Read(&hi2c1,(0x50<<1),0,1, (uint8_t *)&Rx_Buff, 14,1000);
			osDelay(200);
		}
		else
		{
			inet_flag=1;
			RM_LCD_Goto(11,1);
		  RM_LCD_PutStr("OK   ");	
      osDelay(200);
		}
  }
  /* USER CODE END EEPROM */
}