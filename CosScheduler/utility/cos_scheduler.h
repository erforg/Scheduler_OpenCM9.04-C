/*!
 ********************************************************************
   @file            cos_scheduler.h
   @par Project   : co-operative scheduler (COS)
   @par Module    : Task-list and scheduler implementation

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
   Version | Date        | Author          | Change Description
   0.0     | 21.03. 2011 | Fgb             | First Version, Linux
   0.1     | 01.11. 2011 | E.Forgber (Fgb) | Migration to Atmel uC
   0.2     | 17.09. 2013 | Fgb             | nur noch Atmel, deutsche Doku.
   0.3     | 08.10. 2015 | Fgb             | change to renesas controller
   0.4     | 19.11. 2016 | Fgb             | change to openCM, english docu

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



#ifndef _cos_scheduler_h_
#define _cos_scheduler_h_

#include "cos_configure.h"
#include "cos_systime.h"
#include "cos_linear_task_list.h"


int8_t COS_InitTaskList(void);
CosTask_t* COS_CreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *));
int8_t COS_DeleteTask(CosTask_t* task_pt);
int8_t COS_SuspendTask(CosTask_t* task_pt);
int8_t COS_ResumeTask(CosTask_t* task_pt);
int8_t COS_SetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio);
int8_t COS_RunScheduler(void);


void   COS_PrintTaskList(void);
int8_t COS_GetCPULoadInPercent(void);

Node_t* COS_GetTaskListRootPointer(void);


/*-------------- macros for task start, end, scheduling ------------*/

/*!
********************************************************************
  @par Description
  This macro has to be inserted at the beginning of every task function.


@par parameter:  pt - IN, pointer to task


@par Code example:
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_BEGIN(pt) switch((pt)->lineCnt) {case 0:


/*!
********************************************************************
  @par Description
  This macro implements a co-operative scheduling point of the task
  function. The macro invokes the scheduler, that may switch to another
  task.

@par parameter: pt - IN, pointer to task


@par Code example:
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_SCHEDULE(pt) (pt)->sleepTime_Ticks=0;\
                          (pt)->lineCnt=__LINE__;\
                          return;\
                          case __LINE__:




/*!
********************************************************************
  @par Description
  This macro implements a co-operative scheduling point of the task
  function. The task will block for a given sleep-time, the scheduler
  will be invoked and another task will be activated.

@par parameter: pt - IN, pointer to task


@par Code example:
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SLEEP(task_pt,100);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_SLEEP(pt,t_Ticks) (pt)->sleepTime_Ticks=(t_Ticks);\
                          (pt)->lineCnt=__LINE__;\
                          return;\
                          case __LINE__:







/*!
********************************************************************
  @par Description
  This macro has to be inserted at the end of every task function.
  IT will terminate the task and delete it from the task list.
  Memory use by the task will be freed. 

@see
@arg  COS_CreateTask(), COS_DeleteTask()

@par parameter: pt - IN, pointer to task


@par Code example:
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_END(pt) }; (pt)->lineCnt=0; COS_DeleteTask((pt)); return;




#endif

