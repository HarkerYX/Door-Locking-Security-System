/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Tamer_Zedan
 *
 *******************************************************************************/
#include "uart.h"

#include "external_eeprom.h"

#include "motor.h"

#include "Timer0.h"

#define READY 0x01

#define NOTREADY 0x00

uint16 Tick=0;


/*function to calculate numbers of Over flow*/
void Timer0_Tick(void){
	Tick++;
}

int main(void){
	SREG|=(1<<7); /*enable global interrupt*/

	Timer0_ConfigType T0_Config={clk_1024}; /*Set timer0 configuration*/

	Set_Callback(Timer0_Tick);  /*Set call back function*/

	/******************************************************************************
	 * key[6] to receive the password and save it in EEPROM                       *
	 * check to ensure that the user enter the right password                     *
	 * Door_key used to know what the user want (open the door ,change password)  *
	 * wrong used to count the wrong passwords                                    *
	 ******************************************************************************/

	uint8 key[6]={0},i=0,check=0,Door_key=0,wrong=0;

	UART_init(); /*Initialize UART*/

	EEPROM_init(); /*Initialize UART*/

	UART_receiveString(key); /*  Receive the password for the first time */

	DDRD|=(1<<7);
	PORTD&=~(1<<7);

	/*Save the password in EEPROM*/
	for(i=0;i<6;i++){
		EEPROM_writeByte(0x0040+i,key[i]);
		_delay_ms(10);
	}

	while(1){

		UART_receiveString(key);/* Take password from the user*/

		Door_key=UART_recieveByte(); /*receive what he want (open door,change password*/

		/*if he pressed '*' the door will be opened if the password is right*/
		if(Door_key=='*'){

			/*This loop to check the password is the same as in EEPROM*/
			for(i=0;i<6;i++){
				EEPROM_readByte(0x0040+i,&check);
				if(check==key[i]){

				}else{
					break;
				}
			}

			/*************************************************************
			 * if the password is right the C_ECU Send READY for HMI_ECU
			 * and start to calculate the time for opening the door 15s
			 * and start to calculate the time for closing the door 15s
			 *************************************************************/
			if(i==6){
				UART_sendByte(READY);
				Timer0_init_Normal_Mode(&T0_Config);
				Motor_run();
				while(Tick<457);
				Timer0_Deinit();
				Tick=0;
				Timer0_init_Normal_Mode(&T0_Config);
				Motor_Reverse();
				while(Tick<457);
				Timer0_Deinit();
				Tick=0;
				Motor_Stop();
			}
			/*if the password is not right stop the motor and increment the wrong*/
			else{
				UART_sendByte(NOTREADY); /*Send to HMI_ECU the password is wrong */
				Motor_Stop(); /* Stop Motor*/
				wrong++;

				/* if the user enter the password wrong three time Stop the system for 1 minute*/
				if(wrong==3){
					Timer0_init_Normal_Mode(&T0_Config);
					PORTD|=(1<<7);
					while(Tick!=1828);
					Timer0_Deinit();
					PORTD&=~(1<<7);
					wrong=0;
				}
				PORTD|=(1<<7);
				_delay_ms(500);
				PORTD&=~(1<<7);

			}

		}
		/* if he want to change the password */
		else{
			for(i=0;i<6;i++){
				EEPROM_readByte(0x0040+i,&check);
				if(check==key[i]){

				}else{
					break;
				}
			}
			if(i==6){
				UART_sendByte(READY);
				UART_receiveString(key);
				for(i=0;i<6;i++){
					EEPROM_writeByte(0x0040+i,key[i]);
					_delay_ms(10);
				}

			}else{
				UART_sendByte(NOTREADY);
				Motor_Stop();
				wrong++;
				if(wrong==3){
					Timer0_init_Normal_Mode(&T0_Config);
					PORTD|=(1<<7);
					while(Tick!=1828);
					Timer0_Deinit();
					PORTD&=~(1<<7);
					wrong=0;
				}
				PORTD|=(1<<7);
				_delay_ms(500);
				PORTD&=~(1<<7);

			}

		}

	}
}
