#include <avr/io.h>

void USARTBoot(uint16_t ubrr);
char usart_getchar( void ); 
void usart_putchar( char data ); 
void usart_putint( uint8_t data ); 
void usart_pstr (char *s);