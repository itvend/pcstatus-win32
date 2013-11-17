#include "lcd.h"

void LCDBoot(void){

  DD_LCD_DATA 	= 0b11111111; // Output
  P_LCD_DATA 	= 0b00000000; // Pins low

  DD_LCD_COMMAND 	|= 1 << PIN_LCD_COMMAND_RS | 1 << PIN_LCD_COMMAND_RW | 1 << PIN_LCD_COMMAND_E;
  P_LCD_COMMAND 	&= ~(1 << PIN_LCD_COMMAND_RS) | ~(1 << PIN_LCD_COMMAND_RW) | ~(1 << PIN_LCD_COMMAND_E);
  
  LCDWriteCommand(0b00111000); // Function set
  LCDWriteCommand(0b00001100); // Display ON/OFF controll
  LCDWriteCommand(0b00000001); // Clear Display
  _delay_ms(1.6);
  LCDWriteCommand(0b00000110); // Entry Mode Set

}
void LCDWriteCommand(uint8_t command){
	LCDBusy();
	
	DD_LCD_DATA = 0b11111111; // Output
	P_LCD_DATA 	= 0b00000000; // Pins low
	
	P_LCD_DATA = command;
	/* Set RS & RW bits to 0 */
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RW); 
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RS);
	/* Enable pin to 1*/
	P_LCD_COMMAND |= 1 << PIN_LCD_COMMAND_E;
	
	_delay_us(50);
	
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_E);
	P_LCD_DATA = 0;
	
}
uint8_t LCDReadCommand(void){
	LCDBusy();

	uint8_t temp;
	
	DD_LCD_DATA = 0b00000000; // Data direction input

	P_LCD_COMMAND |= 1 << PIN_LCD_COMMAND_RS;
	P_LCD_COMMAND |= 1 << PIN_LCD_COMMAND_RW;
	
	P_LCD_COMMAND |= 1 << PIN_LCD_COMMAND_E;
	
	_delay_us(50);
	
	temp = P_LCD_DATA;
	
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_E); 
	
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RS);
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RW);
	
	return temp;
}
void LCDWriteData(uint8_t data){
	LCDBusy();
	
	P_LCD_DATA = data;

	P_LCD_COMMAND |= (1 << PIN_LCD_COMMAND_E);
	P_LCD_COMMAND |= (1 << PIN_LCD_COMMAND_RS);
	
	_delay_us(50);
	
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_E); 
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RS);
	
	P_LCD_DATA = 0;
	
	_delay_ms(3);
}
void LCDWriteString(uint8_t startpos, uint8_t with, char data[]){
	if(!(with < strlen(data))){
		LCDSetCursor(startpos);
		uint8_t to = with - strlen(data);
		while(*data)
		{
			LCDWriteData(*data);
			data++;
		}
		
		for(int i=0; i < to; i++)
		{
			LCDWriteData(0b00100000);
		}
	}else{
		LCDWriteData(0x32);
	}
}
void LCDWriteStringBack(uint8_t startpos, uint8_t with, char data[]){
	
	//LCDWriteCommand(0b00000100);
	
	uint8_t datalen = strlen(data);
	uint8_t to = with - datalen;
	
	LCDSetCursor(startpos+to);
	
	while(*data)
	{
		LCDWriteData(*data);
		data++;
	}
	LCDSetCursor(startpos);
	for(int i=0; i < to; i++)
	{
		LCDWriteData(0b00110000);
	}
	
	//LCDWriteCommand(0b00000110);
}
void LCDSetCursor(uint8_t pos){
	uint8_t mask = 0b10000000;
	uint8_t ddram = mask | (pos-1);
	LCDWriteCommand(ddram);
}
void LCDClear(void){
	LCDWriteCommand(0b00000001);
	_delay_ms(3);
}
void LCDBusy(void){
	DD_LCD_DATA = 0b00000000;
	
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_E); 
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RS);
	
	P_LCD_COMMAND |= 1 << PIN_LCD_COMMAND_RW;
		while(1 & (P_LCD_DATA >> 7)){ }
	DD_LCD_DATA = 0b11111111;
	P_LCD_COMMAND &= ~(1 << PIN_LCD_COMMAND_RW);
}