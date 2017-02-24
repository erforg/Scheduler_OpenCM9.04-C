/*!
 ********************************************************************
   @file            cos_semaphore.c
   @par Project   : co-operative Scheduler (COS)
   @par Module    : Counting semaphore for co-operative scheduler (COS)

   @brief  For a brief description of the principle of operation, see file cos_semaphore.h



   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 29.04. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 08.10. 2015 | Fgb           | renesas controller
   @endverbatim



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




#include "cos_semaphore.h"



/*!
********************************************************************
  @par Description
  Initializes a semaphore to a given counter value and creates an empty list of waiting tasks.

@see
@arg

@param s       - IN/OUT, pointer to semaphore
@param n_start - IN, initial counter value

@retval 0 fuer ok, negative on error.

@par code example
@verbatim
CosSema_t sema_1;
...
int main(void)
{   ...
    serPuts("hello");

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();

    COS_SemCreate(&sema_1, 0);


    tA = COS_CreateTask(1, NULL, myTask);
    tB = COS_CreateTask(2, NULL, myTask2);
    tC = COS_CreateTask(3, NULL, myTask3);

    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }

    getchar();
    return 0;
}
@endverbatim
********************************************************************/
uint8_t COS_SemCreate(CosSema_t *s, int8_t n_start)
{
    s->count = n_start;
    s->root_pt = NULL;
    return 0;
}







/*!
********************************************************************
  @par Description
  Deletes the list of waiting tasks at this semaphore. Tasks will not be
  deleted, of course.

@see
@arg

@param s       - IN/OUT, pointer to semaphore

@retval 0 for ok, negative on error

@par Code example
@verbatim
CosSema_t sema_1;
...
int main(void)
{   ...
    serPuts("hello");

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_SemCreate(&sema_1, 0);
    ...
    COS_SemDestroy(&sema_1);

    getchar();
    return 0;
}


@endverbatim
********************************************************************/
uint8_t COS_SemDestroy(CosSema_t *s)
{
    /* free memory of all list elements, but don't destroy the tasks! */
    Node_t *pt = s->root_pt;

    while(s->root_pt != NULL)
    {   pt = s->root_pt;                    // node to be freed
        s->root_pt = s->root_pt->next_pt;  // next node in list
        free(pt);
    }
    return 0;
}




/*!
********************************************************************
  @par Description
  Increments the semaphore counter.
  The list of waiting tasks is examined and the first task in the list is set
  to state TASK_STATE_READY and is deleted from the list. In order to
  immediately activate that task due to its priority, insert a call to macro
  COS_TASK_SCHEDULE() after the call to COS_SEM_SIGNAL(). Otherwise, a task with
  high priority will be activated a little later, as soon as another scheduling
  macro is executed.

@see
@arg    COS_SEM_WAIT()

@param s       - IN/OUT, pointer to semaphore


@par Code example:
@verbatim
CosSema_t sema_1;
...
void myTask2(CosTask_t *pt)
{   static count =0;
    COS_TASK_BEGIN(pt);

    serPuts("myTask2: started...\n");
    while(count < 100)
    {    serPuts("Task2\n");
         count++;
         COS_TASK_SLEEP(pt,100);
    }

    COS_SEM_SIGNAL(&sema_1);
    COS_TASK_SCHEDULE(pt); // give high priority tasks a chance

    serPuts("myTask: terminates...\n");

    COS_TASK_END(pt);
}
@endverbatim
********************************************************************/
void COS_SEM_SIGNAL(CosSema_t *s)
{
  CosTask_t *task_pt=NULL;  /*!<  pointer to task structure */

  (s->count)++;
  if(s->root_pt != NULL)  // any task waiting on this sema?
  { task_pt = s->root_pt->task_pt;  // first waiting task
    task_pt->state = TASK_STATE_READY;  // make it ready to run
    s->root_pt = _unlinkTaskFromTaskList(s->root_pt, task_pt); // remove it from sema-list
  }

}
