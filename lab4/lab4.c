/**
Lab 4 for SYSC 3310A: Intro to Real Time Systems (Summer 2021)

This application uses two switches, a built-in 16 bit timer called TimerA and two LEDs (a red LED and an RGB LED).
Only one LED can be controlled at any given time, and that LED that is being controlled is referred to as the 'selected' LED
switch 1 switches the selection between the RGB and the red LED.
When the timer counts to 1 second, it sends an interrupt which ultimatly cuases the selected LED to change state.
The red LED has two states (on/off) while the RGB LED has 8 states (corresponding to 8 different colors it can produce).
After counting to 1 second, the timer resets to 0, and starts counting up again.
By clicking switch 2, the user can pause timer.

##Objective
To learn how to use timers generally, and to set up ISRs that rely on timers

author name: 		    Arsalan Syed
author student ID: 	101169528
date: 			        July 1st, 2021
*/

#include "msp.h"
#include <stdio.h>

#define RED_LED 0
#define RGB_LED 1
#define INPUT 0
#define OUTPUT 1

#define ONE_SECOND 32767//32768 - 1 // assuming a 32768 Hz clock, this is how many ticks are required to count 1 second

static uint8_t LED_selection = RGB_LED; //specifies which LED is currently selected
static uint8_t RGB_LED_state = 0;
static uint8_t red_LED_state = 0;

/*
based on LED_selection, this turns on one LED and turns off the other. For example
when LED_selection = RED_LED, then the red LED is turned on, and the RGB LED is turned on
*/
void updateLED(){
	if(LED_selection == RED_LED){
		//set the red LED's state
		
		/*
		TO DO:
			for some reason, the program crashes when p1 is written to directly, so we have 
		  to use this less elegent method for toggling the bits. Should probably figure out 
		  why this is the case.
		*/
		if (red_LED_state == 1)
			P1->OUT |= 0x1;
		else if(red_LED_state == 0)
			P1->OUT &= ~0x1;
	
		//turn off the RGB LED
		P2->OUT &= ~(BIT0 | BIT1 | BIT2);

	}else if (LED_selection == RGB_LED){
		//turn off the red LED
		P1->OUT &= ~ BIT0;

		//set the RGB LED to a color corresponding to it's current state 
		P2->OUT = (uint8_t)(RGB_LED_state);
		}
}


/*Interrupt handler for port 1. Activated when switch one or two is clicked*/
void PORT1_IRQHandler(void){
	
	if(P1IFG & BIT1){ //switch one has been clicked, so we will change the LED selected

		P1IFG &= ~BIT1; // clear the interrupt flag

		//change the LED that is selected
		if(LED_selection == RED_LED)
			LED_selection = RGB_LED;
		else 
			LED_selection = RED_LED;


	}
	else if(P1IFG & BIT4) { // second switch has been selected
			P1IFG &= ~BIT4;  //clear the interrupt flag

			//Toggle whether the timer is on or off
			TA0CTL ^= (uint16_t)(BIT4);
			

		}
	updateLED();
	return;

}

/*configure timerA0 to repeatedly count to 1 second, at which time it will produce an interupt
	  After reaching 1 second, timerA0 resets immediately to 0, and starts counting up again.*/
void configureTimer(){
	
	
	TA0CTL &= (uint16_t)(~(BIT5 | BIT5)); //stop the timer 
	
	TA0CTL &= (uint16_t)(BIT0);  //reset timer 
	
	TA0CTL |= (uint16_t)(~BIT9); //select SMCLK as the clock source (32.768 khz)
	TA0CTL &= (uint16_t)(BIT8);  //this bit needs to be cleared to use SMCLK as the clock source
	
	TA0CTL &= (uint16_t)(~(BIT7 | BIT6));        //set the input divider to the clock as 1 (ID register)
	
	TA0EX0 &= (uint16_t)(~(BIT0 | BIT1 | BIT2)); // set the input divider as 1 (input divider expansion)
	
	TA0CCR0 = (uint16_t)(ONE_SECOND); //count up to 1 second 
	
	TA0CTL &= (uint16_t)(~BIT0); //clear interrupt flag
	
	TA0CTL |= (uint16_t)(BIT1);  //enable interrupts
	
	TA0CTL |= (uint16_t)(BIT4);  // upmode counter
	
	//enable TA interrupts in the NVIC
	NVIC_SetPriority(TA0_N_IRQn, 3);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
}

/*configure p1.1 and p1.4 sa input pins that are active low
	  configure p1.0 and p2.0, p2.1, p2.2 as output pins.
	  p1.0 is connected to a red LED
	  p2.0...p2.3 are the three cathodes connected to an RGB LED*/
void configureIO(){
	
	
	
	//configure pins as GPIO
	P1->SEL0 &= ~(BIT0 | BIT1 | BIT4);  //configuring pins 0,1 and 4 of port 1 as GPIO
	P1->SEL1 &= ~(BIT0 | BIT1 | BIT4);  
	
	P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);  //configuring pins 0,1, and 2 of port 2 as GPIO
	P2->SEL1 &= ~(BIT0 | BIT1 | BIT2); 

	//configuring the input pins
	P1->DIR &= ~(BIT1 | BIT4);  //set the pins as input (port 1, pin 1 and 4)
	P1->REN |=  (BIT1 | BIT4);  //set the resistor for the input pins
	P1->OUT |=  (BIT1 | BIT4);  //set the resistors as pull up resistors


	//configuring the output pins
	P2->DIR |= (BIT0 | BIT1 | BIT2);  // setting pins 0, 1, 2 as outputs (powers RGB led)
	P1->DIR |=  BIT0;                 // setting pin 0 (port 1) as output (powers red LED)
	P1->DS &= ~ BIT0;                 //set the drive strength of the output pins as 'regular strength' for port 1 pins
	P2->DS &= ~(BIT0 | BIT1 | BIT2);  //set the drive strength of the output pins as 'regular strength' for port 2 pins

	//configuring interrupts
	P1IES |=  (BIT1 | BIT4); //select falling edge as trigger generator
	P1IFG &= ~(BIT1 | BIT4); //remove any flags that are up right now relating to our input pins
	P1IE |=   (BIT1 | BIT4); //enable interrupts on pins 1 and 4
	
	//NVIC configuration
	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);
}

/*interrupt handler for timer. This function is called when the timer counts to 1 second*/
void TA0_N_IRQHandler(void) {
	
	TA0CTL &= (uint16_t)(~BIT0); //clear the flag
	
	if(LED_selection == RGB_LED)
		RGB_LED_state = (RGB_LED_state + 1 ) % 0x8; //flip to the next color on the RGB LED
	else if(LED_selection == RED_LED)
		red_LED_state ^= BIT0;
	
	updateLED();
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
