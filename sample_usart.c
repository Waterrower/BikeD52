

void displaySend(unsigned char arr[], unsigned int len) {
	for (unsigned int i = 0; i < len; i++) {
		while (!(UCSR1A & (1<<UDRE1))); // Wait for empty transmit buffer
		UDR1 = arr[i]; // Send data
	}
}



void displayShowImage(uint8_t image){
	if (image<255){
		uint8_t App_LightDarkMode_Images [][2]= {
			
			#ifdef BIKE
			
			{IMG_BG_GRID_LIGHT,IMG_BG_GRID_DARK},
			{IMG_BG_HALFGRID_LIGHT,IMG_BG_HALFGRID_DARK},
			{IMG_BG_LIGHT,IMG_BG_DARK},
			#else
			{IMG_BG_GRID_LIGHT,IMG_BG_GRID_DARK},
			{IMG_BG_HALFGRID_LIGHT,IMG_BG_DARK_HT40_NEUTRAL},
			#endif
			{IMG_BG_LIGHT_HT40_NEUTRAL,IMG_BG_DARK_HT40_NEUTRAL},
			{IMG_TILE1_TIME_ICON_LIGHT,IMG_TILE1_TIME_ICON_DARK},
			{IMG_TILE2_DISTANCE_ICON_LIGHT,IMG_TILE2_DISTANCE_ICON_DARK},
			{IMG_TILE3_FREE_ICON_LIGHT, IMG_TILE3_FREE_ICON_DARK},
			{IMG_TILE4_SETTINGS_ICON_LIGHT,IMG_TILE4_SETTINGS_ICON_DARK},
			{IMG_HALFTILE41_MINUS_ICON_LIGHT,IMG_HALFTILE41_MINUS_ICON_DARK},
			{IMG_HALFTILE40_PLUS_ICON_LIGHT,IMG_HALFTILE40_PLUS_ICON_DARK},
			{IMG_HALFTILE40_BLANK_LIGHT,IMG_HALFTILE40_BLANK_DARK},
			{IMG_HALFTILE41_LEFT_ARROW_ICON_LIGHT,IMG_HALFTILE41_LEFT_ARROW_ICON_DARK},
			//{IMG_TILE0_BLANK_LIGHT,IMG_TILE0_BLANK_DARK},
			//{IMG_TILE0_FRAME_LIGHT,IMG_TILE0_FRAME_DARK},
			{IMG_TILE1_BLANK_LIGHT,IMG_TILE1_BLANK_DARK},
			{IMG_TILE2_BLANK_LIGHT,IMG_TILE2_BLANK_DARK},
			{IMG_TILE3_BLANK_LIGHT,IMG_TILE3_BLANK_DARK},
			{IMG_TILE4_BLANK_LIGHT,IMG_TILE4_BLANK_DARK},
			
			{IMG_TILE1_FRAME_LIGHT,IMG_TILE1_FRAME_DARK},
			{IMG_TILE2_FRAME_LIGHT,IMG_TILE2_FRAME_DARK},
			{IMG_TILE3_FRAME_LIGHT,IMG_TILE3_FRAME_DARK},
			{IMG_TILE4_FRAME_LIGHT,IMG_TILE4_FRAME_DARK},
			{IMG_HALFTILE10_FRAME_LIGHT,IMG_HALFTILE10_FRAME_DARK},
			{IMG_HALFTILE11_FRAME_LIGHT,IMG_HALFTILE11_FRAME_DARK},
			{IMG_HALFTILE20_FRAME_LIGHT,IMG_HALFTILE20_FRAME_DARK},
			{IMG_HALFTILE21_FRAME_LIGHT,IMG_HALFTILE21_FRAME_DARK},
			{IMG_HALFTILE30_FRAME_LIGHT,IMG_HALFTILE30_FRAME_DARK},
			{IMG_HALFTILE31_FRAME_LIGHT,IMG_HALFTILE31_FRAME_DARK},
			{IMG_HALFTILE40_FRAME_LIGHT,IMG_HALFTILE40_FRAME_DARK},
			{IMG_HALFTILE41_FRAME_LIGHT,IMG_HALFTILE41_FRAME_DARK},
			//{IMG_HALFTILE30_MINUS_ICON_LIGHT,IMG_HALFTILE30_MINUS_ICON_DARK},
			//{IMG_HALFTILE11_PLUS_ICON_LIGHT,IMG_HALFTILE11_PLUS_ICON_DARK},
			{IMG_HALFTILE41_LEFT_ARROW_ICON_LIGHT,IMG_HALFTILE41_LEFT_ARROW_ICON_DARK},
			{IMG_HALFTILE10_BLANK_LIGHT,IMG_HALFTILE10_BLANK_DARK},
			{IMG_HALFTILE11_BLANK_LIGHT,IMG_HALFTILE11_BLANK_DARK},
			{IMG_HALFTILE30_BLANK_LIGHT,IMG_HALFTILE30_BLANK_DARK},
			{IMG_HALFTILE31_BLANK_LIGHT,IMG_HALFTILE31_BLANK_DARK},
			{IMG_HALFTILE41_BLANK_LIGHT,IMG_HALFTILE41_BLANK_DARK},
			
			{IMG_TILE1_FRAME_LIGHT,IMG_TILE1_FRAME_DARK},
			{IMG_TILE2_FRAME_LIGHT,IMG_TILE2_FRAME_DARK},
			{IMG_TILE3_FRAME_LIGHT,IMG_TILE3_FRAME_DARK},
			{IMG_HALFTILE10_FRAME_LIGHT,IMG_HALFTILE10_FRAME_DARK},
			{IMG_HALFTILE11_FRAME_LIGHT,IMG_HALFTILE11_FRAME_DARK},
			{IMG_HALFTILE20_FRAME_LIGHT,IMG_HALFTILE20_FRAME_DARK},
			{IMG_HALFTILE21_FRAME_LIGHT,IMG_HALFTILE21_FRAME_DARK},
			{IMG_HALFTILE30_FRAME_LIGHT,IMG_HALFTILE30_FRAME_DARK},
			{IMG_HALFTILE31_FRAME_LIGHT,IMG_HALFTILE31_FRAME_DARK},
			{IMG_TILE2_SUN_ICON_LIGHT,IMG_TILE2_MOON_ICON_DARK},
			{IMG_TILE2_HR_ICON_LIGHT,IMG_TILE2_HR_ICON_DARK},
			{IMG_TILE4_NEXT_ICON_LIGHT,IMG_TILE4_NEXT_ICON_DARK},
			{IMG_TILE4_HOME_LIGHT,IMG_TILE4_HOME_DARK},
			{IMG_TILE1_INFO_LIGHT,IMG_TILE1_INFO_DARK},
			{IMG_TILE3_BT_ICON_OFF_LIGHT,IMG_TILE3_BT_ICON_OFF_DARK},
			{IMG_TILE2_CALIBRATE_UP_LIGHT,IMG_TILE2_CALIBRATE_UP_DARK},
			{IMG_HALFTILE41_STOP_ICON_LIGHT,IMG_HALFTILE41_STOP_ICON_DARK},
			{IMG_HALFTILE41_HOME_ICON_LIGHT,IMG_HALFTILE41_HOME_ICON_DARK},
			{IMG_HALFTILE41_DISCONBT_LIGHT,IMG_HALFTILE41_DISCONBT_DARK},
			{IMG_TILE2_CANCEL_ICON_LIGHT,IMG_TILE2_CANCEL_ICON_DARK},
			{IMG_HALFTILE41_PAUSED_ICON_LIGHT,IMG_HALFTILE41_PAUSED_ICON_DARK},
			{IMG_TILE1_CHECK_ICON_LIGHT,IMG_TILE1_CHECK_ICON_DARK},
			{IMG_TILE3_CONTRAST_ICON_LIGHT,IMG_TILE3_CONTRAST_ICON_DARK},
			{IMG_TILE2_CUSTOMIZE_ICON_LIGHT,IMG_TILE1_CUSTOMIZE_ICON_DARK},
			{IMG_TILE3_SETUP_ICON_LIGHT,IMG_TILE2_SETUP_ICON_DARK},
			{IMG_TILE1_FULL_BRIGHTNESS_ICON_LIGHT,IMG_TILE1_FULL_BRIGHTNESS_ICON_DARK},
			{IMG_TILE2_HALF_BRIGHTNESS_ICON_LIGHT,IMG_TILE2_HALF_BRIGHTNESS_ICON_DARK},
			{IMG_TILE3_LOW_BRIGHTNESS_ICON_LIGHT,IMG_TILE3_LOW_BRIGHTNESS_ICON_DARK},
			{IMG_TILE1_BATTERY_ICON_LIGHT,IMG_TILE1_BATTERY_ICON_DARK},
			{IMG_TILE1_GRID_ICON_LIGHT,IMG_TILE1_GRID_ICON_DARK},
			{IMG_TILE4_BACK_ICON_LIGHT,IMG_TILE4_BACK_ICON_DARK},
			{0,0}
		};

		uint8_t img = image;
		
		uint8_t i=0;
		if (App_Darkmode){
			do
			{
				if (App_LightDarkMode_Images[i][0]==image){
					img = App_LightDarkMode_Images[i][1];
					break;
				}
				i++;
			} while (App_LightDarkMode_Images[i][0]!=0);
		}
		
		uint8_t buffer[2] = {0x80,img};
		DisplayCMD(buffer,2);
	}
}



unsigned short Rx_CRC_CCITT(const uint8_t * puchMsg, unsigned int usDataLen)
{
	uint8_t i = 0;
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;
	uint16_t wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for (i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
			wCRCin = (wCRCin << 1) ^ wCPoly;
			else
			wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

void displayCMD(const uint8_t* string, uint8_t length){
	uint8_t cdcBuffer[length+8];
	
	cdcBuffer[0]=0xaa;
	uint8_t i = 1;
	for (i;i<length+1;i++){
		cdcBuffer[i]=string[i-1];
	}
	
	cdcBuffer[i++]=(Rx_CRC_CCITT(string,length)>> 8 & 0xff);
	cdcBuffer[i++]=(Rx_CRC_CCITT(string,length) & 0xff);
	cdcBuffer[i++]=(0xe4);
	cdcBuffer[i++]=(0x1b);
	cdcBuffer[i++]=(0x11);
	cdcBuffer[i++]=(0xee);
	cdcBuffer[i]=(0x00);
	
	Display_send(cdcBuffer,i);
}

void displayDeactivateButton(uint8_t button){
	uint8_t buffer[2]={0xa3,button};
	DisplayCMD(buffer,2);
}

void displayActivateButton(uint8_t button){
	uint8_t buffer[2]={0xa2,button};
	DisplayCMD(buffer,2);
}

void displayRemoveImage(uint8_t image){
	uint8_t buffer[2] = {0x85,image};
	DisplayCMD(buffer,2);
}

void displayShowText(uint8_t field, const char* text, uint8_t length){
	uint8_t App_LightDarkMode_Texts [][2]= {
		{SF_VALUE0_LIGHT,SF_VALUE0_DARK},
		{SF_LABEL1_LIGHT, SF_LABEL1_DARK},
		{SF_LABEL2_LIGHT,SF_LABEL2_DARK},
		{SF_LABEL3_LIGHT,SF_LABEL3_DARK},
		{SF_LABEL4_LIGHT,SF_LABEL4_DARK},
		{SF_VALUE1_LIGHT,SF_VALUE1_DARK},
		{SF_VALUE2_LIGHT,SF_VALUE2_DARK},
		{SF_VALUE3_LIGHT,SF_VALUE3_DARK},
		{SF_VALUE4_LIGHT,SF_VALUE4_DARK},
		{SF_LABEL10_LIGHT,SF_LABEL10_DARK},
		{SF_LABEL31_LIGHT,SF_LABEL31_DARK},
		{SF_LABEL11_LIGHT,SF_LABEL11_DARK},
		{SF_LABEL30_LIGHT,SF_LABEL30_DARK},
		{SF_INFO_LINE0_LIGHT,SF_INFO_LINE0_DARK},
		{SF_INFO_LINE1_LIGHT,SF_INFO_LINE1_DARK},
		{SF_INFO_LINE2_LIGHT,SF_INFO_LINE2_DARK},
		{SF_INFO_LINE3_LIGHT,SF_INFO_LINE3_DARK},
		{SF_INFO_LINE4_LIGHT,SF_INFO_LINE4_DARK},
		{SF_HALFTILE41_LABEL_LIGHT,SF_HALFTILE41_LABEL_DARK},

		{0,0}
	};
	
	uint8_t buffer[length+2];;
	buffer[0] = 0x9f;
	uint8_t fld = field;
	
	uint8_t i=0;
	if (App_Darkmode){
		
		do
		{
			if (App_LightDarkMode_Texts[i][0]==field){
				fld = App_LightDarkMode_Texts[i][1];
				break;
			}
			i++;
		} while (i>0 && App_LightDarkMode_Texts[i][0]!=0);
	}

	buffer[1] = fld;
	for (uint8_t i = 2; i<length+2;i++){
		buffer[i]=text[i-2];
	}
	
	DisplayCMD(buffer,length+2);
}

void actOnDisplayResponse(void){
	
	uint8_t command_id = Display_Parser.current_message.payload[0];
	uint8_t target_id = Display_Parser.current_message.payload[1];
	uint8_t response_code = Display_Parser.current_message.payload[2];
	
	switch (command_id){
		case (CMD_BUTTON):
		switch (response_code){
			case (BTN_PRESS):
			User_State=USER_PRESS;
			app_response_id = target_id;
			break;
			case (BTN_RELEASE):
			User_State = USER_RELEASE;
			app_response_id = target_id;
			break;
			default:
			break;
		}
		break;
		case (CMD_SWIPE):
		switch (response_code){
			case (SWIPE_UP):
			User_State = USER_SWIPE_UP;
			break;
			case (SWIPE_DOWN):
			User_State = USER_SWIPE_DOWN;
			break;
			case (SWIPE_LEFT):
			User_State = USER_SWIPE_LEFT;
			break;
			case (SWIPE_RIGHT):
			User_State = USER_SWIPE_RIGHT;
			break;
			default:
			break;
		}
		break;
		case (0x80):
			if (target_id==0xc4){
				
			}
		break;
		
		default:
		
		break;
	}

	DisplayIdx=0;


}
