void DS1307_RTC(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		 
		/* Date & Time Write in to RTC  */
//	   HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x00,1, (uint8_t *)&Mem_Write_data[0],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x01,1, (uint8_t *)&Mem_Write_data[1],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x02,1, (uint8_t *)&Mem_Write_data[2],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x03,1, (uint8_t *)&Mem_Write_data[3],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x04,1, (uint8_t *)&Mem_Write_data[4],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x05,1, (uint8_t *)&Mem_Write_data[5],1,1000);
//		 HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x06,1, (uint8_t *)&Mem_Write_data[6],1,1000);
		
		
		
		HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x00,1, (uint8_t *)&Time_Date[0],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x01,1, (uint8_t *)&Time_Date[1],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x02,1, (uint8_t *)&Time_Date[2],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x03,1, (uint8_t *)&Time_Date[3],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x04,1, (uint8_t *)&Time_Date[4],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x05,1, (uint8_t *)&Time_Date[5],1,1000);
			HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x06,1, (uint8_t *)&Time_Date[6],1,1000);
			RM_LCD_Goto(0,0);
			//RM_LCD_PutStr("T:");
			sprintf(Temp_Value, "%d",Time_Date[2]-6*(Time_Date[2]>>4));
		  RM_LCD_PutStr(Temp_Value);
		  RM_LCD_Write_DATA(':');
		  sprintf(Temp_Value, "%d",Time_Date[1]-6*(Time_Date[1]>>4));
		  RM_LCD_PutStr(Temp_Value);
		  RM_LCD_Write_DATA(':');
		  sprintf(Temp_Value, "%d",Time_Date[0]-6*(Time_Date[0]>>4));
		  RM_LCD_PutStr(Temp_Value);
			RM_LCD_Goto(0,1);
			//RM_LCD_PutStr("D:");
			sprintf(Temp_Value, "%d",Time_Date[4]-6*(Time_Date[4]>>4));
			RM_LCD_PutStr(Temp_Value);
			RM_LCD_Write_DATA(':');
			sprintf(Temp_Value, "%02d",Time_Date[5]-6*(Time_Date[5]>>4));
			RM_LCD_PutStr(Temp_Value);
			RM_LCD_Write_DATA(':');
			sprintf(Temp_Value, "%d",Time_Date[6]-6*(Time_Date[6]>>4));
			RM_LCD_PutStr(Temp_Value);
			//HAL_Delay(300);
		//HAL_GPIO_TogglePin(GPIOB,RED_LED_Pin);

		
		  osDelay(1000);
		 		//HAL_GPIO_TogglePin(GPIOB,GREEN_LED_Pin);

		/* Date & Time Write in to RTC  */
	 
 }
	
  /* USER CODE END 5 */
}
