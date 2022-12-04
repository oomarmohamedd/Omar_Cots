/**************************************************************/
/**************************************************************/
/*********		Author: Omar Mohamed	    *******************/
/*********		File: PORT_prog.h	        *******************/
/*********		Version: 1.00				*******************/
/**************************************************************/
/**************************************************************/

#include"STD_TYPES.h"

#include"PORT_reg.h"
#include"PORT_interface.h"
#include"PORT_cfg.h"
#include"PORT_private.h"

void PORT_voidInit(void)
{
	DDRA = PORTA_DIR;
	DDRB = PORTB_DIR;
	DDRC = PORTC_DIR;
	DDRD = PORTD_DIR;
	
	PORTA=PORTA_INTIAL_VAL;
	PORTB=PORTB_INTIAL_VAL;
	PORTC=PORTC_INTIAL_VAL;
	PORTD=PORTD_INTIAL_VAL;

}
