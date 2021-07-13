/*	Author: Alyssa Zepeda
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: https://youtu.be/8DHfh8jHNHM
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//0.954 Hz is lowest frequency possible with this function
//based on settings in PWM_on()
//Passing in 0 as the frequenct will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; //keeps track of curr freq
	//will only update when frequency changes, otherwise allows
	//music to play uninterrupted
	if(frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0x08;} //stop timer/counter
		else {TCCR3B |= 0x03;} //resumes timer/counter

		//prevents OCR3A from overflowing, uses prescaler 64
		//0.954 is smallest freq that will not result in overflow
		if(frequency < 0.954) { OCR3A = 0xFFFF;}

		//prevents OCR3A from underfowing 
		else if (frequency > 31250) {OCR3A = 0x0000;}

		//set OCR3A based on desired frequency
		else {OCR3A = (short) (8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; //reset counter
		current_frequency = frequency; //update frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	   //COM3A0: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	   //WGM32: when counter (TCNT3) matches OCR3A, reset counter
	   //CS31 & CS30: set prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x40; PORTB = 0x00; //set only PB6
    /* Insert your solution below */
   PWM_on(); 
   while (1) {
	if((~PINA & 0x07) == 0x01) {
		set_PWM(261.63);
	}
	else if((~PINA & 0x07) == 0x02) {
		set_PWM(293.66);
	}
	else if((~PINA & 0x07) == 0x04) {
		set_PWM(329.63);
	}
	else {
		set_PWM(0);
	}
    }
    PWM_off();
    return 1;
}
