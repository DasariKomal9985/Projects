void LM35_TEMP(void const * argument)
{
  /* USER CODE BEGIN LM35_TEMP */
  /* Infinite loop */
  for(;;)
  {
		
		 HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1,10);
			raw_adc=HAL_ADC_GetValue(&hadc1); // digtal value range is 0 to 4095;
			temp=(raw_adc/12); // ANALOG SUPPLY - Reference Volatage - 3.6V ; 
			sprintf(Temp_Value, "%d", temp);
			RM_LCD_Goto(10,0);
			RM_LCD_PutStr("T:");
			RM_LCD_PutStr(Temp_Value);
			RM_LCD_Put_Char(0xDF);
			RM_LCD_Put_Char('C');
			lm35_flag=1;
		//	sprintf(Tx_Buff,"%d",temp);
	
			osDelay(5000);
  }
  /* USER CODE END LM35_TEMP */
}
