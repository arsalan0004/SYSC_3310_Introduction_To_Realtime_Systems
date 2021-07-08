/**
Lab 5 for SYSC 3310A: Intro to Real Time Systems (Summer 2021)

##Program Summery
This program toggles the states of two LEDs at different rates. The first LED is a red LED which can be turned on or off.
The program toggles it on/off every second. The second LED is a an RGB LED which has eight different states corresponding to 
'OFF' and seven 'ON' states where it produces different solid colors. The RGB LED's state is changed at time intervals following 
the this pattern: 0.1s...0.6s...1.1s..... where 's' is seconds. 

##Functionality
This program utilizes a 16-bit timer peripheral called TimerA0. This timer's input is a crystal oscillator (32.768 kHz) that allows it 
to count to 2 seconds. In this program, this timer is set to count to 100 milliseconds, afterwhich it will reset to 0 and count up again.
Once the counter reaches 100ms, it initiates an ISR which calculates if enough time has passed to toggle either of the LED states.

##Objective
To learn how to use timers generally, and to set up ISRs that rely on timers

author name: 		    Arsalan Syed
author student ID: 	101169528
date: 			        July 8th, 2021
*/

#include "msp.h"
#include <stdio.h>

#define RED_LED 0
#define RGB_LED 1
#define INPUT 	0
#define OUTPUT 	1

#define ONE_TENTH_SECOND 3277  //3276.8 rounded up 3277 // assuming a 32768 Hz clock, this is how many ticks are required to count 0.1 second (rounded up)

uint8_t LED_State = 0;

void configureTimer(){
	
	
	//timer configuration
	TA0CTL &= (uint16_t)(~(BIT5 | BIT5)); //stop the timer 
	
	TA0CTL &= (uint16_t)(BIT0); //reset timer 
	
	TA0CTL |= (uint16_t)(~BIT9); //select SMCLK as the clock source (32.768 khz)
	TA0CTL &= (uint16_t)(BIT8); //select SMCLK as the clock source (32.768 khz)
	
	TA0CTL &= (uint16_t)(~(BIT7 | BIT6));//set the input divider to the clock as 1 (ID register)
	TA0EX0 &= (uint16_t)(~(BIT0 | BIT1 | BIT2)); // set the input divider as 1 (input divider expansion)
	
	TA0CCR0 = (uint16_t)(ONE_TENTH_SECOND); //count up to 1 second 
	
	TA0CTL &= (uint16_t)(~BIT0); //clear interrupt flag
	
	TA0CTL |= (uint16_t)(BIT1); //enable interrupts
	
	TA0CTL |= (uint16_t)(BIT4); // upmode counter
	
	//enable TA interrupts in the NVIC
	NVIC_SetPriority(TA0_N_IRQn, 3);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
}


void configureIO(){
	
	//configure pins as GPIO
	P1->SEL0 &= ~BIT0;                //configuring pin 0 of port 1 as a GPIO pin (this pin is connected to a red LED)
	P1->SEL1 &= ~BIT0;  
	
	P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);//configuring pins 0,1, and 2 of port 2 as a GPIO pin (these pins are connected to cathodes of a RGB LED
	P2->SEL1 &= ~(BIT0 | BIT1 | BIT2); 

	//configuring the output pins
	P1->DIR |=  BIT0;                 // setting pin 0 (port 1) as output (powers red LED)
	P1->DS &= ~ BIT0;                 //set the drive strength of the output pins as 'regular strength' for port 1 pins
	P2->DIR |= (BIT0 | BIT1 | BIT2);  // setting pins 0, 1, 2 as outputs (powers RGB led)
	P2->DS &= ~(BIT0 | BIT1 | BIT2);  //set the drive strength of the output pins as 'regular strength' for port 2 pins

	P1->OUT &= ~ BIT0;                // sets pin 0 to 'Low'
	P2->OUT &= ~(BIT0 | BIT1 | BIT2); // sets pins 0, 1 and 2 as 'low'
}

void toggleRed(){
	 P1->OUT ^= BIT0;
}

void toggleRGB(){
	LED_State = (LED_State + 1 ) % 0x8; //toggle the state
	P2->OUT = (uint8_t)(LED_State);
}

void TA0_N_IRQHandler(void) {
	static uint16_t counter = 0; 	//this counter keeps track of how many times this ISR has been called (and the ISR is called every ONE_TENTH_SECOND (one thenth of a second))
	TA0CTL &= (uint16_t)(~BIT0); 	//clear the interrupt flag
	
	if(counter% 10 == 0){      		//if we've counted to 1 second, then toggle the red LED (toggle on/off)
			toggleRed();
	}
	else if (counter % 5 == 1){ 	//if we've counted to any time (in seconds) that follows the pattern...0.1...0.6...1.1....0.5(x) + 0.1, x = any positive integer
			toggleRGB();   						//change the color of the RGB LED
	}
	counter++;
	
}
int main(void){

	//disable watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	configureTimer();
	configureIO();
	
	//enable global interrupts
	__ASM("CPSIE I");



	//done with configuration
	while(1){
		__ASM("WFI"); //wait for interrupts
	}
	return 0;
}
