#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#ifndef DD_LCD_DATA
	#define DD_LCD_DATA 		DDRA
#endif

#ifndef P_LCD_DATA
	#define P_LCD_DATA 			PORTA
#endif

#ifndef DD_LCD_COMMAND
	#define DD_LCD_COMMAND 		DDRB
#endif

#ifndef P_LCD_COMMAND
	#define P_LCD_COMMAND		PORTB
#endif

#ifndef PIN_LCD_COMMAND_RS
	#define PIN_LCD_COMMAND_RS	0
#endif

#ifndef PIN_LCD_COMMAND_RW
	#define PIN_LCD_COMMAND_RW	1
#endif

#ifndef PIN_LCD_COMMAND_E
	#define PIN_LCD_COMMAND_E	2
#endif

void LCDBoot(void);

void LCDWriteCommand(uint8_t command);
uint8_t LCDReadCommand(void);
void LCDWriteString(uint8_t startpos, uint8_t with, char data[]);
void LCDWriteStringBack(uint8_t startpos, uint8_t with, char data[]);
void LCDWriteData(uint8_t data);
void LCDSetCursor(uint8_t pos);
void LCDClear(void);
void LCDBusy(void);