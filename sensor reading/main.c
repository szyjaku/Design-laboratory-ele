#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "sdcard.h"
#include "sdprint.h"
#include "uart.h"
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1
#define TIMEOUT 200


enum
{
	DHT11_OK,
	DHT11_TIMEOUT,
	DHT11_CHECKSUM_ERROR
};
typedef struct
{
	uint8_t humidity;
	uint8_t temp_whole;
	uint8_t temp_frac;
}
DHT_TEMP_HUMIDITY;
static DHT_TEMP_HUMIDITY temp_humidity;
static uint16_t count_at_level(uint8_t level);

static uint8_t read_sensor()
{
	uint16_t count[40];
	uint8_t data[5];
	DDRB &= ~(1 << DDB1);
	PORTB |= (1 << PB1);
	_delay_ms(20);
	
	DDRB |= (1 << DDB1);
	PORTB &= ~(1 << PB1);
	_delay_ms(20);
	
	DDRB &= ~(1 << DDB1);
	PORTB |= (1 << PB1);
	_delay_us(30);
	
	if (count_at_level(0) == TIMEOUT)
	return DHT11_TIMEOUT;
	
	if (count_at_level(1) == TIMEOUT)
	return DHT11_TIMEOUT;
	
	for (int i = 0; i < 40; ++i)
	{
		if (count_at_level(0) == TIMEOUT)
		break;
		count[i] = count_at_level(1);
	}
	
	for (int i = 0; i < 40;  ++i)
	{
		data[i / 8] <<= 1;
		data[i / 8] |= count[i] > 40;
	}
	
	if (data[0] + data[1] + data[2] + data[3] != data[4])
	return DHT11_CHECKSUM_ERROR;
	
	temp_humidity.humidity = data[0];
	temp_humidity.temp_whole = data[2];
	temp_humidity.temp_frac = data[3];
	
	return DHT11_OK;
}
static uint16_t count_at_level(uint8_t level)
{
	uint16_t count = 0;
	uint8_t while_state = level ? (1 << PB1) : 0;
	while((PINB & (1 << PB1)) == while_state)
	{
		if (++count >= TIMEOUT)
		break;
		_delay_us(0.10);
	}
	return count;
}
void S1_Init()
{
	DDRE &= ~(1 << PE0);
	PORTE |= (1 << PE0);
}
uint8_t get_S1()
{
	if ((PINE & (1 << PE0)) == 0)
	{
		_delay_ms(20);
		if ((PINE & (1 << PE0)) == 0)
		{
			while ((PINE & (1 << PE0)) == 0);
			return 1;
		}
	}
	return 0;
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
	PORTE |= ((1<<PE1) | (1<<PE2) | (1<<PE3));
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
	
	for(uint16_t i=0; i<10; i++)
	{
		turn_off();
		segment(hundreds);
		PORTE &= ~(1<< PE1);
		delay(1);
		
		
		turn_off();
		segment(tens);
		PORTE &= ~(1<<PE2);
		delay(1);
		
		
		turn_off();
		segment(units);
		PORTE &= ~(1<<PE3);
		delay(1);
		
	}
}
void led(uint8_t diody)
{
	uint8_t leds[7]  = {0b000000, 0b000001, 0b000011, 0b000111, 0b001111, 0b011111, 0b111111 };
	
	PORTC = leds[diody];
}
int main(void)
{
	UART_init(MYUBRR);
	S1_Init();
	SPI_init(SPI_MASTER | SPI_FOSC_128 | SPI_MODE_0);
	
	
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	DDRB |= (1 << DDB0);
	DDRE |= (1 << DDE1) | (1 << DDE2) | (1 << DDE3);
	
	uint8_t count=0;
	uint8_t up=1;
	uint8_t res[5], buf[512], token;
	uint32_t addr = 0x00000000;
	
	_delay_ms(5000);
	if(SD_init() != SD_SUCCESS)
	{
		UART_puts("Error initializaing SD CARD\r\n");
	}
	else
	{
		UART_puts("SD Card initialized\r\n");
	}
	while (1)
	{
		led(count);
		
		uint8_t status = read_sensor();
		if (status == DHT11_OK)
		{
			UART_puts("Wilgotnosc: ");
			UART_Print_Number(temp_humidity.humidity);
			UART_puts("%  Temperatura: ");
			UART_Print_Number(temp_humidity.temp_whole);
			UART_putc('.');
			UART_Print_Number(temp_humidity.temp_frac);
			UART_puts(" LED: ");
			UART_putc('0' + count);
			UART_puts("\r\n");
		}
		else if (status == DHT11_TIMEOUT)
		{
			UART_puts("Timeout\r\n");
		}
		else
		{
			UART_puts("Error\r\n");
		}
				   
		   
		
		for(uint8_t i=1; i<=100; i++)
		{
			display_numbers(i);
			
			if (get_S1())
			{
				
				UART_pputs("\r\nReading sector: 0x");
				UART_puthex8((uint8_t)(addr >> 24));
				UART_puthex8((uint8_t)(addr >> 16));
				UART_puthex8((uint8_t)(addr >> 8));
				UART_puthex8((uint8_t)addr);
				res[0] = SD_readSingleBlock(addr, buf, &token);
				UART_pputs("\r\nResponse:\r\n");
				SD_printR1(res[0]);

				if((res[0] == 0x00) && (token == SD_START_TOKEN))
				{
					UART_puts("\r\n");
					UART_puts("Odczytane dane: ");
					UART_puts("Wilgotnosc: ");
					UART_Print_Number(buf[0]);
					UART_puts("%  Temperatura: ");
					UART_Print_Number(buf[1]);
					UART_putc('.');
					UART_Print_Number(buf[2]);
					UART_puts("\r\n");
				}
				
				
				else if(!(token & 0xF0))
				{
					UART_pputs("Error token:\r\n");
					SD_printDataErrToken(token);
				}
				
				
				for(uint16_t j = 0; j < 512; j++)
				{
					buf[j] = 0;
				}
				 buf[0] = temp_humidity.humidity;
				 buf[1] = temp_humidity.temp_whole;
				 buf[2] = temp_humidity.temp_frac;

				 UART_pputs("Writing values to sector: 0x");
				 UART_puthex8((uint8_t)(addr >> 24));
				 UART_puthex8((uint8_t)(addr >> 16));
				 UART_puthex8((uint8_t)(addr >> 8));
				 UART_puthex8((uint8_t)addr);

				 // write data to sector
				 res[0] = SD_writeSingleBlock(addr, buf, &token);

				 UART_pputs("\r\nResponse:\r\n");
				 SD_printR1(res[0]);

				 // if no errors writing
				 if(res[0] == 0x00)
				 {
					 if(token == SD_DATA_ACCEPTED)
					 UART_pputs("Write successful\r\n");
				 }
				 
				
			}
			
		}
		if(up){count++;};
		if(!up){count--;};
		if(count==6){up=0;};
		if(count==0){up=1;};
	}
}



