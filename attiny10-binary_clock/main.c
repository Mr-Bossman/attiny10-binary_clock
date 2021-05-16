#include <util/delay_basic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
volatile uint8_t s,m,h,display;
volatile uint16_t t = 0;
struct the {
	unsigned ddrx : 3;
	unsigned portx : 3;
};
struct the a[6] = {{0b001,0b100 },{0b001,0b010 },{0b010,0b100 },{0b010,0b001 },{0b100,0b010 },{0b100,0b001 }};
void setArray(uint8_t in){
	DDRB = 0;
	for(uint8_t b = 0;in;b++,in>>=1){
		if(in&1){
			DDRB = 0;
			PORTB = (1<<3);
			DDRB = (~a[b].ddrx);
			PORTB = a[b].portx | (1<<3);
		}
	}
}
int main(void)
{
	CCP= 0xD8;
	CLKPSR = 1;
	s=m=5;
	h=17;
	display=2;
	DDRB = 0;
	PORTB = (1<<3);// no  pullup on this pin :(
	PCICR |= (1 << PCIE0);
	PCMSK |= (1 << PCINT3);
	TCCR0B |= 12;
	OCR0A = 15625;
	TIMSK0 = 2;
	sei();
	while (1)
	{
		if(s==60)s=0,m++;
		if(m==60)m=0,h++;
		if((h&0b1111)==13)h = ((h&48)^48)+1;
		if ((display&3) == 0)setArray(h);
		else if ((display&3) ==1)setArray(m);
		else if ((display&3) ==2)setArray(s);
		else display = 0;


	}
}
ISR(TIM0_COMPA_vect){
	s++;
}
ISR (PCINT0_vect)
{
	if(PINB&8){
		if(t>10&&t<5000){
			if(display&128){
				if ((display&3) == 0)h++;
				else if ((display&3) ==1)m++;
				else if ((display&3) ==2)s++;
				}else{
				display++;
			}
		}
		if(t>5000)display = (display&128)?display+1:128;
		t=0;
		}else{
		while(!(PINB&8)){
			t++;
			_delay_loop_1(0);
		}
	}
}
