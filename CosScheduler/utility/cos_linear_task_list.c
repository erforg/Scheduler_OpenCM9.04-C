/*!
 ********************************************************************
   @file            cos_linear_task_list.c
   @par Project   : 'co-operative' Scheduler COS
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
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku
   0.2     | 08.10. 2015 | Fgb           | umgeschrieben fuer renesas
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



#include <stdlib.h>
//#include <avr/io.h>
//#include <stdint.h>
#include "cos_ser.h"
#include <stdio.h>  // for degugging on serial terminal

#include "cos_linear_task_list.h"



/*------------- DEBUGGING ---------------------------------*/
#define DEBUG_MODULE 0 /*!< 0 for no additional debug code, 1 otherwise */

#if DEBUG_MODULE
    /*!< for debugging only: insert some code */
    #define DebugCode( code_fragment ) { code_fragment }
#else
    #define DebugCode( code_fragment )  /*!< for debugging only: insert some code */
#endif


#if DEBUG_MODULE
    static void _msg(char *msg)
    {
        DebugCode(serPuts(msg););
    }
#endif

/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Description
  Creates a new node, links the task to that node and inserts the node
  * at the start of the task-list. Hence, the root-pointer will change.
  * The new root-pointer value is returned be the fuction. The task
  * struct task_pt is pointing to, has to be initialized before calling
  * this function.

@see _unlinkTaskFromTaskList(), _newNode()
@arg

@param root_pt - IN, pointer to  first node in task-list
@param task_pt - IN, pointer to initialized task struct

@retval pointer to first node in task-list

@par Code example:
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_addTaskAtBeginningOfTaskList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    pt = _newNode(task_pt);    /* init node, connect to existing task */
    pt->next_pt = root_pt;      /* append old list to new node: new first element */
    return pt;                  /* new root pointer: address of first element */
}
/*---------------------------------------------------------------*/



/*!
********************************************************************
  @par Description
  This routine searches the task-list for the node, pointing to the same
  * task as task_pt. The node will be deleted from the list, its memory 
  * is freed. The task struct will not be deleted, it might be part of 
  * other lists (e.g semaphores have a list of waiting task). The 
  * root pointer of the task-list will change, if the node to be deleted
  * is the first in the list. The function returns the (possibly updated) 
  * value of the root pointer to the task-list.


@see _addTaskAtBeginningOfTaskList(), _newNode()
@arg

@param root_pt - IN, pointer to  first node in task-list
@param task_pt - IN, pointer to initialized task struct

@retval pointer to first node in task-list

@par Code example:
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    root_pt = _unlinkTaskFromTaskList(root_pt, pt);
    ...


    return 0;
}
@endverbatim
********************************************************************/
 Node_t *_unlinkTaskFromTaskList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;
    Node_t *predecessor_pt=NULL;

    pt = _searchTaskInList(root_pt, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("_unlink...task not found"););
        return root_pt;  /* don't change the list! */
    }
    /* is it the first node in the list? root_pt has to be changed... */
    if(pt == root_pt)
    {   pt = pt->next_pt; /* second node in list */
        free(root_pt); /* free the first node, don't touch the task! */
        return pt;    /* the old second element now is the first */
    }
    /* node exists, is not the first list element, should have a predecessor... */
    predecessor_pt = _searchPredecessorTaskInList(root_pt, task_pt);
    if(NULL == predecessor_pt)
    {   DebugCode(_msg("_unlink...pred.task not found"););
        return root_pt;  /* don't change the list! */
    }
    /* task node and its predecessor have been found. Unlink task node: */
    predecessor_pt->next_pt = pt->next_pt;
    free(pt); /* free the node, don't touch the task! */
    return root_pt; /* old first list element still is the first */
}

/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Description
  The function gets a pointer to a task struct and searches the task-list
  * for a node pointing to the given task. The function returns a
  * pointer to the corresponding node. If the task is not found in the
  * list, NULL is returned.

@see _unlinkTaskFromTaskList()
@arg

@param root_pt - IN, pointer to  first node in task-list
@param task_pt - IN, pointer to initialized task struct

@retval pointer to node connected to task *task_pt

@par Code example:
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    npt = _searchTaskInList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_searchTaskInList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    pt = root_pt;
    while(pt != NULL)
    {   if((pt->task_pt) == task_pt)
        {   return pt;
        }
        pt = pt->next_pt;
    }
    DebugCode(_msg("Task not found"););
    return NULL;
}
/*---------------------------------------------------------------*/

/*!
********************************************************************
  @par Description
  The function gets a pointer to a task struct. The function searches
  * the task-list for the predecessor node of the task. The function 
  * returns a pointer to the corresponding predessessor node. 
  * If no predecessor is found, NULL is returned.

@see _searchTaskInList(), _unlinkTaskFromTaskList()
@arg

@param root_pt - IN, pointer to  first node in task-list
@param task_pt - IN, pointer to initialized task struct

@retval pointer to the predecessor node of the task

@par Code example:
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    npt = _searchPredecessorTaskInList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_searchPredecessorTaskInList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    /* list empty? */
    if(NULL==root_pt)
    {   DebugCode(_msg("_searchPred...empty list"););
        return NULL;
    }
    /* task connected to first node?  no predecessor...*/
    pt = root_pt;
    if(pt->task_pt == task_pt)
    {   DebugCode(_msg("_searchPred...nothing found"););
        return NULL;
    }
    /* ok, list has at least one node */
    while(pt->next_pt != NULL)
    {   if((pt->next_pt->task_pt) == task_pt) return pt;  /* the predecessor! */
        pt = pt->next_pt;
    }
    DebugCode(_msg("Pred. Task not found"););
    return NULL;
}
/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Description
  The function gets a pointer to a task struct. The function allocates
  * a new node pointing to the task. The pointer to the node is returned.

@see _addTaskAtBeginningOfTaskList()
@arg

@param task_pt - IN, pointer to initialized task struct

@retval pointer the new node

@par Code example:
@verbatim
int main(void)
{   Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt  = _newTask(...);
    npt = _newNode(pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_newNode(CosTask_t *task_pt)
{   Node_t *pt;
    pt = (Node_t *) malloc(sizeof(Node_t));
    if(pt!=NULL)
    {   pt->task_pt = task_pt;
        pt->next_pt = NULL;
    }
    return pt;
}
/*---------------------------------------------------------------*/

/*!
********************************************************************
  @par Description
  Allocates memory for a new task struct and initializes it. 
  * The pointer to the new task struct is returned.

@see _addTaskAtBeginningOfTaskList(), _newNode()
@arg

@param  prio - IN priority of the task (min) 1..254 (max)
@param  pData -IN pointer to private user data of the task
@param  func  -IN name of task callback-function


@retval Pointer to task struct or NULL on error

@par Code example:
@verbatim
int main(void)
{   Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt  = _newTask(...);
    npt = _newNode(pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
CosTask_t *_newTask(uint8_t prio, void * pData, void (*func) (CosTask_t *))
{  CosTask_t *pt;
   pt = (CosTask_t *)malloc(sizeof(CosTask_t));

   if(pt!=NULL)
   {  pt->lastActivationTime_Ticks  = _gettime_Ticks();
      pt->sleepTime_Ticks           = 0;  /* run asap */
      pt->state                     = TASK_STATE_READY;
      pt->prio                      = prio;
      pt->lineCnt                   = 0;    /* re-entry at start of function */
      pt->pData                     = pData;
      pt->func                      = func;
   }
   return pt;
}

/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Description
  Sorts the linear task-list due to priority. The task with highest 
  * priority is pointed to by the first node. Sorting doesn't change
  * the root pointer to the list, since only node data is switched,
  * not node order.


@param root_pt - IN, pointer to first node in task-list



@par Code example:
@verbatim
int main(void)
{   ...
    _sortLinearListPrio(root_pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
void _sortLinearListPrio(Node_t *root_pt)
{

    int noOfSwaps;
    CosTask_t *temp_pt;
    Node_t *pt;

    if (root_pt==NULL) return;  /* empty list */

    /* bubblesort, slow but simple... */
    do
    {   noOfSwaps=0;
        pt = root_pt;
        while(pt->next_pt != NULL)  // run through linear list
        {   if((pt->task_pt)->prio < ((pt->next_pt)->task_pt)->prio)
            {   /* swap the 'payloads' of the nodes*/
                noOfSwaps++;
                temp_pt = pt->task_pt;  /* swap payload of nodes */
                pt->task_pt = (pt->next_pt)->task_pt;
                (pt->next_pt)->task_pt = temp_pt;
            }
            pt = pt->next_pt;
        }
    } while(noOfSwaps >0);
}
