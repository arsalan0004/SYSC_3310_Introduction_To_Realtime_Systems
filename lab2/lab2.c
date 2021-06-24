/*
lab 2 for SYSC 3310A: Intro to Real Time Systems (Summer 2021)

This function uses two switches and two LEDs. One LED is a red LED and the other
is an RGB LED. When switch 'one' is clicked the selected LED changes state (on/off
in the case of the red LED). When switch 'two' is clicked we switch to another LED
to be 'selected'. This program's default is to have the red LED selected, and for 
the red LED to be in the off state. By clicking switch two, we can select the RGB
LED; following that, clicking switch one repeatedly will change the state of the RGB
LED such that it's color changes from one color to the next.

##Objective
To learn how to configure and control input/output pins  

##Limitations
The tasks carried out in this lab could be done more effectively by using interrupts 
and a timer (instead of using a while loop) for debouncing

author name: 				Arsalan Syed
author student ID: 	101169528
date: 							May 18th, 2021
*/

#include "msp.h"
#include <stdint.h>

#define DELAY_VALUE 100000
#define RED_LED 0
#define RGB_LED 1


void incrementLED_State(uint8_t LED_selection){
	if(LED_selection == RED_LED) {
				P1->OUT ^= 0x01; //toggle the red LED off/on
				return;
				}
	
	if(LED_selection == RGB_LED){
			static uint8_t RGB_LED_state = 0; 
			RGB_LED_state = (RGB_LED_state +1) % 8; //RGB LED has 8 states (0 - 7)
			P2->OUT = RGB_LED_state;
			return;
				}
	
}

int main(void){
	
	//pins to be used by this program
	uint8_t p0 = 0x1;
	uint8_t p1 = 1<<1;
	uint8_t p2 = 1<<2;
	uint8_t p4 = 1<<4;
	
	//disable the watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	//disable all interrupts
	__ASM("CPSID I");

	//configure pins as GPIO
	P1->SEL0 &= ~(p0|p1|p4);  //configuring pins 0,1 and 4 of port 1 as GPIO
	P1->SEL1 &= ~(p0|p1|p4);  
	
	P2->SEL0 &= ~(p0|p1|p2);  //configuring pins 0,1, and 2 of port 2 as GPIO
	P2->SEL1 &= ~(p0|p1|p2); 

	//configuring the input pins
	P1->DIR &= ~(p1|p4); //set the pins as input (port 1, pin 1 and 4)
	P1->REN |=  (p1|p4);  //set the resistor for the input pins
	P1->OUT |=  (p1|p4);  //set the resistors as pull up resistors


	//configuring the output pins
	P2->DIR |= (p0|p1|p2);  // setting pins 0, 1, 2 as outputs (powers RGB led)
	P1->DIR |=  p0;         // setting pin 0 (port 1) as output (powers red LED)
	P1->DS &= ~p0;          //set the drive strength of the output pins as 'regular strength' for port 1 pins
	P2->DS &= ~(p0|p1|p2);  //set the drive strength of the output pins as 'regular strength' for port 2 pins

	static uint8_t LED_selection = RED_LED;
	
	//done with configuration. Now the program waits for inputs
	while(1){
		//if pin 1 is driven low (button 1 is pressed)
		if(!(P1IN & p1)){
			
			//Debounce the button
			for(int i = 0; i < DELAY_VALUE; i++);

			if(!(P1IN & p1))
				//button click has passed debouncing.
				LED_selection ^= 0x1; //toggle which LED is selected
			
			
		}



		//if pin 4 is driven low (button 2 is pressed)
		if(!(P1IN & p4)){
		//Debounce the button
			for(int i = 0; i < DELAY_VALUE; i++);

			if(!(P1IN & p4))
				//button click has passed debouncing
				incrementLED_State(LED_selection);
			
		}
	}
}
