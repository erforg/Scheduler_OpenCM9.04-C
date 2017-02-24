/*!
 ********************************************************************
   @file            cos_linear_task_list.h
   @par Project   : co-operative scheduler 
   @par Module    : linear task list implementation

   @brief  linear list management for COS


   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 21.03. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 09.10.2015  | Fgb           | umgestiegen auf renesas controller
   0.3     | 20.11.2016  | Fgb           | english docu
   @endverbatim

   Routines for linear list management

  @verbatim

                          node
           root          --------      --------           --------
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
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*****************************************************************************/



#ifndef linear_task_list_h_
#define linear_task_list_h_

#include "cos_configure.h"
#include "cos_systime.h"
#include <stdlib.h>


#define TASK_STATE_READY         0  /*!< internal task state */
#define TASK_STATE_SUSPENDED     1  /*!< internal task state */
#define TASK_STATE_BLOCKED       2  /*!< internal task state */



/*!
 ********************************************************************
  @par Description
  Task structure of the co-operative scheduler COS. The scheduler starts
  * the task callback-function 'func', updates the value of 
  * 'lastActivationTime_Ticks' to current time in ticks and sets
  * 'nextActivationTime_Ticks = lastActivationTime_Ticks', thus causing
  * the scheduler to activate the task as soon as possible.
  * In order to be activated after a sleep time, the task has to 
  * invoke the macro COS_TASK_SLEEP(), which will set the new activation
  * time to 
  * 'nextActivationTime_Ticks = lastActivationTime_Ticks + sleepTime_Ticks'.
  * 
   

@par Task state
* A task in state 'ready' is ready to run. The task may be set to state
* 'suspended' by a dedicated command or it may be set to state 'blocked'
* by a semaphore.


@par Re-entry point
* The scheduler calls the task callback-function, that will continue 
* its work at code line 'lineCnt' by means of dedicated macros.

@par User data: private data of a task
* A task may store local data in 'static' variables, that will keep
* their values to the next function call. Using this simple way, only
* one entity of the task may be exist, because local variables are
* ascociated to the task callback-function, not to the task structure
* variable.
* If several versions of same task shall run at the same time, the 
* local data have to be attached to the task structure variable not to
* the task callback-variable.
* The pointer 'pData'may be used to point to a user data struct, storing 
* data private to the individual task.
* 




  @see
 ********************************************************************/
typedef struct CosTask_t CosTask_t;
struct CosTask_t                       /*! task struct */
{   uint16_t lastActivationTime_Ticks; /*!< last activation time in ticks */
    uint16_t sleepTime_Ticks;          /*!< will cause the task to block.
                                            0 == sleepTime_Ticks means:
                                            start asap */
    uint8_t  state;     /*!< task states:  TASK_STATE_READY,
                             TASK_STATE_SUSPENDED, TASK_STATE_BLOCKED */
    uint8_t  prio;      /*!< priority, 1 ist minimum, 254 ist maximum. 0 and 255 reserved */
    uint16_t lineCnt;   /*!< stores code line number for re-entry */
    void * pData;       /*!< pointer to user-data, opportunity to store 
                             locale task-variables */
    void (*func)(CosTask_t*); /*!< name of the task callback-function */
};


/*!
 ********************************************************************
  @par Description
  Node struct to build up the task list.
********************************************************************/  
typedef struct Node_t Node_t;  
struct Node_t            /*! Node struct to build up the task list */
{          
    CosTask_t *task_pt;  /*!<  Pointer to task struct */
    Node_t *next_pt;     /*!<  Pointer to next node in list */
};



Node_t *_addTaskAtBeginningOfTaskList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_unlinkTaskFromTaskList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_searchTaskInList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_searchPredecessorTaskInList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_newNode(CosTask_t *task_pt);
void _sortLinearListPrio(Node_t *root_pt);
CosTask_t *_newTask(uint8_t prio, void * pData, void (*func) (CosTask_t *));



#endif

