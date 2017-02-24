/*!
 ********************************************************************
   @file            cos_types.h
   @par Project   : co-operative scheduler (COS)
   @par Module    : Type definitions for COS Scheduler

   @brief  Type definitions for COS Scheduler for several platforms


   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1


   @par Description
   This header file provides basic data types used by COS.

   @par History   :
   @verbatim
   Version | Date        | Author  | Change Description
   0.0     | 08.10. 2015 | Fgb     | created for renesas controller RX63N
   0.1     | 11.11. 2016 | Fgb     | compatibility to openCM included
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







#ifndef COS_TYPES_H_
#define COS_TYPES_H_

#include "cos_configure.h"

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    typedef char                    int8_t;     /**< 8 bit signed integer */
    typedef unsigned char           uint8_t;    /**< 8 bit unsigned integer */
    typedef short                   int16_t;    /**< 16 bit signed integer */
    typedef unsigned short          uint16_t;   /**< 16 bit unsigned integer */
    typedef int                     int32_t;    /**< 32 bit signed integer */
    typedef unsigned int            uint32_t;   /**< 32 bit unsigned integer */
    typedef long long               int64_t;    /**< 64 bit signed integer */
    typedef unsigned long long      uint64_t;   /**< 64 bit unsigned integer */
#endif // COS_PLATFORM

#if COS_PLATFORM == PLATFORM_RENESAS_RX63N
    /* for compatibility with gcc types: */
    #ifndef int8_t
        #define int8_t signed char
    #endif
    #ifndef uint8_t
        #define uint8_t unsigned char
    #endif
    #ifndef int16_t
        #define int16_t signed short
    #endif
    #ifndef uint16_t
        #define uint16_t unsigned short
    #endif
    #ifndef int32_t
        #define int32_t signed long
    #endif
    #ifndef uint32_t
        #define uint32_t unsigned long
    #endif
#endif // COS_PLATFORM



#endif /* COS_TYPES_H_ */
