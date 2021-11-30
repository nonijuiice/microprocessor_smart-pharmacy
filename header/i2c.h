#ifndef __I2C_DHT22_H__
#define __I2C_DHT22_H__

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#define ERR1 1
#define ERR2 2 
#define ERR3 3

// usage: PORT(A~F) 중 수정
#define DHT22_DDR DDRG
#define DHT22_PORT PORTG
#define DHT22_PIN PING
#define DHT22_INPUTPIN PG0

int state;

int getdata();    // temperature: num = 0, humidity: num = 1, int select;
void adc(int *data);
void init_adc();
int check_state(int val);
void print_led(int val);   // for debug

/* get data from dht22 */
int getdata() {
    char bits[5];
 
    memset(bits, 0, sizeof(bits));
 
    // reset port
    DHT22_DDR |= (1 << DHT22_INPUTPIN); //output
    DHT22_PORT |= (1 << DHT22_INPUTPIN); //high
    _delay_ms(100);
    
    // send request
    DHT22_PORT &= ~(1 << DHT22_INPUTPIN); //low
    _delay_ms(1);  // host the start signal down time
    
    // check pulse for 30us
    DHT22_PORT |= (1 << DHT22_INPUTPIN); //high
    _delay_us(1);   // host -> sensor
    DHT22_DDR &= ~(1 << DHT22_INPUTPIN); //input
    _delay_us(29);  // bus master has released time, 30us
    // while ((DHT22_PIN & (1 << DHT22_INPUTPIN)));
 
    // check start condition 1
    if((DHT22_PIN & (1 << DHT22_INPUTPIN))) {
        DHT22_DDR |= (1 << DHT22_INPUTPIN); //output
        DHT22_PORT |= (1 << DHT22_INPUTPIN); //high
        _delay_ms(100);
        return ERR1;
    }

    // _delay_us(80);
    while(!(DHT22_PIN & (1 << DHT22_INPUTPIN)));    // response to low time for 80us

    // check start condition 2
    if(!(DHT22_PIN & (1 << DHT22_INPUTPIN))) {
        DHT22_DDR |= (1 << DHT22_INPUTPIN); //output
        DHT22_PORT |= (1 << DHT22_INPUTPIN); //high
        _delay_ms(100);
        return ERR2;
    }

    // _delay_us(80);
    while (DHT22_PIN & (1 << DHT22_INPUTPIN));  // response to high time for 80us
 
    // read the data
    for (int i = 0; i < 5; i++) { // read 5 bytes
        char result = 0;
        for(int j = 0; j < 8; j++) {//read every bit
            while ( !(DHT22_PIN & (1 << DHT22_INPUTPIN)) ); //wait for an high input
            _delay_us(32);  // 어차피 '0'을 나타내는 high 구간
            if (DHT22_PIN & (1 << DHT22_INPUTPIN)) //if input is high after 32 us, get result <= '1'을 수신 중
                result |= (1 << (7-j));   // 해당 bit set
            while (DHT22_PIN & (1 << DHT22_INPUTPIN)); //wait until input get low
        }
        bits[i] = result;
    }

    // check checksum
    char cks = 0;
    for (int i = 0; i < 4; i++) cks += bits[i];
    if (cks != bits[4]) return ERR3;
 
    // reset port
    DHT22_DDR |= (1 << DHT22_INPUTPIN); // output
    DHT22_PORT |= (1 << DHT22_INPUTPIN); // low
    _delay_ms(100);

    return bits[2]*256 + bits[3];
}

void adc(int *data){
    int buf = getdata();
    if (buf == ERR1 || buf == ERR2 || buf == ERR3)
       PORTB = 0xFF;
    else
        *data = buf;
}

void init_adc()
{
    DDRF |= (1 << PF1);
	PORTF |= (1 << PF1);
	_delay_ms(100);
}

int check_state(int val)
{
    int ret = 0;
    if (val >= 400 && !state) {
		state = 1;
        DDRD |= (1 << PD2);
        PORTD |= (1 << PD2);
        _delay_ms(1);
        PORTD &= ~(1 << PD2);
        ret = 1;
    }

	if (val < 400 && state) {
        state = 0;
        DDRD |= (1 << PD2);
        PORTD |= (1 << PD2);
        _delay_ms(1);
        PORTD &= ~(1 << PD2);
        ret = 0;
	}

    return ret;
}

// for debug, if iPhone
void print_led(int val)
{
    DDRB |= 0x1F;   // pin4 ~ pin0
    if (100 <= val && val < 200) {
        PORTB &= ~0x1E;
        PORTB |= 0x01;
    }
    else if (200 <= val && val < 250) {
        PORTB &= ~0x1D;
        PORTB |= 0x02;
    }
    else if (250 <= val && val < 300) {
        PORTB &= ~0x1B;
        PORTB |= 0x04;
    }
    else if (300 <= val && val < 350) {
        PORTB &= ~0x17;
        PORTB |= 0x08;
    }
    else if (350 <= val && val < 750) {
        PORTB &= ~0x0F;
        PORTB |= 0x10;
    }
    else if (val == ERR1) {
        PORTB &= ~0x1C;
        PORTB |= 0x03;
    }
    else if (val == ERR2) {
        PORTB &= ~0x13;
        PORTB |= 0x0C;
    }
    else if (val == ERR3) {
        PORTB &= ~0x10;
        PORTB |= 0x0F;
    }
    else PORTB = 0x1F;
}
#endif
