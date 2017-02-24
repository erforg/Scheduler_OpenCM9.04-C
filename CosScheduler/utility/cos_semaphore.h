/*!
 ********************************************************************
   @file            cos_semaphore.h
   @par Project   : co-operative Scheduler
   @par Module    : Counting semaphore for co-operative Scheduler (COS)

   @brief  Counting semaphore for co-operative Scheduler (COS)



   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1

   @details How does a semaphore work in COS?
      Only counting semaphores are implemented. A semaphore consists of a list
      of task, waiting in blocked state at this semaphore.
      The macro COS_SEM_WAIT(s,pt) tries to decrement the semaphore counter.
      If the counter is >0, it will be decremented and the task will continue
      with the next instruction after the macro.
      If the counter is <=0, the task that called COS_SEM_WAIT(s,pt) will block.
      The co-operative scheduling scheme will set the tasks state to
      TASK_STATE_BLOCKED. The task will be added to the list of waiting tasks
      at the semaphore. The semaphore counter is decremented and might even
      become negative. The negative counter value shows how many tasks are
      waiting at the semaphore. As long as the state of a task is TASK_STATE_BLOCKED,
      it will not be activated by the scheduler.
      Only after the state of a task has changed to TASK_STATE_READY, it may be
      activated and it will continue with the next instruction after the macro.
      The state change to TASK_STATE_READY is caused by the function
      COS_SEM_SIGNAL(CosSema_t *s). This function will increment the semaphore
      counter. If there is a non-empty waiting list, the first task in the list
      will be set to state TASK_STATE_READY and it will be erased from the list.

  @verbatim

                          node
           root_pt       --------      --------           --------
           -----         |      |----->|      |-- ... --->|      |----> NULL
           |   |-------->|......|      |......|           |......|
           -----         |      |      |      |           |      |
                         --------      --------           --------
                            |             |                  |
                            \/            \/                 \/
                          -----         -----              -----
                          |   |         |   |              |   |
                          -----         -----              -----
                          task

  @endverbatim




 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 29.04. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | ported to Atmel AVR, deutsche Doku.
   0.2     | 08.10. 2015 | Fgb           | switch to renesas controller
   0.3     | 22.10.2015  | Fgb           | Bugfix in COS_SEM_WAIT()

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


#ifndef _cos_semaphore_h_
#define _cos_semaphore_h_

#include "cos_configure.h"
#include "cos_scheduler.h"
#include "cos_linear_task_list.h"


/***********************************************
 * semaphore data structure :
 ***********************************************/
/*! counting semaphore data structure */
typedef struct {
        int8_t count;     /*!< semaphore counter, sign is used internally */
        Node_t *root_pt;  /*!< pointer to next node in list of waiting tasks. */
} CosSema_t;              /*!< counting semaphore type */




uint8_t COS_SemCreate(CosSema_t *s, int8_t n_start);
uint8_t COS_SemDestroy(CosSema_t *s);




/*!
********************************************************************
  @par Description
This macro implements the 'wait' operation at a semaphore. The task must not really
stop executing, since that would mean 'deadlock' to the co-operative scheduling scheme.
If COS_SEM_WAIT() causes a task to block, the task function has to return and
hand over program control to the scheduler, immediately. At the next activation,
the task has to continue with the next instruction after COS_SEM_WAIT().
The task's state is switched to TASK_STATE_BLOCKED, the scheduler will no longer
activate a task in this state. The task is added to the list of tasks waiting at
the semaphore.

TODO: Maybe the list should be sorted due to task priority. But even a sorted waiting
list will not solve the problem of 'priority inversion'.



@see
@arg  COS_SemCreate(), COS_SemDestroy()

@param s - IN, Zeiger auf Semaphore struct: CosSema_t
@param pt - IN, Zeiger auf Task struct: CosTask_t

@retval nichts

@par code example:
@verbatim
CosSema_t sema_1;
...
void myTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);

    serPuts("myTask: started...\n");
    serPuts("myTask: step0\n");

    COS_SEM_WAIT(&sema_1,pt);

    serPuts("myTask: step1\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("mserPutsyTask: step2\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: step3\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: step4\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: terminates...\n");

    COS_TASK_END(pt);
}
@endverbatim
********************************************************************/
#define COS_SEM_WAIT(s,pt)  (pt)->lineCnt=__LINE__;\
                            if((s)->count <= 0) {  \
                              (pt)->state = TASK_STATE_BLOCKED; \
                              (s)->root_pt=_addTaskAtBeginningOfTaskList((s)->root_pt,(pt)); \
                            } \
                            ((s)->count)--; \
                            return;\
                            case __LINE__:



void COS_SEM_SIGNAL(CosSema_t *s);



#endif


