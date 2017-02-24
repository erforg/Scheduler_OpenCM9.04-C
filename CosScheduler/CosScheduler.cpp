/*!
 ********************************************************************
   @file            CosScheduler.cpp
   @par Project   : co-operative Scheduler fuer Arduino
   @par Module    : COS-Scheduler

   @brief  openCM / Arduino version of the 'co-operative', stackless tasking system (COS)


   A task-switch ist performed by means of deticated macros.
   These macros have to be called by the task-function directly.
   They must not be used in nested function calls.


   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1


 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author    | Change Description
   0.0     | 02.12. 2010 | Fgb       | First Version, Linux
   0.1     | 07.03. 2011 | Fgb       | added some documentation
   0.2     | 01.08. 2013 | Fgb       | nur noch Atmel, deutsche Doku
   0.3     | 30.10. 2014 | Fgb       | Portierung auf C++, Arduino
   0.4     | 10.11. 2014 | Fgb       | Basisklasse fuer Task, virtuelle Task-Funktion
   0.5     | 15.06. 2015 | Fgb       | Protierung auf C++, Arduino
   0.6     | 31.05. 2016 | Fgb       | Bugfix: Sleep-Zeit im Scheduler auf 0
                                     | zureuckgesetzt, siehe dort
   0.7     | 11.11.2016  | Fgb       | port to openCM (ARM Cortex-M3)
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
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.*****************************************************************************/



/*! \page PlatformPageLabel COS on platform openCM / Arduino
  \tableofcontents

  \section PlatformSecLabel COS on platform openCM / Arduino
  This is COS for platforms openCM and Arduino. On these platforms, no
  further hardware resources as timers or interrupts are required.
  COS uses millis() as system clock. See documentation of file
  CosScheduler.cpp for an overview of the functionality COS provides.

  \section InstallationSecLabel  Installation
  Please extract the files of the archive and copy them to your 'libraries'
  directory. After that, the 'libraries' directory should have this structure:
	@verbatim
	libraries/
	    CosScheduler/
			examples/
			html/
			utility/
	@endverbatim
  Restart the IDE.
  To view the documentation of COS, go to 'html' sub-directory and open
  file 'index.html' in your browser.

  \section CodeExampleSecLable  Code examples
  Code examples an short whitepaper on COS may be found in sub-directory
  'exmples'. Please copy one of the examples into a new sketch, compile
  and run. A description of the examples is given in the whitepaper.

*/




#include "CosScheduler.h"


extern "C" {

/*!
 ********************************************************************
  @par Description
       Prints some version info to the serial monitor on Arduino and
       openCM.

  @par Code example:

  @verbatim
  @endverbatim
 ********************************************************************/
void CosVersionInfo(void)
{
#if COS_PLATFORM == PLATFORM_ARDUINO
     Serial.println("\r\nCOS for Arduino V0.0\r\n");
#endif

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
     SerialUSB.println("\r\nCos for openCM9.04 V0.0\r\n");
#endif
}




/*---------------------------------------------------*/
// wrapper functions for Arduino and openCM
/*---------------------------------------------------*/
/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_InitTaskList(), see there for details

  @see
  @arg  COS_InitTaskList()

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim
void setup()
{
    ...
    if(0!=CosInitTaskList())
    {   SerialUSB.println("error in COS_InitTaskList!\r\n");
    }
    ...
}

void loop()
{
    if(0!=CosRunScheduler())
    {   SerialUSB.print("error in COS_RunScheduler!!\r\n");
    }
}
  @endverbatim
 ********************************************************************/
int8_t CosInitTaskList(void)
{
    return COS_InitTaskList();
}


/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_CreateTask(), see there for details

  @see
  @arg  COS_CreateTask()

  @param  prio     - IN, task-priority. 1 is minimum, 254 is maximum,
                      0 and 255 are reserved.
  @param  pData    - IN, pointer to user-data struct, local task data
  @param  func       IN, name of the task-function

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim
CosTask_t *tA;
...
void myTask2(CosTask_t *pt)
{   static int16_t count =0;
    COS_TASK_BEGIN(pt);

    SerialUSB.print("T2 started\r\n");
    while(count < 10)
    {    SerialUSB.print("T2 cnt=");
         SerialUSB.print(count++);
         SerialUSB.print("\r\n");
         COS_TASK_SLEEP(pt,_milliSecToTicks(500));
    }
    SerialUSB.print("T2 end\r\n");
    COS_TASK_END(pt);
}
...
void setup()
{
    ...
    if(0!=CosInitTaskList())
    {   SerialUSB.println("error in COS_InitTaskList!\r\n");
    }
    tA = CosCreateTask(2, NULL, myTask2);
    ...
}

void loop()
{
    if(0!=CosRunScheduler())
    {   SerialUSB.print("error in COS_RunScheduler!!\r\n");
    }
}
  @endverbatim
 ********************************************************************/
 CosTask_t* CosCreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *))
{
    return COS_CreateTask(prio, pData, func);
}


/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_DeleteTask(), see there for details

  @see
  @arg  COS_DeleteTask()

  @param  task_pt  - IN, pointer to task, that is to be deleted

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim

  @endverbatim
 ********************************************************************/
int8_t CosDeleteTask(CosTask_t* task_pt)
{
    return COS_DeleteTask(task_pt);
}


/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_SuspendTask(), see there for details.
       Suspends a task. The task will not be deleted, but it will not
       be scheduled any more, until CosResumeTask() is called for it.

  @see
  @arg  COS_SuspendTask(), CosResumeTask()

  @param  task_pt  - IN, pointer to task, that is to be suspended

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim

  @endverbatim
 ********************************************************************/
 int8_t CosSuspendTask(CosTask_t* task_pt)
{
    return COS_SuspendTask(task_pt);
}



/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_ResumeTask(), see there for details.
       Resumes a task, that was suspended beforehand. The resumed task
       will be scheduled again.

  @see
  @arg  COS_ResumeTask(), CosSuspendTask()

  @param  task_pt  - IN, pointer to task, that is to be resumed

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim

  @endverbatim
 ********************************************************************/
int8_t CosResumeTask(CosTask_t* task_pt)
{
    return COS_ResumeTask(task_pt);
}



/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_SetTaskPrio(), see there for details.
       Resumes a task, that was suspended beforehand. The resumed task
       will be scheduled again.

       Setzt die Prioritaet einer Task auf einen neuen Wert, 1 ist
       minimal, 254 maximal, 0 und 255 sind reserviert.

  @see
  @arg  COS_SetTaskPrio()

  @param  task_pt  - IN, pointer to task
  @param  taskPrio - IN, new priority. 1 is minimum, 254 is maximum, 0 and 255 reserved

  @retval 0 for ok, negative on error

  @par Code example:

  @verbatim

  @endverbatim
 ********************************************************************/
int8_t CosSetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio)
{
    return COS_SetTaskPrio(task_pt, taskPrio);
}


/*!
 ********************************************************************
  @par Description
       Wrapper for function COS_GetCPULoadInPercent(), see there for details.

  @see
  @arg  COS_GetCPULoadInPercent()

  @retval estimated CPU load in percent

  @par Code example:

  @verbatim

  @endverbatim
 ********************************************************************/
int8_t CosGetCPULoadInPercent(void)
{
    return COS_GetCPULoadInPercent();

}


/*!
 ********************************************************************
  @par Description
       Prints the current task-list to the serial monitor on Arduino and
       openCM.

  @par Code example:

  @verbatim
  @endverbatim
 ********************************************************************/
void CosPrintTaskList(void)
{
    Node_t *pt=COS_GetTaskListRootPointer();

#if COS_PLATFORM == PLATFORM_ARDUINO
        Serial.print("\r\nCOS for Arduino, Task List:");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
        SerialUSB.println("\r\nCos for openCM9.04, Task List:");
#endif

    while(NULL != pt)
    {
#if COS_PLATFORM == PLATFORM_ARDUINO
        Serial.print("\r\ntask:");  Serial.print((uint32_t) pt->task_pt);
        Serial.print("\r\nState:"); Serial.print(pt->task_pt->state);
        Serial.print("\r\nPrio:");  Serial.print(pt->task_pt->prio);
        Serial.print("\r\n");
#endif

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
        SerialUSB.print("\r\ntask:");  SerialUSB.print((uint32_t) pt->task_pt);
        SerialUSB.print("\r\nState:"); SerialUSB.print(pt->task_pt->state);
        SerialUSB.print("\r\nPrio:");  SerialUSB.print(pt->task_pt->prio);
        SerialUSB.print("\r\n");
#endif
        pt = pt->next_pt;
    }
    return;
}






/*!
 ********************************************************************
  @par Description
       Priority base scheduler for Arduino and
       openCM. The task-list is sorted due to priority, scheduler will run
       the first task in the list that is in 'ready' state and has an expired
       'sleep time'. The scheduler run in an endless loop.



  @retval 0, this function runs in an endless loop

  @par Code example:

  @verbatim
  @endverbatim
 ********************************************************************/
int8_t CosRunScheduler(void)
{   // Scheduler version for Arduino and openCM:
    Node_t *pt=NULL;
    uint16_t t_Ticks;

    //DebugCode(_msg("RunScheduler,prio based\r\n"););

    pt = COS_GetTaskListRootPointer(); /* first task, highest prio */
    while(1) /* loop forever */
    {   /* time to run? */
        t_Ticks =millis();
        /* time wrap around is ok, time difference will be right... */
        if(((uint16_t)(t_Ticks - pt->task_pt->lastActivationTime_Ticks) >=
             pt->task_pt->sleepTime_Ticks)&&
            (pt->task_pt->state == TASK_STATE_READY))
        {  pt->task_pt->lastActivationTime_Ticks = t_Ticks;
           /*  when the task function runs to its very end, the task will be deleted:
               it will be removed from the list, and the task struct will be freed,
               i.e. pt->task_pt is no longer valid.
           */
           pt->task_pt->sleepTime_Ticks = 0;  // Bugfix 22.10.2015: sleep time must be specified by task!
           pt->task_pt->func(pt->task_pt);  /* call task function, must not block! */
           pt = COS_GetTaskListRootPointer(); /* next: check task with highest prio */
        }
        else
        {  pt = pt->next_pt;  /* check next task in list */
           if(NULL==pt)
           {  pt = COS_GetTaskListRootPointer();  /* treat linear list as ring list */
           }
        }
    }
    return 0;
}


} // extern "C"
















