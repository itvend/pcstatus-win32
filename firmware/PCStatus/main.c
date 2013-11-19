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

void main(){

	LCDBoot();
	USARTBoot(MYUBRR);

	//LCDWriteString(0x00, 13, "PC Status 1.1");
	LCDWriteString(0x00, 4, "CPU:");
	LCDWriteString(0x40, 4, "RAM:");
	
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
		sprintf(bf, "%i%%", cpuUsage);
		LCDWriteString(0x06, 6, bf);
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
		char bf2[1];
		sprintf(bf2, "%iMb", memUsage);
		LCDWriteString(0x46, 10, bf2);
	/* ++++++++++++++++++++++ ++++++++++++++++++++++ */
	
	sei();
	
}