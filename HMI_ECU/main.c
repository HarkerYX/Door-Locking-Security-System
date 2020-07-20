/******************************************************************************
 *Module: main.c
 *
 * File Name: main.c
 *
 * Description: main file for the HMI_ECU
 *
 * Author: Tamer_Zedan
 *
 *******************************************************************************/

#include"lcd.h"

#include "keypad.h"

#include "uart.h"

#include "Timer0.h"

#define READY 0x01

uint16 Tick=0;

void Timer0_Tick(void){
	Tick++;
}

void Password_init(uint8 *key){
	LCD_clearScreen();
	uint8 count=0,check_numb=0;
	LCD_displayString("Please Enter New");
	LCD_goToRowColumn(1,0);
	LCD_displayString("Password:");
	for(count=0;count<5;count++){
		check_numb=KeyPad_getPressedKey();
		if(check_numb<=9&&check_numb>=0){
			key[count]=check_numb;
			LCD_displayCharacter('*');
			_delay_ms(300);
		}else{
			LCD_clearScreen();
			LCD_displayString("Please Enter Five Numbs!");
			_delay_ms(500);
			LCD_clearScreen();
			LCD_displayString("Please Enter New");
			LCD_goToRowColumn(1,0);
			LCD_displayString("Password:");
			count=0;
		}
	}
}

void Take_Password(uint8 *key){
	uint8 count=0;
	LCD_clearScreen();
	LCD_displayString("Please Enter");
	LCD_goToRowColumn(1,0);
	LCD_displayString("Password:");
	for(count=0;count<5;count++){
		key[count]=KeyPad_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	key[5]='#';
	key[6]='\0';
	UART_sendString(key);
}

void Take_Old_Password(uint8 *key){
	uint8 count=0;
	LCD_clearScreen();
	LCD_displayString("Please Enter Old ");
	LCD_goToRowColumn(1,0);
	LCD_displayString("Password :");
	_delay_ms(500);
	for(count=0;count<5;count++){
		key[count]=KeyPad_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	key[5]='#';
	key[6]='\0';
	UART_sendString(key);
}

int main()
{
	uint8 check[6],key[7],i=0,Door_key=0,wrong=0;;
	SREG|=(1<<7);
	Timer0_ConfigType T0_Config={clk_1024};   /*Set configuration for Timer0*/
	UART_init();  /*Initialize uart */
	LCD_init();  /* Initialize LCD */
	Set_Callback(Timer0_Tick);  /*Set Timer0 Call back function */

	/**************************************************************************
	 * This Loop to Take the password from the user in first time             *
	 * and check if enter the password right twice send it to C_ECU to save it*
	 **************************************************************************/

	while(1){
		Password_init(key);
		Password_init(check);
		for(i=0;i<5;i++){
			if(key[i]==check[i]){

			}else{
				break;
			}
		}
		if(i==5){
			break;
		}else{
			i=0;
		}
	}
	key[5]='#';
	key[6]='\0';
	UART_sendString(key); /* Send the password to Control ECU to Save it in EEPROM */
	while(1){
		LCD_clearScreen();
		LCD_displayString("* :Open Door");
		LCD_goToRowColumn(1,0);
		LCD_displayString("# :Change Pass");
		Door_key=KeyPad_getPressedKey();
		_delay_ms(300);
		/* if user want to open the door*/
		if(Door_key=='*'){
			/* take password from the user and store it in array and send array to C_ECU*/
			Take_Password(key);
			/* Send to C_ECU that the user want to open the door*/
			UART_sendByte(Door_key);
			/* if the Password is the same as in EEPROM Open the door*/
			Door_key=UART_recieveByte();
			if(Door_key==READY){
				LCD_clearScreen();
				LCD_displayString("Door is ");
				LCD_goToRowColumn(1,0);
				LCD_displayString("unlocking");
				Timer0_init_Normal_Mode(&T0_Config);
				while(Tick<457);
				Timer0_Deinit();
				LCD_clearScreen();
				Tick=0;
				LCD_displayString("Door is Locking");
				Timer0_init_Normal_Mode(&T0_Config);
				while(Tick<457);
				Timer0_Deinit();
				Tick=0;
			}
			/*if the Password is not right Print in LCD Wrong*/
			else{
				LCD_clearScreen();
				LCD_displayString("Wrong");
				wrong++;
				/* if the user enter the password wrong three times Stop the System for one minute*/
				if(wrong==3){
					Timer0_init_Normal_Mode(&T0_Config);
					LCD_clearScreen();
					LCD_displayString("Wrong..Wrong");
					while(Tick<1828);
					Timer0_Deinit();
					Tick=0;
					wrong=0;
				}else{
					_delay_ms(500);
				}
				LCD_clearScreen();
				LCD_displayString("Door is closed");
			}
		}
		/* if the user want to change password*/
		else if(Door_key=='#'){
			Take_Old_Password(key); /* take old password and send it to C_ECU*/
			UART_sendByte(Door_key); /* Send to C_ECU that the user want to change password*/
			Door_key=UART_recieveByte();
			/* if the password is Right Take new password from user*/
			if(Door_key==READY){
				/**************************************************************************
				 * This Loop to Take the password from the user in first time             *
				 * and check if enter the password right twice send it to C_ECU to save it*
				 **************************************************************************/
				while(1){
					Password_init(key);
					Password_init(check);
					for(i=0;i<5;i++){
						if(key[i]==check[i]){

						}else{
							break;
						}
					}
					if(i==5){
						key[5]='#';
						key[6]='\0';
						UART_sendString(key);
						break;
					}else{
						i=0;
					}
				}
			}
			/*if the Password is not right Print in LCD Wrong*/
			else{
				LCD_clearScreen();
				LCD_displayString("Wrong");
				wrong++;
				/* if the user enter the password wrong three times Stop the System for one minute*/
				if(wrong>=3){
					Timer0_init_Normal_Mode(&T0_Config);
					LCD_clearScreen();
					LCD_displayString("Wrong..Wrong");
					while(Tick<1828);
					Timer0_Deinit();
					Tick=0;
					wrong=0;
				}else{
					_delay_ms(500);
				}
			}
		}

	}
}

