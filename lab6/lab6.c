/**
Lab 6 for SYSC 3310A: Intro to Real Time Systems (Summer 2021)

##Program Summary
This program uses UART to communicate (via a COM port) with a PC.
Everytime the state of the IC changes, it notifies the PC, and tells it what state it is in.
The IC can take on 4 different states, which are represented by an onboard RGB LED.
The following list matches the state of the system with the color of the RGB LED:

State 0:  RGB LED OFF
State 1:  RED
State 2:  GREEN
State 3:  YELLOW/ORANGE

The state of the IC can also be changed by sending it an integer value ranging from 0-3.
The IC will take on a state corresponding to that integer. This change is visualized by the onboard RGB LED

##Functionality
This project works by using the enhanced universal communication interface (eUSCI) module found
on the MSP432. This module connects through the microUSB to the computer, allowing it to
both send and recieve data to/from the computer.

##Objective
To learn how to use timers generally, and to set up ISRs that rely on timers
author name: 		    Arsalan Syed
author student ID: 	101169528
date: 			        July 1st, 2021
*/

#include "msp.h"
#include <stdio.h>


#define ONE_SECOND 32767//32768 - 1 // assuming a 32768 Hz clock, this is how many ticks are required to count 1 second

static int8_t LED_state = 0;
const uint8_t p1 = (1 << 1);
const uint8_t p4 = (1 << 4);
const uint8_t p14 = p1 | p4;
const uint8_t p0 = 0x01;
const uint8_t p012 = (p0 | p1 | (1 << 2));

/*
initiliaze the UART port connected through the USB
*/
void UART0_init(void){
	 EUSCI_A0->CTLW0 |=  0x1;             // put UART module in reset mode for configuration
   EUSCI_A0->CTLW0  =  0x0081;         // set one stop bit, disable parity, set clock as SMCLK, allow 8-bit data transfer */
   EUSCI_A0->BRW    =  0x1A;               // set prescaler to allow baudrate of 115200*/
   P1->SEL0        |=  0x0C;                 // set P1.3, P1.2 alternative function for UART */
   P1->SEL1        &= ~0x0C;
   EUSCI_A0->CTLW0 &= ~0x1;            // take UART out of reset mode */

}
/*
send a char array to the computer indicating the state
*/
void UART0_sendState(){
	char s[8] = "STATE X ";
		switch(LED_state){
			case 0:
				s[6] = '1';
				break;
			case 1:
				s[6] = '2';
				break;
			case 2:
				s[6] = '3';
				break;
			case 3:
				s[6] = '4';
				break;
		}

	for(int i = 0; i <8; i++){
		while(!(EUSCI_A0->IFG&0x02)); //wait for transmission buffer to be empty

		EUSCI_A0->TXBUF = s[i]; //send each letter individually
		}
	}



/*
Interrupt handler for port 1. Activated when switch one or two is clicked
*/
void PORT1_IRQHandler(void){
	if(P1IFG & BIT1){ //switch one has been clicked, so increment the LED_state

		P1IFG &= ~BIT1;    						 // clear the interrupt flag
		LED_state = (LED_state + 1)%4; // and increment the LED state


	}
	else if(P1IFG & BIT5) {   // second switch has been selected, so

		P1IFG &= ~0x10;  				// clear the interrupt flag
		LED_state--;            // and decrement the LED state
		if(LED_state < 0 )			// make sure that the LED state rolls over it if goes below zero
			LED_state = 3;
	}


	//set the RGB LED
	P2->OUT = (uint8_t)(LED_state);

	//let the computer know the new state through UART
	UART0_sendState();

	return;

}


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

int main(void){

	//disable watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	configureIO();
	UART0_init();

	//enable global interrupts
	__ASM("CPSIE I");

	//done with configuration
	while(1){
		__ASM("WFI"); //wait for interrupts
	}
	return 0;
}
