#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

volatile unsigned int start_flag, T3_cnt, colorstate, color; //red=0 g=1 b=2
volatile unsigned int red_T, green_T, blue_T;
volatile unsigned int flag = 0;

void colorsensing(void) {

    start_flag = 1;
    TCNT1H = 0x00;
    TCNT1L = 0x00;
    PORTC = 0b00110010; //LED On, 2% Freq scale , Setting for dectecting Red
    T3_cnt = 0;
    _delay_ms(1000);
    if (red_T >= green_T) {
        if (red_T >= blue_T)
            PORTB = 0x01;
    }
    if (green_T > red_T) {
        if (green_T >= blue_T)
            PORTB = 0x02;
    }
    if (blue_T > red_T) {
        if (blue_T > green_T)
            PORTB = 0x04;
    }

}

int main(void) {
    cli();
    DDRC = 0xFF;
    DDRB = 0xFF;
    DDRD = 0x00;

    EIMSK = 0x01;
    EICRA = 0x02;

    TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);  // External Source Clk from T1
    TCCR3A = (1 << WGM11) | (1 << WGM10); 
    TCCR3B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Fast PWM Mode, Using OCRA1 as Top 
    ETIMSK = (1 << TOIE3);

    TCNT1H = 0x00;
    TCNT1L = 0x00;
    OCR3A = 624;
    PORTB = 0x00;
    sei();



    do
    {
        if (flag == 1) {
            colorsensing();
            flag = 0;
        }

    } while (1);


    return 0;
}

ISR(TIMER3_OVF_vect)
{
    cli();
    T3_cnt++;
    if (T3_cnt == 100)
    {
        T3_cnt = 0;

        if (start_flag == 1)
        {

            if (colorstate == 0) //red
            {
                red_T = TCNT1H * 256 + TCNT1L;

                colorstate = 1;
                TCNT1H = 0x00;
                TCNT1L = 0x00;
                PORTC = 0b00111110; //Setting for dectecting Green
            }
            else if (colorstate == 1) //green
            {
                green_T = TCNT1H * 256 + TCNT1L;

                colorstate = 2;
                TCNT1H = 0x00;
                TCNT1L = 0x00;
                PORTC = 0b00111010; //Setting for dectecting Blue
            }
            else if (colorstate == 2) //blue
            {
                blue_T = TCNT1H * 256 + TCNT1L;

                colorstate = 0;
                TCNT1H = 0x00;
                TCNT1L = 0x00;
                PORTC = 0b00000000; //LED off, Initialization
                start_flag = 0;

            }
        }
    }
    sei();
}

ISR(INT0_vect) {
    cli();
    flag = 1;
    sei();
}

