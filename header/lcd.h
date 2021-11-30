#include<avr/io.h>
#include<util/delay.h>

#define LCD_WDATA PORTA
#define LCD_WINST PORTA

#define LCD_CTRL PORTF
#define LCD_EN 0
#define LCD_RW 1
#define LCD_RS 2

#define Byte unsigned char

#define On 1
#define Off 0

#define RIGHT 1
#define LEFT 0


void LCD_Data(Byte ch)
{
	LCD_CTRL |= (1<<LCD_RS);
	LCD_CTRL &= ~(1<<LCD_RW);
	LCD_CTRL |= (1<<LCD_EN);
	_delay_us(50);
	LCD_WDATA=ch;
	_delay_us(50);
	LCD_CTRL &= ~(1<<LCD_EN);	
	
}

void LCD_Comm(Byte ch)
{
	LCD_CTRL &= ~(1<<LCD_RS);
	LCD_CTRL &= ~(1<<LCD_RW);
	LCD_CTRL |= (1<<LCD_EN);
	_delay_us(50);
	LCD_WINST = ch;
	_delay_us(50);
	LCD_CTRL &= ~(1<<LCD_EN);
}


void LCD_CHAR(char c)
{	
	LCD_Data(c);
	_delay_ms(1);
	
}


void LCD_STR(Byte *str)
{
	while(*str!=0)
	{
		LCD_CHAR(*str);
		str++;
	}
}


void LCD_pos(unsigned char col,unsigned char row)
{
	LCD_Comm(0x80|(row+col*0x40));

}



void LCD_Clear(void)
{
	LCD_Comm(0x01);
	_delay_ms(2);
}
void PortInit(void)
{
	DDRA=0xff;
	DDRF=0x0f;

}
void LCD_Init(void)
{
	_delay_ms(75);
	LCD_Comm(0x38);     //DDRAM, 데이터 8비트 사용, LCD 2열 사용(6)
    _delay_ms(30);       //2ms 지연
    LCD_Comm(0x38);     //DDRAM, 데이터 8비트 사용, LCD 2열 사용(6)
    _delay_ms(5);       //2ms 지연
    LCD_Comm(0x38);     //DDRAM, 데이터 8비트 사용, LCD 2열 사용(6)
    _delay_ms(5);       //2ms 지연
    LCD_Comm(0x0e);     //Display ON/OFF
    _delay_ms(5);       //2ms 지연
    LCD_Comm(0x06);     //주소+1, 커서를 우측 이동(3)
    _delay_ms(2);       //2ms 지연
    LCD_Clear();        //LCD 화면 클리어

}
void LCD_Shift(char p)
{
	if(p==RIGHT){
		LCD_Comm(0x1c);
		_delay_ms(1);
		}
	else if(p==LEFT)
	{
		LCD_Comm(0x10);
		_delay_ms(1);
	}
}
void Cursor_Home(void)
{
	LCD_Comm(0x02);
	_delay_ms(2);
}










 
