#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char time[6]={0};

void initTimer1()
{
	//configuring timer1 to compare mode
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);//using prescaler 64 so the clock = 15625
	TCNT1=0;
	OCR1A=15625;
	TIMSK|=(1<<OCIE1A);//enabling the timer interrupt
}

void init7SEG()
{
	DDRC|=0x0F;//configuring first 4 pins of port c as output for the 7SEG decoder
	PORTC&=0xF0;//initializing the output to 0
	DDRA|=0x3F;//configure the first 6 pins of port a as control for the 7 segments
	PORTA|=0x3F;
}

void initINT0()
{
	DDRD&=~(1<<2);//configure pin d2 as input
	PORTD|=(1<<2);//enable internal pull up resistance
	//configure interrupt 0 to falling edge
	MCUCR|=(1<<ISC01);
	MCUCR&=~(1<<ISC00);
	GICR|=(1<<INT0);//enable interrupt 0
}

void initINT1()
{
	DDRD&=~(1<<3);//configure pin d3 as input
	MCUCR|=(1<<ISC10)|(ISC11);//configure interrupt 1 to rising edge
	GICR|=(1<<INT1);//enable interrupt 1

}

void initINT2()
{
	DDRB&=~(1<<2);//configure pin b2 as input
	PORTB|=(1<<2);//enable internal pull up resistance
	MCUCSR&=~(1<<ISC2);//configure interrupt 2 to falling edge
	GICR|=(1<<INT2);//enable interrupt 2
}

int main()
{
	SREG|=(1<<7);//setting global interrupt bit
	initTimer1();
	init7SEG();
	initINT0();
	initINT1();
	initINT2();
	for(;;)
	{
		unsigned char i;
		for(i=0; i<6; i++)
		{
			PORTA=(PORTA&0xC0) | (1<<i);
			PORTC=(PORTC&0xF0) | time[i];
			_delay_ms(1);
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	//updating the clock every second
	time[0]++;
	if(time[0]==10)
	{
		time[0]=0;
		time[1]++;
		if(time[1]==6)
		{
			time[1]=0;
			time[2]++;
			if(time[2]==10)
			{
				time[2]=0;
				time[3]++;
				if(time[3]==6)
				{
					time[3]=0;
					time[4]++;
					if(time[4]==10)
					{
						time[4]=0;
						time[5]++;
						if(time[5]==10)
						{
							time[5]=0;
						}
					}
				}
			}
		}
	}
}

ISR(INT0_vect)
{
	unsigned char i;
	//reseting the time
	for(i=0; i<6; i++)
	{
		time[i]=0;
	}
}

ISR(INT1_vect)
{
	TCCR1B&=~(1<<CS10)&~(1<<CS11);//turning the timer off by reseting the clock
}

ISR(INT2_vect)
{
	TCCR1B|=(1<<CS10)|(1<<CS11);//turning the timer on by setting the clock
}
