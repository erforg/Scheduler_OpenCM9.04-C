/*!
 ********************************************************************
   @file            CosScheduler.h
   @par Project   : co-operative Scheduler
   @par Module    : CosScheduler

   @brief  openCM / Arduino version of the 'co-operative', stackless tasking system (COS)

   A task-switch ist performed by means of deticated macros.
   These macros have to be called by the task-function directly.
   They must not be used in nested function calls.



   @par Author    : Ernst Forgber (Fgb)
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts, Faculty 1,  Germany

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author  | Change Description
   0.0     | 21.03. 2011 | Fgb     | First Version, Linux
   0.1     | 01.11. 2011 | Fgb     | Migration to Atmel uC
   0.2     | 17.09. 2013 | Fgb     | nur noch Atmel, deutsche Doku.
   0.3     | 11.11. 2014 | Fgb     | Portierung auf Arduino
   0.4     | 11.11. 2016 | Fgb     | port to openCM (ARM Cortex-M3)
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

#ifndef cos_scheduler_oop_h_
#define cos_scheduler_oop_h_

extern "C"{
    #include "utility/cos_configure.h"
}

/****** platform specific includes ******************************************/
#if COS_PLATFORM == PLATFORM_ARDUINO
    #include "Arduino.h"
#endif

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    #include "Pandora.h"
    #include "wirish.h"
    #include "Arduino-compatibles.h"
#endif
/****** end of: platform specific includes **********************************/


extern "C" {
/* includes for the C-version of COS */
#include "utility/cos_ser.h"
#include "utility/cos_scheduler.h"
#include "utility/cos_semaphore.h"
#include "utility/cos_data_fifo.h"

void CosVersionInfo(void);

// wrapper functions for Arduino and openCM
int8_t CosInitTaskList(void);
CosTask_t* CosCreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *));
int8_t CosDeleteTask(CosTask_t* task_pt);
int8_t CosSuspendTask(CosTask_t* task_pt);
int8_t CosResumeTask(CosTask_t* task_pt);
int8_t CosSetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio);
int8_t CosRunScheduler(void);
void   CosPrintTaskList(void);
int8_t CosGetCPULoadInPercent(void);


} // extern "C"



#endif  // belongs to #ifndef at top of file...





