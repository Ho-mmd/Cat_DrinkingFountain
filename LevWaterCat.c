#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define ENABLE PORTE |= 0x01;
#define DISABLE PORTE &= ~0x01;

#define RS_SET PORTE |= 0x04;
#define RS_CLI PORTE &= ~0x04;

#define RW_SET PORTE |= 0x02;
#define RW_CLI PORTE &= ~0x02;

volatile unsigned int measure, flag = 1, cnt = 0;
void LED_CLK();

void write_instruction(unsigned char data)
{
   RS_CLI
   RW_CLI
   ENABLE
   _delay_us(1);
   PORTB = data;
   _delay_us(1);
   DISABLE
}

void write_data(unsigned char data)
{
   RS_SET
   RW_CLI
   ENABLE
   PORTB = data;
   _delay_us(1);
   DISABLE
   _delay_us(1);
}

void init_LCD(void)
{
   _delay_ms(75);
   write_instruction(0x30);
   _delay_ms(25);
   write_instruction(0x30);
   _delay_ms(5);
   write_instruction(0x30);
   _delay_ms(5);
   write_instruction(0x38);
   _delay_ms(5);
   write_instruction(0x08);
   _delay_ms(5);
   write_instruction(0x01);
   _delay_ms(5);
   write_instruction(0x04);
   _delay_ms(5);
   write_instruction(0x0c);
   _delay_ms(5);
}

int main(void)
{
   cli();
   
   DDRD = 0x08; //PD3 출력
   DDRE = 0x07; //PE0,1,2 출력
   DDRB = 0xFF; //PB 전체 출력
   DDRF = 0xFF;

   UCSR1C = 0x07;
   UCSR1B = 0x98;

   UBRR1H = 0x00;
   UBRR1L = 0x2F;   

   _delay_ms(100);
   init_LCD();   
   
   write_instruction(0x8B); _delay_ms(10);
   write_data(':'); _delay_ms(100);
   write_data('L'); _delay_ms(100);
   write_data('E'); _delay_ms(100);
   write_data('V'); _delay_ms(100);
   write_data('E'); _delay_ms(100);
   write_data('L'); _delay_ms(100);
   write_data(' '); _delay_ms(100);
   write_data('R'); _delay_ms(100);
   write_data('E'); _delay_ms(100);
   write_data('T'); _delay_ms(100);
   write_data('A'); _delay_ms(100);
   write_data('W'); _delay_ms(100);

   	TIMSK = 0x04;
	TCCR1A = 0x00; 
	TCCR1B = 0x05; //clk select

	TCNT1H = 0xFF; //초기에 overflow 발생시기 당김
	TCNT1L = 0xC0;

   sei();

   do{

   }while(1);
}

ISR(USART1_RX_vect)
{
   cli();
   measure = UDR1;

   _delay_ms(100);
 
   if(measure == 0){
	   write_instruction(0xCF); _delay_ms(10);
	   write_data('%'); _delay_ms(100);
	   write_data('0'); _delay_ms(100);
	   write_data(' '); _delay_ms(100);
	   write_data(' '); _delay_ms(100);
	   /*PORTF = 0xFF;
	   _delay_ms(100);
	   PORTF = 0x00;
	   _delay_ms(100);
	   PORTF = 0xFF;
	   _delay_ms(100);
	   PORTF = 0x00;
	   _delay_ms(100);
	   PORTF = 0xFF;
	   _delay_ms(100);
	   PORTF = 0x00;
	   _delay_ms(100);*/
   }

   else if(measure <= 2){
	    write_instruction(0xCF); _delay_ms(10);
	    write_data('%'); _delay_ms(100);
	    write_data('5'); _delay_ms(100);
	    write_data('2'); _delay_ms(100);
	    write_data(' '); _delay_ms(100);
   }

   else if(measure <= 4){
	   write_instruction(0xCF); _delay_ms(10);
	   write_data('%'); _delay_ms(100);
	   write_data('0'); _delay_ms(100);
	   write_data('5'); _delay_ms(100);
	   write_data(' '); _delay_ms(100);
   }

   else if(measure <= 6){
	   write_instruction(0xCF); _delay_ms(10);
	   write_data('%'); _delay_ms(100);
	   write_data('5'); _delay_ms(100);
	   write_data('7'); _delay_ms(100);
	   write_data(' '); _delay_ms(100);
   }

   else if(measure > 6){
	   write_instruction(0xCF); _delay_ms(10);
	   write_data('%'); _delay_ms(100);
	   write_data('0'); _delay_ms(100);
	   write_data('0'); _delay_ms(100);
	   write_data('1'); _delay_ms(100);
   }

 	LED_WaterLev(measure);

   sei();
}


ISR(TIMER1_OVF_vect) { 
	cli();

	TCNT1H = 0xFF; //0.004초
	TCNT1L = 0x62;

	if(measure == 0) {
		if(flag) {
			PORTF = 0xFF;
			flag = 0;
		} else {		
			PORTF = 0x00;
			flag = 1;
		}
	}
	
		
	sei();

}

void LED_WaterLev(unsigned int a) {
	if(0 < a && a <= 2)
		PORTF = 0x01;
	else if(a <= 4)
		PORTF = 0x03;
	else if(a <= 6)
		PORTF = 0x07;
	else if(a > 6)
		PORTF = 0x0F;
}
