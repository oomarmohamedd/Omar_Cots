
/**********************************************************************************/
/**********************************************************************************/
/************************   Author: Omar Mohamed      *****************************/
/************************   File: CLCD_Program.h    *****************************/
/************************   Version: 1.0              *****************************/
/**********************************************************************************/
/**********************************************************************************/

#include"STD_TYPES.h"
#include"BIT_MATH.h"
#include "defines.h"
#include <util/delay.h>
#include "DIO_interface.h"

#include"CLCD_interface.h"
#include"CLCD_prv.h"
#include"CLCD_cfg.h"

static void voidSendEnablePulse(void)
{
	/*Send enable pulse*/
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,DIO_u8PIN_HIGH);
	_delay_ms(2);
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,DIO_u8PIN_LOW);
}


static void voidSetLCDHalfDataPort(uint8 Copy_u8Nipple)
{
	DIO_u8SetPinValue(CLCD_DATA_PORT,CLCD_D4_PIN,GET_BIT(Copy_u8Nipple,0));
	DIO_u8SetPinValue(CLCD_DATA_PORT,CLCD_D5_PIN,GET_BIT(Copy_u8Nipple,1));
	DIO_u8SetPinValue(CLCD_DATA_PORT,CLCD_D6_PIN,GET_BIT(Copy_u8Nipple,2));
	DIO_u8SetPinValue(CLCD_DATA_PORT,CLCD_D7_PIN,GET_BIT(Copy_u8Nipple,3));
}


void CLCD_voidSendCmd(uint8 Copy_u8Cmd)
{
	/*Set RS pin to low for command*/
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_RS_PIN,DIO_u8PIN_LOW);

#if CLCD_RW_CTRL_EN	== ENABLED
	/*Set RW pin to low for write operation*/
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_RW_PIN,DIO_u8PIN_LOW);
#endif

#if CLCD_OP_MODE == EIGHT_BIT_MODE
	/*Send the command on data pins*/
	DIO_u8SetPortValue(CLCD_DATA_PORT,Copy_u8Cmd);

	voidSendEnablePulse();

#elif CLCD_OP_MODE == FOUR_BIT_MODE

	voidSetLCDHalfDataPort(Copy_u8Cmd>>4);	/*send the most 4 bits to the LCD*/
	voidSendEnablePulse();

	voidSetLCDHalfDataPort(Copy_u8Cmd);		/*send the least 4 bits to the LCD*/
	voidSendEnablePulse();
#endif
}

void CLCD_voidSendData(uint8 Copy_u8Data)
{
	/*Set RS pin to high for Data*/
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_RS_PIN,DIO_u8PIN_HIGH);

#if CLCD_RW_CTRL_EN	== ENABLED
	/*Set RW pin to low for write operation*/
	DIO_u8SetPinValue(CLCD_CTRL_PORT,CLCD_RW_PIN,DIO_u8PIN_LOW);
#endif

#if CLCD_OP_MODE == EIGHT_BIT_MODE
	/*Send the command on data pins*/
	DIO_u8SetPortValue(CLCD_DATA_PORT,Copy_u8Data);

	voidSendEnablePulse();

#elif CLCD_OP_MODE == FOUR_BIT_MODE

	voidSetLCDHalfDataPort(Copy_u8Data>>4);	/*send the most 4 bits to the LCD*/
	voidSendEnablePulse();

	voidSetLCDHalfDataPort(Copy_u8Data);	/*send the least 4 bits to the LCD*/
	voidSendEnablePulse();
#endif

}

void CLCD_voidinit(void)
{
	/*wait for more than 30ms after power on*/
	_delay_ms(40);

	/*Function set command: 2 lines, 5*7 font size*/
#if CLCD_OP_MODE == EIGHT_BIT_MODE
	CLCD_voidSendCmd(0b00111000);

#elif CLCD_OP_MODE == FOUR_BIT_MODE
	voidSetLCDHalfDataPort(0b0010);
	voidSendEnablePulse();
	voidSetLCDHalfDataPort(0b0010);
	voidSendEnablePulse();
	voidSetLCDHalfDataPort(0b1000);
	voidSendEnablePulse();
#endif

	/*Display on/off control: display enable, cursor and blink disable*/
	CLCD_voidSendCmd(0b00001100);

	/*Display clear*/
	CLCD_voidSendCmd(1);

}
uint8 CLCD_u8SendString(const char* Copy_chString)
{
	uint8 Local_u8ErrorState= OK;
	if(Copy_chString != NULL)
	{
		uint8 Local_u8Counter=0u;

		while(Copy_chString[Local_u8Counter] != '\0')
		{
			CLCD_voidSendData(Copy_chString[Local_u8Counter]);
			Local_u8Counter++;
		}
	}
	else
	{
		Local_u8ErrorState=NOK;
	}




	return Local_u8ErrorState;
}
void CLCD_voidSendNumber(uint32 Copy_u32Number)
{
	uint8 Local_u8RightMost;
	uint32 Local_u32Result=0;
	uint8 Local_u8Counter=0;

	while(Copy_u32Number != 0)
	{
		Local_u8RightMost=Copy_u32Number%10;
		Local_u32Result=Local_u32Result*10+Local_u8RightMost;
		Copy_u32Number/=10;
		Local_u8Counter++;
	}

	while(Local_u32Result != 0 || Local_u8Counter!=0 )
	{
		if(Local_u32Result!=0)
		{
			Local_u8RightMost=Local_u32Result%10;
			CLCD_voidSendData(Local_u8RightMost+48);
			Local_u32Result/=10;
		}
		else
		{
			CLCD_voidSendData(0+48);

		}
		Local_u8Counter--;
	}
}
void CLCD_voidGoToXY(uint8 Copy_u8XPos,uint8 Copy_u8Ypos)
{
	uint8 Local_u8DDRamAdd;
	if(Copy_u8Ypos == 0u)
	{
		Local_u8DDRamAdd=Copy_u8XPos;

	}
	else if( Copy_u8Ypos ==1u)
	{
		Local_u8DDRamAdd=0x40 + Copy_u8XPos;
	}
	/*Set bit 7 for set DDRAM address command*/
	Local_u8DDRamAdd |=1<<7u;

	CLCD_voidSendCmd(Local_u8DDRamAdd);
}

void CLCD_voidSendSpecialCharacter(uint8* Copy_pu8Pattern,uint8 Copy_u8PatternNum,uint8 Copy_u8XPos,uint8 Copy_u8YPos)
{
	uint8 Local_u8CGRAMAdd= Copy_u8PatternNum*8u;
	uint8 Local_u8LoopCounter;

	/*Set Bit Number 6 for Set CGRAM address command */
	Local_u8CGRAMAdd |= 1<<6u;

	/*Set CGRAM Add.*/
	CLCD_voidSendCmd(Local_u8CGRAMAdd);

	/*Write the pattern into CGRAM*/
	for(Local_u8LoopCounter=0; Local_u8LoopCounter<8u;Local_u8LoopCounter++)
	{
		CLCD_voidSendData(Copy_pu8Pattern[Local_u8LoopCounter]);
	}

	/*Go To DDRAM to display the pattern*/
	CLCD_voidGoToXY(Copy_u8XPos,Copy_u8YPos);

	/*Display the pattern written inside CGRAM*/
	CLCD_voidSendData(Copy_u8PatternNum);
}


