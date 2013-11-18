#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1
	
#include "lcd/lcd.h"
#include "usart/usart.h"

volatile uint8_t 	charCount = 0;
volatile char 		String[255];

void main(){

	LCDBoot();
	USARTBoot(MYUBRR);

	LCDWriteString(0x00, 13, "PC Status 1.1");
	LCDWriteString(0x40, 2, "CP");
	LCDWriteString(0x48, 2, "ME");
	
	sei();

	while(1){
	}
}
ISR(USART_RXC_vect){

	cli();

	char byte = usart_getchar();
	
	uint8_t cpuUsage	= 0;
	uint8_t memUsageMSB = 0;
	uint8_t memUsageLSB = 0;
	uint16_t memUsage	= 0;
	
	if(byte == 0xfa){
		byte = 0x00;
		while(!(byte == 0x6c) || !(charCount >= 3)){
			byte = usart_getchar();
			switch(charCount){
				case 0:
					cpuUsage = byte;
				break;
				
				case 1:
					memUsageMSB = byte;
				break;
				
				case 2:
					memUsageLSB = byte;
				break;
				
			}
			charCount++;
		}
	}
	charCount = 0;
	memUsage = memUsageLSB | (memUsageMSB << 8);
	
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
		char bf[1];
		sprintf(bf, "%i", cpuUsage);
		LCDWriteStringBack(0x44, 3, bf);
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
		char bf2[1];
		sprintf(bf2, "%i", memUsage);
		LCDWriteStringBack(0x4B, 5, bf2);
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
	sei();
	
}
/*
ISR(USART_RXC_vect){
	char byte = usart_getchar();
	
	String[charCount] = byte;
	charCount++;
	if(byte == '\n' || byte == '\r'){
		String[charCount-1] = '\0';
		// HERE
			LCDWriteString(0x40, 40, String);	
		// END
		charCount = 0;
		memset(String, 0, sizeof(String));
	}
}
*/