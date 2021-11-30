#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/i2c.h>
#include <avr/uart.h>
#include <avr/servo2.h>
#include <avr/lcd.h>

volatile int val,sec=0,min=0,hour=0,halfsec=0,timerstart=0,mode=0;
extern int state;


int main(void){
    cli();
   DDRB=0xff;
   DDRD=0x00;
   DDRC=0xF8;  // pin3 for pulse shooting / pin 0,1,2 for input
   PortInit(); //LCD Port
	PORTC=0x07; // pin3 on Gnd / o.w scan source current 

   EIMSK=0b00000001; 
   EICRA=0b00000011;
   
   TCCR3A=0x00;
   TCCR3B=0x04;
   TCNT3H=0xff;
   TCNT3L=0xf0;
   
   ETIMSK=(1<<TOIE3);

   INIT_TIMER(); 
   init_adc();
   init_uart();
	LCD_Init();

   sei();

   
   do{
	      if(sec==1){			
		  
		   Move_servo(mode++);
		   if(mode==3) mode=0;
		}
	      else{
		  
	        val = getdata(); 
			print_led(val); 
			
			  
	         _delay_ms(1000);
		     if(state){                  //Display & Send Temperature Error msg;
				LCD_pos(0,0);
				LCD_STR("Temp Warning!      ");
				LCD_pos(1,0);
				LCD_STR("Check the bottle ");
				htmp();
				send_temp(val);
			 
			 }	
			 		 
			   if (check_state(val) == 1) {    
		         }			
	     }
    
   }while(1);
}

ISR(TIMER3_OVF_vect){
   cli();
   TCNT3H=0x85;
   TCNT3L=0xed; 
   if(timerstart==1){
      halfsec++;
      if(halfsec==2){
            sec++;
              halfsec=0;
            if(sec==60){
               sec=0;
               min++;    
               if(min==60){
                  min=0;
                  hour++;
               }
            }
         }
   }
   sei();
}

ISR(INT0_vect){
    timerstart=1;
}
