#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

volatile unsigned int flag = 0, ADC_ReLED; //LED_Freq Variable
volatile unsigned char ADC_LLED, ADC_HLED;
volatile unsigned int ADC_ReWLev, Lev3; //Water_Level Variable
volatile unsigned char ADC_LWLev, ADC_HWLev; 
volatile unsigned int Tx; //Transmit Data

void ADC_LED(); //LED Control
void ADC_WaterLevel(); //Measure Water Level

int main(void) {
	
	cli();

	DDRF = 0x03; 
	DDRB = 0x10;
	DDRD = 0xF0;
	DDRE = 0x02;	
	
	//PWM, Inverting Mode
	//Set compare match: up_counting, Clear compare match: down_counting
	TCCR0 = (0<<FOC0) | (1<<WGM00) | (1<<COM01) | (1<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (1<<CS00 ); 

	TIMSK = 0x04; //Timer_counter1 interrupt enabled
	TCCR1A = 0x00; 
	TCCR1B = 0x05; //Prescale 1024

	TCNT1H = 0xFF; //Advance the Overflow Time
	TCNT1L = 0xC0;

	ADMUX = 0x00; // ADC init()
	ADCSRA = 0x83; // ADC Enable, Prescale 8

	UCSR0C = 0x07; //Stop bit: 1 bit, Data_Size: 8 bit, Transmit_data: Falling Edge, Received_data: Rising Edge
	UCSR0B = 0x98; //Rx complete interrupt enable, Receiver & Transimitter Enable

	UBRR0H = 0x00; //Baud Rate
	UBRR0L = 0x67;

	sei();
	
	do {
		
	} while (1);

	return 0;
}


ISR(TIMER1_OVF_vect) { 
	cli();

	TCNT1H = 0xFF; //0.01ÃÊ (10ms)
	TCNT1L = 0x62;

	ADC_LED(); //ADC_ to control LED Frequency
	ADC_WaterLevel(); //ADC_ to receive Water level
	
	OCR0 = 0x0F; //PWM Water Pump Control


	while((UCSR0A& 0x20)==0); //Ready to receive new data

	UDR0 = Tx; //Water Level Transmit

	sei();

}

void ADC_LED() {
	ADMUX = 0x00; //Use ADC Pin 0
	ADCSRA |= 0x40; //ADC start, (1<<ADSC)

	while(ADCSRA&0x40); //Wait until ADCSRA & 0x40 != 0, (1<<ADSC)

	ADC_LLED = ADCL;  //Low value First
	ADC_HLED = ADCH;
	ADC_ReLED = (ADC_LLED + ADC_HLED * 256) / 20;

	flag++;

	if(flag < ADC_ReLED) {
		PORTD = 0xF0;
	} else {
		PORTD = 0x00;
		flag = 0;
	}
}

void ADC_WaterLevel() {
	ADMUX = 0x01; //Use ADC Pin 1
	ADCSRA |= 0x40; //ADC start, (1<<ADSC)

	while ((ADCSRA & 0x10) == 0); //Wait until ADCSRA & 0x40 != 0, (1<<ADSC)

	ADC_LWLev = ADCL; //Low value First
	ADC_HWLev = ADCH;
	ADC_ReWLev = ADC_HWLev * 256 + ADC_LWLev;

	Lev3 = ADC_ReWLev;

	if ((Lev3) < 100) {
		Tx = 0;
	}
	else if ((Lev3) >= 100 && (Lev3) < 150) {
		Tx = 1;
	}
	else if ((Lev3) >= 150 && (Lev3) < 200) {
		Tx = 2;
	}
	else if ((Lev3) >= 200 && (Lev3) < 250) {
		Tx = 3;
	}
	else if ((Lev3) >= 250 && (Lev3) < 300) {
		Tx = 4;
	}
	else if ((Lev3) >= 300 && (Lev3) < 350) {
		Tx = 5;
	}
	else if ((Lev3) >= 350 && (Lev3) < 400) {
		Tx = 6;
	}
	else if((Lev3 >= 400)){
		Tx = 7;
	}
}
