/*	Author: Alyssa Zepeda
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: https://youtu.be/d25cc9qGGDU
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
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
/////////////////////////////////////////////////////////////////////////

enum states{start, init, on, release} state;
//mary had a little lamb
double melody[25] = {329.63, 0, 293.66, 0, 261.63, 0, 293.66, 0, 329.63, 0, 329.63, 0, 329.63, 0, 329.63, 0, 293.66, 0, 293.66, 0, 329.63, 0, 293.66, 0, 261.63};
unsigned short timeHeld[25] = {24, 1, 12, 1, 12, 1, 12, 1, 12, 1, 12, 1, 24, 1, 12, 1, 12, 1, 12, 1, 12, 1, 12, 1, 24};

unsigned char j;
unsigned char k;

void Tick() {
   switch(state) {
	case start: state = init; break;
	case init: 
		if((~PINA&0x01) == 0x01) {state = on;}
		else {state = init;}
		break;
	case on:
		if(k < 25) {state = on;}
		else {state = release;}
		break;
	case release:
		state = ((~PINA&0x01) == 0x00) ? init : release;
		break;
	default: break;
   }
   switch(state) {
	case start: break;
	case init: j= 0; k = 0; set_PWM(0); break;
	case on: 
		for(int i = 0; i < timeHeld[j]; i++) {
			set_PWM(melody[k]);
		}
		j++;
		k++;
		break;
	case release: set_PWM(0); break;
	default: break;
   }
}   
	
//////////////////////////////////////////////////////////////

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x40; PORTB = 0x00; //set only PB6
    /* Insert your solution below */
  	TimerSet(250);
	TimerOn();
	PWM_on();
 	state = start; 
   while (1) {
	Tick();
	while(!TimerFlag) {};
	TimerFlag = 0;
    }
    PWM_off();
    return 1;
}
