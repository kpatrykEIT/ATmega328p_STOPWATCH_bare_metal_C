
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "hd44780.h"
#include "defines.h"
#include "LCD.h"

#define LED PB0
#define BUTTON PB1

void TIMER1_INIT(void);
void stopwatch_menu(void);


uint8_t BUTTON_STATE			= 0; 

volatile uint8_t miliseconds	= 0;
volatile uint8_t seconds		= 0;
volatile uint8_t minutes		= 0;
volatile uint8_t hours			= 0;

char msBufor[10];
char secBufor[10];
char minBufor[10];
char hourBufor[10];

int main(void)
{
	
	lcd_init();
	DDRB |= (1 << LED);
	DDRB &= ~(1 << BUTTON);
	TIMER1_INIT();   
	sei(); 
	
    while (1) 
    {
		if( (PINB & _BV(BUTTON)) == 0)
		{
			BUTTON_STATE ++;
			_delay_ms(200);
		}
		switch(BUTTON_STATE)
		{
			
			case 0:			// RESET
			
			TCCR1B = 0;
			miliseconds = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
			stopwatch_menu();
			PORTB &= ~(1 << LED);
			
			break;
			
			case 1:			// START
			
			stopwatch_menu();
			
			// --- CTC MODE ENABLED, 1024 PRESCALER ---
			TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
			PORTB |= (1 << LED);
			// 
			
			break;
			
			case 2:			// STOP
			
			TCCR1B = 0;
			PORTB ^= (1 << LED);
			stopwatch_menu();
			
			break;
			
			default:
			
			BUTTON_STATE = 0;
			_delay_ms(30);
			break;
		}		
    }
}



ISR(TIMER1_COMPA_vect)
{
	miliseconds ++;
	
	if(miliseconds == 100)
	{
		seconds ++;
		miliseconds = 0;
	}
	if(seconds == 60)
	{
		minutes ++;
		seconds = 0;
	}
	if(minutes == 60)
	{
		hours ++;
		minutes = 0;
	}
	if(hours == 24)
	{
		miliseconds = 0;
		seconds = 0;
		minutes = 0;
		hours = 0;
		
	}
}

void stopwatch_menu()
{
	lcd_goto(0,0);
	lcd_puttext("Stopwatch");
	sprintf(msBufor, "%d", miliseconds);
	sprintf(secBufor, "%d", seconds);
	sprintf(minBufor, "%d", minutes);
	sprintf(hourBufor, "%d", hours);
	lcd_goto(8,1);
	lcd_puttext(":");
	lcd_goto(9,1);
	lcd_puttext(msBufor);
	lcd_goto(6,1); 
	lcd_puttext(secBufor);
	lcd_goto(5,1);
	lcd_puttext(":");
	lcd_goto(2,1);
	lcd_puttext(":");
	lcd_puttext(minBufor);
	lcd_goto(0,1);
	lcd_puttext(hourBufor);
	_delay_ms(100);
	lcd_cls();
	
}

void TIMER1_INIT()
{
	OCR1A = 156; // 1ms	
	TIMSK1 |= (1 << OCIE1A);
}

