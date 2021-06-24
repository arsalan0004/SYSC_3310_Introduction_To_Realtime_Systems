/*
For Lab 1 of SYSC 3310 (Summer 2021)
File Name: main.c
Author Name: Arsalan Syed
Author Student ID: 101169528
*/


#include "my_function.h"

int main(void){
	extern int x;
	x = 12;
	increment();
	increment();

	return 0;
}
