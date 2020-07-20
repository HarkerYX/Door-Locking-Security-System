 /******************************************************************************
 *
 * Module: GPT
 *
 * File Name:timer.c
 *
 * Description: Source file for the GPT AVR driver
 *
 * Author: Tamer_Zedan
 *
 *******************************************************************************/
#include "Timer0.h"

/*callback function*/
static  void (*g_callBackPtr)(void) = NULL;


ISR(TIMER0_OVF_vect){
		if(g_callBackPtr!=NULL){
			(*g_callBackPtr)();
		}
}

ISR(TIMER0_COMP_vect){
		if(g_callBackPtr!=NULL){
			(*g_callBackPtr)();
		}
}

/*Initialize Timer0 in Normal Mode*/

void Timer0_init_Normal_Mode(Timer0_ConfigType *Timer0_config){
	TCNT0=195;
	TCCR0 |=(1<<FOC0)|(Timer0_config->clk);
	TIMSK|=(1<<TOIE0);
}

/*Initialize Timer0 in Compare Mode*/

void Timer0_init_Comp_Mode(Timer0_ConfigType *Timer0_config){
	TCNT0=0;
	TCCR0 |=(1<<FOC0)|(Timer0_config->clk)|(1<<COM01);
	TIMSK|=(1<<OCIE0);
}

/*Function to Set Callback Function*/

void Set_Callback(void (*T_callBackPtr)(void)){
	g_callBackPtr=T_callBackPtr;
}

/*Disable Timer0*/

void Timer0_Deinit(void){
	TCCR0=0;
}
