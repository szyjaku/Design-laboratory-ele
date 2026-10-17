#define F_CPU 16000000UL
#include <avr/io.h>

void delay(uint16_t ms)
{
	while(ms!=0)
	{
		for(uint16_t volatile i=0;i<=1600;i++)
		{
			
		}
		ms--;
	}
}

uint8_t numbers[10]={0b0111111,0b0000110,0b1011011,0b1001111,0b1100110,0b1101101,0b1111101,0b0000111,0b1111111,0b1101111};

void turn_off(void)
{
	PORTB |= ((1<<PB0) | (1<<PB1) | (1<<PB2));
} 

void display_numbers(uint8_t number)
{
	uint8_t hundreds = number/100;
	uint8_t tens = (number%100)/ 10;
	uint8_t units = number%10;
	
	for(uint16_t i=0; i<190; i++)
	{
		turn_off();
		PORTD = ~numbers[hundreds];
		PORTB &= ~(1<< PB0);
		delay(1);
		
		
		turn_off();
		PORTD = ~numbers[tens];
		PORTB &= ~(1<<PB1);
		delay(1);
		
		
		turn_off();
		PORTD = ~numbers[units];
		PORTB &= ~(1<<PB2);
		delay(1);
		
	}
}
void led(uint8_t diody)
{
	uint8_t leds[9]  = {0b000000, 0b000001, 0b000011, 0b000111, 0b001111, 0b011111, 0b111111, 0b111111, 0b111111};
	uint8_t leds2[9] = {0b00,     0b00,     0b00,     0b00,     0b00,     0b00,     0b00,     0b01 , 0b11};
		
	PORTC = leds[diody];
	PORTB &= (00<<PB3);
	PORTB |= (leds2[diody]<<PB3);
}
int main(void)
{
	DDRD = 0xFF;
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);
	
	uint8_t count=0;
	uint8_t up=1;
    
    while (1) 
    {
		led(count);
		for(uint8_t i=1; i<=100; i++)
		{
			display_numbers(i);
			
		}
		if(up){count++;};
		if(!up){count--;};
		if(count==8){up=0;};
		if(count==0){up=1;};
    }
}

