/******************************************************************************
 *
 * Module: UART
 *
 * File Name: Motor.c
 *
 * Description: Source file for the Motor driver
 *
 * Author: Tamer_Zedan
 *
 *******************************************************************************/
#include <avr/io.h>

void Motor_run(void)
{
	DDRB|=(1<<PB0)|(1<<PB1);
	PORTB|=(1<<PB0);
	PORTB&=~(1<<PB1);
}

void Motor_Reverse(void){
	PORTB^=(1<<PB1);
	PORTB^=(1<<PB0);
}
void Motor_Stop(void){
	PORTB&=~(1<<PB1);
	PORTB&=~(1<<PB0);
}
