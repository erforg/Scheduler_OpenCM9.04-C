/*!
 ***********************************************************************
 *   @file            cos_configure.h
 *   @par Project:    Co-operative scheduler COS
 *   @par Module:     Select platform processor
 *
 *   @brief  This file is used to select the platform the COS scheduler
 *           will be running on. Define
 *           the PLATFORM symbol to one of the predefined symbols
 *           and re-compile the module.
 *
 *
 * @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1
 *
 ***********************************************************************/
/*******************************************************************************

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
 *******************************************************************************/
    
#ifndef _cos_codecontrol_h_
#define _cos_codecontrol_h_

/********* supported platforms, don't edit this! *******/
#define PLATFORM_OPEN_CM_9_04       1  /*!<  platform: openCM , ARM */
#define PLATFORM_ARDUINO            2  /*!<  platform: arduino */
#define PLATFORM_RENESAS_RX63N      3  /*!<  platform: renesas RX63N */

/********* end of: "don't edit this" *******/


/***********************************************************************/
/******* select the platform COS will be running on ********************/
/******* un-comment ONLY ONE of the following options ******************/
/***********************************************************************/
#define COS_PLATFORM        PLATFORM_OPEN_CM_9_04 /*!< select COS platform */
//#define COS_PLATFORM      PLATFORM_ARDUINO
//#define COS_PLATFORM      PLATFORM_RENESAS_RX63N




#endif

