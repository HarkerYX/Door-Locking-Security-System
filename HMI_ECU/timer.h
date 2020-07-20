 /******************************************************************************
 *
 * Module: GPT
 *
 * File Name: timer.h
 *
 * Description: Header file for the GPT AVR driver
 *
 * Author: Tamer_Zedan
 *
 *******************************************************************************/
#include "common_macros.h"

#include "std_types.h"

#include "micro_config.h"

#include <avr/io.h>

#include <avr/interrupt.h>

#ifndef TIMER0_H_
#define TIMER0_H_

#define NULL ((void*)(0))

typedef enum{
	No_clock,F_CPU_CLK,clk_8,clk_64,clk_256,clk_1024,External_clock_Falling,External_clock_Rising
}Clock_Select;


typedef struct{
	Clock_Select clk;
}Timer0_ConfigType;

void Timer0_init_Normal_Mode(Timer0_ConfigType *Timer0_config);

void Timer0_init_Comp_Mode(Timer0_ConfigType *Timer0_config);

void Set_Callback(void (*T_callBackPtr)(void));

void Timer0_Deinit(void);

#endif /* TIMER0_H_ */
