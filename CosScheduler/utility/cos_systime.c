/*!
 ********************************************************************
   @file            cos_systime.c

   @brief  This modul provides basic time functions. On Arduino and openCM, use 'millis()' instead

   For platforms other than Arduino and openCM: Time is measured in ticks.
   A Timer overflow interrupt is used to increment a tick counter.

   @par Author    : Ernst Forgber (Fgb)
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author   | Change Description
   0.0     | 03.04. 2013 | Fgb      | First Version
   0.1     | 01.08. 2013 | Fgb      | bugfix in _milliSecToTicks()
   0.2     | 09.10. 2015 | Fgb      | change to renesas controller RX63N
   0.3     | 11.11. 2016 | Fgb      | openCM compatibility included
   @endverbatim

 ********************************************************************/


/**************************************************************************

Copyright 2016 Ernst Forgber


This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "cos_systime.h"



/**************************************************************************
*      Renesas RX63N Verion                                               *
**************************************************************************/
#if COS_PLATFORM == PLATFORM_RENESAS_RX63N

#include "iodefine.h"
#include "isr.h"

#define MICROSEC_PER_TICK 1000

/************************************************************
 * Debugging section Renesas RX63N:
 * Timer ISR used LED15 for debugging
 ************************************************************/
#define DEBUG 0   // 0 for no debugging with LEDs

#define _LED4		PORTD.PODR.BIT.B5
#define _LED5		PORTE.PODR.BIT.B3
#define _LED6		PORTD.PODR.BIT.B2
#define _LED7		PORTE.PODR.BIT.B0
#define _LED8		PORTD.PODR.BIT.B4
#define _LED9		PORTE.PODR.BIT.B2
#define _LED10		PORTD.PODR.BIT.B1
#define _LED11		PORTD.PODR.BIT.B7
#define _LED12		PORTD.PODR.BIT.B3
#define _LED13		PORTE.PODR.BIT.B1
#define _LED14		PORTD.PODR.BIT.B0
#define _LED15		PORTD.PODR.BIT.B6

#if DEBUG

static volatile unsigned char _led15_state=0;

#define _LedInitPortDirections()				\
{	/*
	 * port output data register (PODR)
	 * des ports d mit 1en belegen,
	 * d.h. leds an port d ausschalten
	 */											\
	PORTD.PODR.BYTE  = 0xFF;					\
												\
	/*
	 * port output data register (PODR)
	 * des ports e mit 1en belegen,
	 * d.h. leds an port e ausschalten
	 */											\
	PORTE.PODR.BYTE |= 0x0F;					\
												\
	/*
	 * port direction register (PDR)
	 * des ports d mit 1en belegen,
	 * d.h. leds an port d als
	 * ausgÃ¤nge definieren
	 */											\
	PORTD.PDR.BYTE   = 0xFF;					\
												\
	/*
	 * port direction register (PDR)
	 * des ports e mit 1en belegen,
	 * d.h. leds an port e als
	 * ausgÃ¤nge definieren
	 */											\
	PORTE.PDR.BYTE  |= 0x0F;					\
}

#else
  #define _LedInitPortDirections()
#endif  // DEBUG
/************************************************************
 * End of Debugging section
 ************************************************************/








/****************************************************************
 * static variables
 ****************************************************************/

static volatile uint16_t systemTimeInTicks=0; /*!< private tick counter */




/*!
 **********************************************************************
 * @par Description:
   Interrupt service routine (ISR) for timer event.
 ************************************************************************/
/*******************************
 * first try was as shown in 'RX63N_Update.pdf, pp 421. BUT: that solution
 * didn't work at all! Therefore Timer CMT0 with interrupt
 * 'compare match timer0' is used instead, same as used by FreeRtos.
 *******************************/
void INT_Excep_CMT0_CMI0(void)
{
#if DEBUG
	if(_led15_state)
	{
		_LED15=0;
		_led15_state = 0;
	}
	else
	{
		_LED15=1;
		_led15_state = 1;
	}
#endif
    systemTimeInTicks++;   // increment tick counter
}




/*!
 **********************************************************************
 * @par Description:
   Initializes timer CMT0 for use as system clock. Time is measured in
   ticks. One tick correspond to a 'compare match interrupt' of timer0.
   The compare match value is adjusted to have one tick per millisecond.
 ************************************************************************/
 void _initSystemTime(void)
{
	 	// take from FreeRtos
	 	/* switch off register protection
	 	   PRCR = protect register. */
	 	SYSTEM.PRCR.WORD = 0xA50B;

	 	/* Modul-Stop-State of Compare-Match-Timer 0 is deactivated.
	 	   MSTP = module stop state, it is a Makro defined in  iodefine.h. */
	 	MSTP( CMT0 ) = 0;

	 	/* switch on register protection */
	 	SYSTEM.PRCR.WORD = 0xA500;

	 	/* activate interrupt
	 	   CMT0 = compare match timer 0,
	 	   CMCR = compare match timer control register,
	 	   CMIE = compare match interrupt enable. */
	 	CMT0.CMCR.BIT.CMIE = 1;

	 	/* set compare value for interrupt
	 	   CMCOR = compare match timer constant register. */
	 	//CMT0.CMCOR = ( unsigned short ) ( ( ( configPCLKB_HZ / configTICK_RATE_HZ ) - 1 ) / 8 );
	    //CMT0.CMCOR = 0x5555; // 3.5 ms
	 	//CMT0.CMCOR = 0x2222; // 1.5 ms
	 	//CMT0.CMCOR = 0x1560; // 0.917 ms
	 	//CMT0.CMCOR = 0x1600; // 0.956 ms
	 	CMT0.CMCOR = 0x1700; // 0.99 ms

	 	/* PCLKB/8 is clock source
	 	   CKS = clock select. */
	 	CMT0.CMCR.BIT.CKS = 0;

	 	/* set interrupt priority */
	 	//IPR( CMT0, CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;
	     IPR(CMT0,CMI0) = 2;

	 	/* activate interrupt in the ICU */
	 	IEN( CMT0, CMI0 ) = 1;

	 	/* start the timer
	 	   CMSTR0 = compare match timer start register 0,
	 	   STR0   = count start 0. */
	 	CMT.CMSTR0.BIT.STR0 = 1;
	 #if DEBUG
	 	_LedInitPortDirections();
	 #endif
}
//-------------------------------------------------------


/*!
 **********************************************************************
 * @par Description:
 *   Returns the number of microseconds, that
 *   correspond to a timer tick.
 * @see
 * @arg _initSystemTime()
 *
 * @retval                - Tick-interval in microseconds
  ************************************************************************/
 uint16_t _microSecPerTick(void)
{   return MICROSEC_PER_TICK;
}

/*-------------------------------------------------------*/

/*!
 **********************************************************************
 * @par Description:
 *   Returns the system time in ticks.
 * @retval                - system time in ticks
 ************************************************************************/
 uint16_t _gettime_Ticks(void)
{   uint16_t t;
    //cli();  // deactivate INT, mutex for tick variable, AVR only
    t = systemTimeInTicks;
    //sei(); // aktivate INT, Timer0 ISR may change systemTimeInTicks, AVR only
    return t;
}
/*-------------------------------------------------------*/
/*!
 **********************************************************************
 * @par Description:
 *   Given the number of milliseconds, the function returns the corresponding
 *   number of ticks. The constant MICROSEC_PER_TICK has to be set
 *   accordingly.
 *
 * @param  milliSec  - IN, time in milliseconds
 *
 * @retval                - time in ticks
 *
 ************************************************************************/
 uint16_t _milliSecToTicks(uint16_t milliSec)
{   uint32_t t_ms;

    t_ms = ((uint32_t) milliSec*1000)/MICROSEC_PER_TICK;
    if (t_ms<1) t_ms=1;
    return ((uint16_t) t_ms);
}
/*-------------------------------------------------------*/




#endif // COS_PLATFORM
/**************************************************************************
*   END OF:    Renesas RX63N Verion                                       *
**************************************************************************/



/**************************************************************************
*      openCM9.04  Verion                                                 *
**************************************************************************/

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04

/*********************************************************************
*  these function are empty and provided for compatibility. On openCM9.04
*  use 'millis()' instead!
*************************************************************************/

/*!
 ********************************************************************
  @par Description
  On platforms Arduino and openCM, this function is empty and
  provided for compatibility only.
 ********************************************************************/
void     _initSystemTime(void){ return; }

/*!
 ********************************************************************
  @par Description
  On platforms Arduino and openCM, this function provided for
  compatibility only. On these platforms, a tick is 1000 microseconds.
 ********************************************************************/
uint16_t _microSecPerTick(void){ return 1000; }

/*!
 ********************************************************************
  @par Description
  On platforms Arduino and openCM, this function is empty and
  provided for compatibility only.
 ********************************************************************/
uint16_t _gettime_Ticks(void){ return 0; }

/*!
 ********************************************************************
  @par Description
  On platforms Arduino and openCM, this function is provided for
  compatibility only. On these platforms, a tick is 1 millisecond.
 ********************************************************************/
uint16_t _milliSecToTicks(uint16_t milliSec){ return milliSec; }

#endif
/**************************************************************************
*   END OF:    openCM9.04  Verion                                         *
**************************************************************************/


