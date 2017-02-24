/*!
 * \file cos_ser.h
 * \brief routines for serial communication on renesas RX63N Controller
 *
 *   @par Author:     Ernst Forgber (Fgb)
 *
 ***********************************************************************
 *
 *   @par History       :
 *   @verbatim
 * Ver  Date        Author            Change Description
 * 0.0  04.12.2008  E. Forgber        - First Version
 * 0.1  20.03.2013  E. Forgber        Dokumentation auf Deutsch umgestellt
 * 0.2  09.10.2015  E. Forgber        switch to renesas controller
 *
 *   @endverbatim
 ****************************************************************************/
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

#ifndef _avr_ser_h_
#define _avr_ser_h_

//#include <avr/io.h>
//#include <stdint.h>

#include "cos_configure.h"

#include "cos_types.h"

#if COS_PLATFORM == PLATFORM_RENESAS_RX63N
    #include "poll_serial_interface.h"
    #include <stdio.h>
#endif // COS_PLATFORM


void serInit(uint32_t baudRate);
void serPutc(char x);
void serPuts(char *pt);

void serOutUint8Bin(uint8_t x);
void serOutUint8Hex(uint8_t x);

void serOutUint16Dec(uint16_t x);
void serOutUint16Hex(uint16_t x);
void serOutInt16Dec(int16_t x);

void serOutUint32Dec(uint32_t x);
void serOutUint32Hex(uint32_t x);
void serOutInt32Dec(int32_t x);


uint8_t serGetc(void);
int16_t serPollc(void);
uint8_t serGets(char *pt);
int8_t serInUint16Dec(uint16_t *x);
int8_t serInInt16Dec(int16_t *x);
int8_t serInUint16Hex(uint16_t *x);


#endif




