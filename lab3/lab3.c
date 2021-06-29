/**
Lab 3
Arsalan Syed
101169528
*/

#include "msp.h"
#include <stdio.h>

#define RED_LED 0
#define RGB_LED 1
#define PORT2_INTERRUPT_PRIORITY 2

static uint8_t LED_selection = RGB_LED;
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

/*
Interrupt handler for port 1. Activated when switch one or two is clicked
*/
void PORT1_IRQHandler(void){
	//interrupt detected at port 1. Which pin caused the interrupt?
	
	if(P1IFG & BIT1){ //switch one has been clicked (pin 1 has been set), so we will change the LED selected
		P1IFG &= ~BIT1; // clear the interrupt flag
		
		if(LED_selection == RED_LED) //toggle which LED is selected
			LED_selection = RGB_LED;
		
		else 
			LED_selection = RED_LED;


	}
	else if(P1IFG & BIT4){ //switch four has been clicked (pin 4 has been set), so we will change the LED selected
		P1IFG &= ~BIT4;  //clear the interrupt flag
	
		if(LED_selection == RGB_LED)
			RGB_LED_state = (RGB_LED_state + 1 ) % 0x8; ////increment the LED state, thereby changing the color 
		else if (LED_selection == RED_LED)
			red_LED_state ^= 0x1; //toggle the red LED state (on/off) or (0/1)
	
		
	}
	
	updateLED();
	return;

}

int main(void){


	//disable watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	//setting GPIO Function
	P1->SEL0 &= ~(BIT0 | BIT1 | BIT4);
	P1->SEL1 &= ~(BIT0 | BIT1 | BIT4);

	P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);
	P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);


//disable the watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	//disable all interrupts
	__ASM("CPSID I");

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

	//enable global interrupts
	__ASM("CPSIE I");

	//NVIC configuration
	NVIC_SetPriority(PORT1_IRQn, PORT2_INTERRUPT_PRIORITY);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);

	//done with configuration
	while(1){
		__ASM("WFI"); //wait for interrupts
	}
	return 0;
}
