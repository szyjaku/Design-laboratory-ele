#define F_CPU 16000000UL
#include <avr/io.h>
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

void UART_Init( unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}
void UART_Transmit( unsigned char data )
{
	
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	UDR0 = data;
}
void Uart_Send_String(const char str[])
{
	uint8_t i = 0;
	while (str[i])
	{
		UART_Transmit(str[i]);
		i++;                   
	}
}

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

void turn_off(void)
{
	PORTB |= ((1<<PB1) | (1<<PB2) | (1<<PB3));
}

void segment(uint8_t bits)
{
	uint8_t numbers[10] = {0b111111,0b000110,0b011011,0b001111,0b100110,0b101101,0b111101,0b000111,0b111111,0b101111};
	uint8_t numbers2[10] = {0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b1 } ;

	PORTD = (PORTD & 0x03) | (~(numbers[bits] << 2) & 0xFC);
	
	if(numbers2[bits] == 1)
	{
		PORTB &= ~(1<<PB0);
	}
	else
	{
		PORTB |= (1<<PB0);
	}
} 

void display_numbers(uint8_t number)
{
	uint8_t hundreds = number/100;
	uint8_t tens = (number%100)/ 10;
	uint8_t units = number%10;
	
	for(uint16_t i=0; i<200; i++)
	{
		turn_off();
		segment(hundreds);
		PORTB &= ~(1<< PB1);
		delay(1);
		
		
		turn_off();
		segment(tens);
		PORTB &= ~(1<<PB2);
		delay(1);
		
		
		turn_off();
		segment(units);
		PORTB &= ~(1<<PB3);
		delay(1);
		
	}
}
void led(uint8_t diody)
{
	uint8_t leds[9]  = {0b000000, 0b000001, 0b000011, 0b000111, 0b001111, 0b011111, 0b111111, 0b111111, 0b111111};
	uint8_t leds2[9] = {0b00,     0b00,     0b00,     0b00,     0b00,     0b00,     0b00,     0b01 , 0b11};
		
	PORTC = leds[diody];
	PORTB &= (00<<PB4);
	PORTB |= (leds2[diody]<<PB4);
}
int main(void)
{
	UART_Init(MYUBRR);
	
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);  
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);
	
	uint8_t count=0;
	uint8_t up=1;
    
    while (1) 
    {
		led(count);
		Uart_Send_String("LED: ");
		UART_Transmit('0' + count); 
		Uart_Send_String("\r\n");
		
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

