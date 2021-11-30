#include <avr/io.h>
#include <util/delay.h>

#define PULSE_MINUS1 1300 //-90 for servo 1 angle
#define PULSE_MID1 3500 // 0
#define PULSE_PLUS1 5200 //90

#define PULSE_MINUS2 1200 //-90  for servo 2 angle
#define PULSE_MID2 3400 // 0
#define PULSE_PLUS2 5200 //90

#define PULSE_MINUS3 1100 //-90 for servo 3 angle
#define PULSE_MID3 3200 // 0
#define PULSE_PLUS3 5200 //90


volatile int R_life[3]={2,1,3}; //the number of tablets for each mode (Morning,lunch,din) 
volatile int G_life[3]={2,3,1};
volatile int B_life[3]={2,2,2};
int CCk_flag=0; // Error Checker

void INIT_TIMER(void)

{

  TCCR1A |= (1<<WGM11);
  TCCR1B |= (1<<WGM12) | (1<<WGM13); // Fast PWM mode, using ICR1 as Top
  TCCR1B |= (1<<CS11); // Prescale 8, 2MHz
  ICR1 = 40000; // Make the period 20ms 
  TCCR1A |= (1<<COM1A1) | (1<<COM1B1)| (1<<COM1C1); // Non inverting
  

}

void Give_Pulse(void){ // Give External Interrupt Pulse to Sub-Atmega
	PORTC|=0x08;
	_delay_ms(500);
	PORTC&=0xF7;

	_delay_ms(300);

	PORTC|=0x08;
	_delay_ms(500);
	PORTC&=0xF7;
	
}

int Color_Error_Detect(int color){ // R:1 G:2 B:3
	
	if(color==1)
	{
		if((PINC&0x07)==0b00000001)
		{
		}
		else
		{
		
			return 1;
		}
		
	}
	else if(color==2){
		if((PINC&0x07)==0b00000010)
		{
		}
		else
		{
			return 1;
		}

	}

	else if(color==3){
		if((PINC&0x07)==0b00000100)
		{
		}
		else
		{
			return 1;
		}
		
	}
		 
	return 0;


}
void Move_servo(int mode){

	int i=0;
	
	LCD_Clear();
	LCD_pos(0,0);
	LCD_STR("     Start!      ");
	

///////////////////////////////////////////////////////RED
	for(i=0; i<R_life[mode]; i++)  
	{
	OCR1A=PULSE_MINUS1;
	_delay_ms(1000);
	
	OCR1A=PULSE_MID1;

	_delay_ms(500);

	Give_Pulse();
	_delay_ms(500);
	
	
	
	CCk_flag=Color_Error_Detect(1); // Red detecting
	_delay_ms(100);

	if(CCk_flag==0)
		{	
			_delay_ms(500);
			LCD_pos(0,0);
			LCD_STR("    RED       ");	
			LCD_pos(1,0);
			LCD_STR("The Right color   ");		

		}
	else{
			_delay_ms(500);
			LCD_pos(0,0);						//LCD Msg output
			LCD_STR(" Bottle 1     ");
			LCD_pos(1,0);
			LCD_STR(" Color Error    ");		
			send_msg("Bottle 1 Color Error"); //// Uart Msg output

		}
	
	_delay_ms(500);



	OCR1A=PULSE_PLUS1;
	_delay_ms(1000);

	OCR1A=PULSE_MINUS1;
	


	}

//////////////////////////////////////////////////////////////////////////// Green
	_delay_ms(1000);

	for(i=0; i<G_life[mode]; i++) 
	{
	OCR1B=PULSE_MINUS2;
	_delay_ms(1000);
	
	OCR1B=PULSE_MID2;

	_delay_ms(500);

	Give_Pulse();
	_delay_ms(500);
	
	
	CCk_flag=Color_Error_Detect(2);   // Green detecting
	_delay_ms(100);
	
	
	if(CCk_flag==0)
		{	
			PORTC|= 0x10;	
			_delay_ms(500);
			LCD_pos(0,0);
			LCD_STR("     Green       ");	
			LCD_pos(1,0);
			LCD_STR("The Right Color   ");		

		}
	else{
			PORTC|= 0x20;
			_delay_ms(500);
			LCD_pos(0,0);						//LCD Msg output
			LCD_STR(" Bottle 2       ");
			LCD_pos(1,0);
			LCD_STR(" Color Error     ");		
			send_msg("Bottle2  Color Error"); //// Uart Msg output
		}
	
	_delay_ms(500);



	OCR1B=PULSE_PLUS2;
	_delay_ms(1000);

	OCR1B=PULSE_MINUS2;
	


	}



////////////////////////////////////////////////////Blue
	_delay_ms(1000);

	for(i=0; i<B_life[mode]; i++)  
	{
	OCR1C=PULSE_MINUS3;
	_delay_ms(1000);
	
	OCR1C=PULSE_MID3;

	_delay_ms(500);

	Give_Pulse();
	_delay_ms(500);
	
	
	
	CCk_flag=Color_Error_Detect(3); // Blue detecting
	_delay_ms(100);

	if(CCk_flag==0)
		{	
			_delay_ms(500);
			LCD_pos(0,0);
			LCD_STR("    BLUE       ");	
			LCD_pos(1,0);
			LCD_STR("The Right color  ");		

		}
	else{
			_delay_ms(500);
			LCD_pos(0,0);						//LCD Msg output
			LCD_STR(" Bottle 3       ");
			LCD_pos(1,0);
			LCD_STR(" Color Error    ");		
			send_msg("Bottle 3 Color Error"); //// Uart Msg output

		}
	
	_delay_ms(500);



	OCR1C=PULSE_PLUS3;
	_delay_ms(1000);

	OCR1C=PULSE_MINUS3;
	


	}
	_delay_ms(200);
	
	LCD_Clear();
	LCD_pos(0,0);
	LCD_STR("    Finish!      ");
	 
}


