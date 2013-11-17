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

uint8_t 	charCount = 0;
char 		String[255];

void main(){

	LCDBoot();
	USARTBoot(MYUBRR);

	LCDWriteString(0x00, 13, "PC Status");
	sei();

	while(1){
	}
}
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