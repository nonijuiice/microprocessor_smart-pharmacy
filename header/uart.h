#ifndef __UART_H__
#define __UART_H__
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_uart();
void htmp(); // high temperature => disply warning message
int txbuf_empty();
void send_msg(char buf[]);
void send_temp(int temp);

/*
ISR(USART0_RX_vect)
{
    cli();

    PORTB = 0xFF;   // LCD에 출력
    sei();
}
*/

void init_uart()
{
    cli();

    DDRE = (1 << PE1);
    
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // disale parity && 8bit
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);  // enable rx interrupt && enable receiver and transmitter
    // Baud rate
    UBRR0H = 0x00;
    UBRR0L = 0x67;
    
    // C: interrupt, parity bit, size of data(8 bit), rising/falling edge
    sei();
}

void htmp()
{
    unsigned char arr[50] = "Warning! High temperature!\n";
    int i = 0;

    while (arr[i] != '\0') {
        while ((UCSR0A & (1 << UDRE0)) == 0);
        UDR0 = arr[i++];
    }
}

int txbuf_empty()
{
    while ((UCSR0A & (1 << UDRE0)) == 0);
    return 1;
}

void send_msg(char buf[])
{
    int i = 0;

    while (buf[i] != '\0') {
        while ((UCSR0A & (1 << UDRE0)) == 0);
        UDR0 = buf[i++];
        // _delay_ms(100);
    }

    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = '\n';
    // _delay_ms(100);
}

void send_temp(int temp)
{
    int itg = temp / 10;
    int frac = temp % 10;
    char buf[20] = { 0, };

    int i = 0;
    const char msg[] = "Temperature:";

    sprintf(buf, "%s %d.%d\n", msg, itg, frac);
    while (buf[i] != '\0') {
        while ((UCSR0A & (1 << UDRE0)) == 0);
        UDR0 = buf[i++];
    }
    
}

void foo()
{
    ;
}
#endif
