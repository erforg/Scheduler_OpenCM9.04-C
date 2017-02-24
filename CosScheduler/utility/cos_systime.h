/*!
 ********************************************************************
   @file            cos_systime.h

   @brief  This modul provides basic time functions. On Arduino and openCM, use 'millis()' instead



   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author  | Change Description
   0.0     | 03.04. 2013 | Fgb     | First Version
   0.1     | 08.10. 2015 | Fgb     | change to renesas controller RX63N
   0.2     | 11.11. 2016 | Fgb     | openCM compatibility included

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



#ifndef avr_systime_h_
#define avr_systime_h_


#include "cos_configure.h"


//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <stdint.h>
#include "cos_types.h"

#define set_bit(sfr,bit)    sfr |= (1<<(bit))   /**< macro to set a dedicated bit */
#define clear_bit(sfr,bit)  sfr &= ~(1<<(bit))  /**< macro to clear a dedicated bit */


void     _initSystemTime(void);
uint16_t _microSecPerTick(void);
uint16_t _gettime_Ticks(void);
uint16_t _milliSecToTicks(uint16_t milliSec);


#endif




