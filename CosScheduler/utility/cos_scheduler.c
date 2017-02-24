/*!
 ********************************************************************
   @file            cos_scheduler.c
   @par Project   : co-operative scheduler (COS) renesas uC
   @par Module    : task-list and scheduling implementation

   @brief  Scheduler of 'co-operative' tasking system (COS)

   A task switch is implemented by dedicated macros. These macros may only
   be invoked directly by the task-function, not from inside nested function
   calls.

   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date       | Author        | Change Description
   0.0     | 21.03.2011 | Fgb           | First Version, Linux
   0.1     | 18.09.2013 | Fgb           | nur noch Atmel, deutsche Doku
   0.2     | 08.10.2015 | Fgb           | umgebaut auf renesas controller
   0.3     | 22.10.2015 | Fgb           | Bugfix: Sleep-Zeit im Scheduler auf 0
                                        | zureuckgesetzt, siehe dort
   0.4     | 19.11.2016 | Fgb           | openCM, english docu 
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




/*! \mainpage Co-Operative Scheduler (COS)




  
  \section Platforms

This is the implementation of a simple stackless, co-operative tasking system COS.
It may be used on several platforms, see \ref PlatformPageLabel.

  \section Introduction
     
A language extension of the gnu C-compiler 'gcc' is used to implement the
co-routine pattern to realize a task (see R. Dunkels: 'protothreads' 
and Michael Dorin: 'Bulding "instant-up" 
real-time operating systems'). The scheduler may run on any system 
providing a gnu gcc-compiler. 

A 'co-routine' has several exit points and will resume where it 
exited before.  The implementation uses macros, building up a 
'switch'-statement with line numbers a 'case' marks  and 'return'
statements at co-operative scheduling points. When reaching a scheduling
point, the task function executes a 'return' statement and will forget
all its local variables, except of those labelled 'static'. The 
scheduling macros will ensure, that before returning, the task will store
its current code line number in the task-struct. The next time, the 
scheduler calls the task-function, execution will resume at that stored 
line of code, implemented as a 'case' mark in the 'switch' statement. 

The scheduling macros have to be called by the task-function directly.
They must not be used in nested function calls.

A task-function is a callback-function. It either runs to its very end 
and will then be deleted from the task-list, or it reaches a scheduling
point (see macros below) and will execute a co-routine return storing 
the current code line number for the next activation. 

A system clock is mandatory, in order to start the task callback-functions
on time. The function _gettime_Ticks(void) has to be implemented on the
platform, COS is running on. Time is measured in 'ticks'. The system time
in Ticks will be returned by _gettime_Ticks(void). Time measurement may
be implemented by a timer interrupt, or if available, it may be provided 
by an operating system. 

The scheduler will call the task callback-functions with respect to 
task-state, task-priority and time of last activation.

Among the tasks in state 'TASK_STATE_READY', the one with highest 
priority will be activated, if it is time to run, i.e. if: 
timeNow-timeLastActivation > sleepTime_Ticks

The scheduler uses time differences, therefore timer wrap-around will 
not cause problems, time differences will be correctly computed, as long
as the time interval is shorter as a complete turn-around of the tick
counter.

Before the task-function is called, the scheduler will set the 
sleep-time (sleepTime_Ticks) of the task to 0. The task-function has to 
specify its sleep-time each time it is activated, see examples below.

The task-list is never empty, there is at least the idle-task with 
its callback-function static void idleTask(CosTask_t *task_pt);

All task callback-functions return nothing an have a single parameter 
CosTask_t*.

The scheduler runs in an endless loop. The task-list will never be
deleted and there is no end of program.

Before running the scheduler, the function int COS_InitTaskList(void);
has to be called. It will initialize the task-list and register the 
idle-task.

The scheduler may run in one of two modes: priority based of round-robin
scheduling.

Priority based scheduling requires the task-list to be sorted due to
priority. In this mode, a coarse estimation of CPU-load may be achieved
by means of two tasks: The cpu-load-task has maximum priority and will
reset a counter to 100. The idle-task with lowest priority decrements
that counter. The task periods have to be adjusted to run the 
idle-task 100 times, while the cpu-load-task runs once, provided there are
no other tasks running. The counter value is an estimate of
cpu-load: With growing cpu-load, the idle-task may be suppressed  and will
not be able to decrement the counter to 0 any more. At maximum load,
idle will be totally suppressed by other tasks and the counter value 
will remain 100, meaning 100% cpu-load. 

In round-robin mode, the cpu-load estimation won't work, but will do no
harm either.

  @verbatim
  list of tasks:
                    node
    root          --------      --------           --------
    -----         |      |----->|      |-- ... --->|      |----> NULL
    |   |-------->|      |      |      |           |      |
    -----         --------      --------           --------
                     |             |                  |
                     \/            \/                 \/
                   -----         -----              -----
                   |   |         |   |              |   |
                   -----         -----              -----
                   task
  @endverbatim



 **********************************************************************/
#include "cos_scheduler.h"
#include <stdlib.h>
#include "cos_ser.h"



/*! 0 for round-robin tasking, 1 for prio-based scheduler */
#define PRIO_BASED_SCHEDULING 1

/*! idle task period, don't edit this */
#define IDLE_TASK_PERIOD_TICKS      _milliSecToTicks(10)
/*! idle task priority, don't edit this */
#define IDLE_TASK_PRIO              0   
/*! cpu load estimation task period, don't edit this */
#define LOAD_MEASURE_TASK_PERIOD_TICKS (100*IDLE_TASK_PERIOD_TICKS)
/*! cpu load estimation task priority, don't edit this */
#define LOAD_MEASURE_TASK_PRIO      255




/*------------- DEBUGGING ---------------------------------*/
#define DEBUG_MODULE 0  /*!< 0 for no additional debug code, 1 otherwise */ 

#if DEBUG_MODULE
  /*!< for debugging only: insert some code */
  #define DebugCode( code_fragment ) { code_fragment }
#else
  #define DebugCode( code_fragment )   /*!< for debugging only: insert some code */ 
#endif

#if DEBUG_MODULE
static void _msg(char *msg)
{
    DebugCode(serPuts(msg););
}

static void _toggle_PD(uint8_t bit)
{
    static uint8_t i=0;

    if(i)
    {   //set_bit(PORTD, bit);
    }
    else
    {   //clear_bit(PORTD, bit);
    }
    i=!i;
}
#endif
/*---------------------------------------------------------------*/






/* private types */


/****************************************************************/
/* private module variables */
/****************************************************************/
static Node_t *root_g=NULL;           /*! root pointer of task-list */
static uint8_t cpuLoadPerCent_g=100;  /*! for CPU-load estimation */
static uint8_t cpuLoadCounter_g=100;  /*! for CPU-load estimation  */
/****************************************************************/

/****************************************************************/
/* private function prototypes */
/****************************************************************/

static void _idleTask(CosTask_t *pt);
static void _cpuLoadMeasureTask(CosTask_t *pt);
static CosTask_t *_idleTask_pt_g = NULL;
static CosTask_t *_cpuLoadMeasureTask_pt_g = NULL;


/****************************************************************/




/****************************************************************/
/* private functions */
/****************************************************************/


/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Description
       The task-list contains at least the idle-task, wich has lowest
       priority. The idle-task decrements a counter for cpu-load
       estimation. Without any additional load, it runs 100 times
       before the cpu-load-task can reset the counter to 100. 

  @see
  @arg _cpuLoadMeasureTask()

  @param  pt - IN, pointer to task
 ********************************************************************/
static void _idleTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    while(1)
    {     if(cpuLoadCounter_g > 0)
          {   cpuLoadCounter_g--;
          }
          //DebugCode(_toggle_PD(5););
          COS_TASK_SLEEP(pt,IDLE_TASK_PERIOD_TICKS);
    }
    COS_TASK_END(pt);
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Description
       This task is optional. It has the maximum priority and resets 
       a counter to 100. The counter is decremented by the idle-task.
       If cpu-load is low, idle may be able to decrement the counter
       to 0. If cpu-load is 100%, idle will not be activated any
       more, since the are always tasks with higher priority ready to
       run. Therefore the counter value will remain 100, indicating 
       100% cpu-load.

  @see  _idleTask(), COS_GetCPULoadInPercent()
  @arg

  @param  keine

  @retval keine
 ********************************************************************/
static void _cpuLoadMeasureTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    while(1)
    {     cpuLoadPerCent_g = cpuLoadCounter_g;  // remains constant for the period
          cpuLoadCounter_g = 100;
          COS_TASK_SLEEP(pt,LOAD_MEASURE_TASK_PERIOD_TICKS);
    }
    COS_TASK_END(pt);
}
/*---------------------------------------------------------------*/





/****************************************************************/
/* exported module functions */
/****************************************************************/


/*!
 ********************************************************************
  @par Description
       Initializes the task-list. Tasks are organized in a linear list,
       containing at least the idle-task. The list is sorted due to
       task priority. The task with highest priority is first in the 
       list.


  @retval 0 for ok, negative on error
  @par Code example:

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    while(1);
}
  @endverbatim
 ********************************************************************/
int8_t COS_InitTaskList(void)
{
    //DebugCode(_msg("InitTaskList\r\n"););
    root_g = NULL;  /* empty task list */
    /* task functions are kept in a linear list, that always has at least
       one element: the idle task.
    */

    _idleTask_pt_g = COS_CreateTask(IDLE_TASK_PRIO, NULL, _idleTask);

    /* add optional cpu load estimation task to list */
    _cpuLoadMeasureTask_pt_g = COS_CreateTask(LOAD_MEASURE_TASK_PRIO, NULL, _cpuLoadMeasureTask);

    _sortLinearListPrio(root_g);
    return 0;
}


/*---------------------------------------------------------------*/

/*!
 ********************************************************************
  @par Description
       Allocates and initializes a task-structure and an new node in
       the task-list. The function appends the node to the list and 
       sorts the list due to task priority.

  @see 
  @arg COS_DeleteTask(), COS_SuspendTask(), COS_ResumeTask(), COS_SetTaskPrio(),

  @param  prio     - IN, task priority. 1 is minimum, 254 ist maximum, 
                      0 and 255 reserved
                 
  @param  pData    - IN, pointer to user data struct, may store local task data
  @param  func       IN, name of task-function

  @retval pointer to task struct or NULL on error

  @par Code example:
  @verbatim
#include <stdio.h>
#include "avr_cos_scheduler.h"

void COS_Task_TogglePD6(CosTask_t* task_pt)
{   static uint8_t on=0;

    COS_TASK_BEGIN(task_pt);

    while(1)
    {
        if(!on)
        {   set_bit(PORTD,6);
            on=1;
        }
        else
        {   clear_bit(PORTD,6);
            on=0;
        }
        COS_TASK_SLEEP(task_pt,100);
    }
    COS_TASK_END(task_pt);
}

CosTask_t *t_TogglePD6;

int main(void)
{   _initPortDirections();
    _initPorts();
    _initSystemTime();
    sei();                  // alle INT freigeben

    serInit(CPU_CLOCK, BAUD);
    _startupMessageRS232();

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    // Task erzeugen
    t_TogglePD6 = COS_CreateTask(5, NULL, COS_Task_TogglePD6);
    COS_PrintTaskList();
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    while(1);    // this should not be reached...
    return 0;
}

  @endverbatim
 ********************************************************************/
CosTask_t* COS_CreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *))
{
    CosTask_t *t_pt= NULL;

    //DebugCode(_msg("CreateTask\r\n"););

    /* allocate and init task struct */
    t_pt = _newTask(prio, pData, func);
    if(t_pt==NULL)
    {   DebugCode(_msg("CreateTask:_newTask!\r\n"););
        return NULL;
    }

    root_g = _addTaskAtBeginningOfTaskList(root_g, t_pt);
    _sortLinearListPrio(root_g);

    return t_pt;  /* pointer to task struct */
}
/*---------------------------------------------------------------*/






/*!
 ********************************************************************
  @par Description
       Deletes a task from the task-list and frees corresponding 
       memory. The task-list stores at least one task, the idle-task
       must not be deleted.

  @see 
  @arg COS_CreateTask()

  @param  task_pt -     IN, pointer to task

  @retval 0 for ok, negative on error
 ********************************************************************/
int8_t COS_DeleteTask(CosTask_t* task_pt)
{
    /* remove from list, i.e. free the corresponding node,
       task struct will not be freed here */
    root_g = _unlinkTaskFromTaskList(root_g, task_pt);

    /* free memory of task struct */
    free(task_pt);
    return 0;
}



/*!
 ********************************************************************
  @par Description
       Sets the task state to  TASK_STATE_SUSPENDED.
       The task will not be erased and may be re-activated by 
       COS_ResumeTask().


  @see COS_ResumeTask()
  @arg

  @param  task_pt -     IN, pointer to task

  @retval 0 for ok, negative on error
 ********************************************************************/
/*---------------------------------------------------------------*/
int8_t COS_SuspendTask(CosTask_t* task_pt)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("Suspend:task not found\r\n"););
        return -1;
    }
    pt->task_pt->state = TASK_STATE_SUSPENDED;
    return 0;
}
/*---------------------------------------------------------------*/



/*!
 ********************************************************************
  @par Description
       Sets the task state to   TASK_STATE_READY.
       The task will again take part in the scheduling.

  @see 
  @arg COS_SuspendTask()

  @param  task_pt -     IN, pointer to task

  @retval 0 for ok, negative on error
 ********************************************************************/
int8_t COS_ResumeTask(CosTask_t* task_pt)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("Resume:task not found\r\n"););
        return -1;
    }
    pt->task_pt->state = TASK_STATE_READY;
    return 0;
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Description
       Sets the priority of a task. Priority 1 is minimum, 254 is
       maximuml, 0 and 255 are reserved.


  @param  task_pt -     IN, pointer to task
  @param  taskPrio -  IN, new priority

@retval 0 for ok, negative on error
********************************************************************/
int8_t COS_SetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("SetTaskPrio:task not found\r\n"););
        return -1;
    }
    pt->task_pt->prio = taskPrio;
    _sortLinearListPrio(root_g);
    return 0;
}
/*---------------------------------------------------------------*/







/**********************************************************
 * Bugfix 22.10.2015: Beim Sleep wird eine Wartezeit gesetzt, die nur durch
 * ein neues COS_TASK_SLEEP() oder COS_TASK_SCHEDULE() geaendert wurde.
 * Vor jedem Neustart der einer Task wird jetzt die Sleep-Zeit auf 0
 * zurueckgesetzt. Die Task muss ihre Sleep-Zeit bei jedem Lauf neu setzen mit
 * COS_TASK_SLEEP(), COS_TASK_SCHEDULE() (setzt die Sleep-Time auf 0). Semaphoren
 * aendern die Sleep-Time nicht, sondern aendern den Task-State.
 *
 */


#if PRIO_BASED_SCHEDULING
/*!
 ********************************************************************
  @par Description
       This is the priority based scheduler. The task-list ist sorted
       due to priority, the scheduler will run the first task in the 
       list, whos state is TASK_STATE_READY and with: 
       timeNow-timeLastActivation > sleepTime_Ticks.
       The scheduler runs in an endless loop.


  @retval 0 for ok, negative on error (although it should never return...)
  @par Code example

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    getchar();
    return 0;
}
  @endverbatim
 ********************************************************************/
int8_t COS_RunScheduler(void)
{

#if COS_PLATFORM == PLATFORM_RENESAS_RX63N

    Node_t *pt=NULL;
    uint16_t t_Ticks;

    //DebugCode(_msg("RunScheduler,prio based\r\n"););

    pt = root_g; /* first task, highest prio */
    while(1) /* loop forever */
    {   /* time to run? */
        t_Ticks = _gettime_Ticks();
        /* time wrap around is ok, time difference will be right... */
        if(((uint16_t)(t_Ticks - pt->task_pt->lastActivationTime_Ticks) >=
             pt->task_pt->sleepTime_Ticks)&&
            (pt->task_pt->state == TASK_STATE_READY))
        {  pt->task_pt->lastActivationTime_Ticks = t_Ticks;
           /*  when the task function runs to its very end, the task will be deleted:
               it will be removed from the list, and the task struct will be freed,
               i.e. pt->task_pt is no longer valid.
           */
           pt->task_pt->sleepTime_Ticks = 0;  // Bugfix 22.10.2015: must be specified by task!
           pt->task_pt->func(pt->task_pt);  /* call task function, must not block! */
           pt = root_g; /* next: check task with highest prio */
        }
        else
        {  pt = pt->next_pt;  /* check next task in list */
           if(NULL==pt)
           {  pt = root_g;  /* treat linear list as ring list */
           }
        }
    }
#endif // COS_PLATFORM

    // not implemented on Arduino and openCM, use CosRunScheduler() instead

    return 0;

}
#else
/*!
 ********************************************************************
  @par Description
       Round-robin version of the scheduler. The task-list is sorted 
       due to priority, but this version ignores priority.
       One after another, every task is scheduled, that is in state 
       TASK_STATE_READY and has: 
       timeNow-timeLastActivation > sleepTime_Ticks.
       The scheduler runs in an endless loop.


  @retval 0 for ok, negative on error (although it should never return...)
  @par Code eample:

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    getchar();
    return 0;
}
  @endverbatim
 ********************************************************************/
int8_t COS_RunScheduler(void)
{

#if COS_PLATFORM == PLATFORM_RENESAS_RX63N

    Node_t *pt=NULL;
    uint16_t t_Ticks;

    //DebugCode(_msg("RunScheduler, round robin\r\n"););

    pt = root_g; /* first task */
    while(1) /* run forever */
    {   /* time to run? */
        #if COS_PLATFORM == PLATFORM_RENESAS_RX63N
            t_Ticks = _gettime_Ticks();
        #endif // COS_PLATFORM
        #if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
            t_Ticks = millis();
        #endif // COS_PLATFORM
        if(((uint16_t)(t_Ticks - pt->task_pt->lastActivationTime_Ticks) >=
             pt->task_pt->sleepTime_Ticks)&&
            (pt->task_pt->state == TASK_STATE_READY))
        {  pt->task_pt->lastActivationTime_Ticks = t_Ticks;
           /*  when the task function runs to its very end, the task will be deleted:
               it will be removed from the list, and the task struct will be freed,
               i.e. pt->task_pt is no longer valid.
           */
           pt->task_pt->sleepTime_Ticks = 0;  // Bugfix 22.10.2015: must be specified by task!
           pt->task_pt->func(pt->task_pt);  /* call task function, must not block! */
        }
        pt = pt->next_pt;  /* next task in list */
        if(NULL==pt)
        {   pt = root_g;  /* use task list as ring list */
        }
    }
#endif // COS_PLATFORM

    return 0;
}
#endif
/*---------------------------------------------------------------*/



/*!
 ********************************************************************
  @par Description
       Use this function to print the current task list.

 ********************************************************************/
void COS_PrintTaskList(void)
{
#if COS_PLATFORM == PLATFORM_RENESAS_RX63N
    Node_t *pt=root_g;

    while(NULL != pt)
    {   serPuts("\r\ntask:");  serOutUint32Hex((uint32_t) pt->task_pt);
        serPuts("\r\nState:"); serOutUint8Hex(pt->task_pt->state);
        serPuts("\r\nPrio:");  serOutUint8Hex(pt->task_pt->prio);
        pt = pt->next_pt;
    }
    return;
#endif // COS_PLATFORM
    // not implemented on Arduino and openCM, use CosPrintTaskList() instead
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Description
       A simple cpu-load estimation uses two tasks, that are created
       by COS_InitTaskList(). The task _cpuLoadMeasureTask()
       has maximum priority and sets a counter to 100. The counter is
       decremented by the idle-task. The counter value may indicate 
       the cpu-load. 
       
  @retval cpu-load in percent
 ********************************************************************/
int8_t COS_GetCPULoadInPercent(void)
{   return cpuLoadPerCent_g;
}
/*---------------------------------------------------------------*/






/*!
 ********************************************************************
  @par Description
       This function returns the root ponter to the task-list. It is
       for internal use in Arduino and openCM wrapper functions only.
       
       
  @retval root pointer to internal task-list
 ********************************************************************/
Node_t* COS_GetTaskListRootPointer(void)
{
    return root_g;
}
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/

/****************************************************************/




