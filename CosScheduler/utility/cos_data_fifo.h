/*!
 ********************************************************************
   @file            cos_data_fifo.h
   @par Project   : co-operative scheduler (COS) 
   @par Module    : FIFO Mailbox for COS Scheduler

   @brief  Data-FIFO (queue) for COS. The FIFO uses dynamic memory allocation
          (malloc()). A FIFO is initialized for a data type and can  
          store data of only that type. The maximum number of storage
          places (slots) is 255 due to 1 byte index variables. A slot 
          can store a maximum of 256 bytes.

   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author | Change Description
   0.0     | 07.09. 2011 | Fgb    | First Version, Linux
   0.1     | 17.09. 2013 | Fgb    | nur noch Atmel, deutsche Doku
   0.2     | 08.10. 2015 | Fgb    | Umbau auf renesas controller
   0.3     | 21.11. 2016 | Fgb    | english docu

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

#ifndef _cos_data_fifo_h_
#define _cos_data_fifo_h_


#include "cos_configure.h"
#include "cos_semaphore.h"

//#include <avr/io.h>
//#include <stdint.h>
#include "cos_types.h"

#ifndef NULL
    #define NULL 0  /*!< the null pointer value */
#endif



/*!
 ********************************************************************
  @par Description
  FIFO data structure :
********************************************************************/ 
typedef struct                 /*! FIFO data structure */
{ 
        char *buffer;          /*!< queue Data buffer */
        uint8_t maxSlots;      /*!< total number of slots in the queue  */
        uint8_t slotSize;      /*!< size of a slot in bytes */
        uint8_t rIndex;        /*!< read index variable of the queue */
        uint8_t wIndex;        /*!< write index variable of the queue */
        uint8_t usedSlots;     /*!< number of used slots */
        uint8_t isInitialized; /*!< 0 if not yet initialized */
        CosSema_t rSema;       /*!< wait at this semaphore when reading */
        CosSema_t wSema;       /*!< wait at this semaphore when writing */
} CosFifo_t;




uint8_t COS_FifoCreate(CosFifo_t *q, uint8_t slotSize, uint8_t nSlots);
uint8_t COS_FifoDestroy(CosFifo_t *q);
int8_t COS_FifoIsEmpty(CosFifo_t *q);
int8_t COS_FifoIsFull(CosFifo_t *q);

uint8_t COS_FifoGetUsedSlots(CosFifo_t *q);
uint8_t COS_FifoGetMaxSlots(CosFifo_t *q);
uint8_t COS_FifoGetSlotSize(CosFifo_t *q);

int8_t _qWriteSingleSlot(CosFifo_t *q, const char *data);
int8_t _qReadSingleSlot(CosFifo_t *q, char *data);

// blocking Macros

/*!
 **********************************************************************
 * @par Description:
 * This macro tries to store data in a FIFO slot. The number of 
 * bytes to be written is 'slotSize', as declared at the call of 
 * COS_FifoCreate(). IF there is no free slot, the macro changes the
 * task *pt to state TASK_STATE_BLOCKED. Another task reading from the 
 * FIFO will un-block it again. 
 *
 * @see
 * @arg  COS_FifoBlockingReadSingleSlot(), _qWriteSingleSlot()
 *
 * @par Macro parameters: (CosTask_t *pt, CosFifo_t *q,  char *data)
 *
 * @param  pt              - IN/OUT, pointer to task struct
 * @param  q               - IN/OUT, pointer to FIFO struct
 * @param  data            - IN, pointer to data
 * @retval void
  * @par Example :
 *    A global FIFO with 5 slots for float variables and a second queue with 2
 *    slots for int arrays with 5 elements each  may be created, used
 *    and destroyed like this:
 * @verbatim
CosFifo_t q01, q02;
CosTask_t *p_Task_A, *p_Task_B;

void Task_A(CosTask_t *pt)
{   static float x,y;
    static int8_t a[5];

    COS_TASK_BEGIN(pt);
    ...
    COS_FifoBlockingWriteSingleSlot(pt, &q01,&x);
    COS_FifoBlockingWriteSingleSlot(pt, &q01,&y);
    COS_FifoBlockingWriteSingleSlot(pt, &q02, a);
    ...
    COS_TASK_END(pt);
}

void Task_B(CosTask_t *pt)
{   static float  z[2];
    static int8_t b[5];

    COS_TASK_BEGIN(pt);
    ...
    COS_FifoBlockingReadSingleSlot(pt, &q02, b);
    COS_FifoBlockingReadSingleSlot(pt, &q01,&z[0]);
    COS_FifoBlockingReadSingleSlot(pt, &q01,&z[1]);
    ...
    COS_TASK_END(pt);
}

int main(void)
{ ...
  COS_InitTaskList();
  p_Task_A = COS_CreateTask(5, NULL, Task_A);
  p_Task_B = COS_CreateTask(7, NULL, Task_B);
  ...
  if(0!= COS_FifoCreate(&q01, sizeof(float),5))
    serPuts("error creating queue");
  if(0!= COS_FifoCreate(&q02, 5*sizeof(int8_t),2))
    serPuts("error creating queue");
  ...
  COS_RunScheduler();
  while(1);    // this should not be reached...
  return 0;
}
  @endverbatim
 ************************************************************************/
#define COS_FifoBlockingWriteSingleSlot(pt, q,  data)  COS_SEM_WAIT(&((q)->wSema),(pt)); \
                                                       _qWriteSingleSlot((q), (char *)(data))



/*!
 **********************************************************************
 * @par Description:
 * This macro tries to read a data slot from the FIFO.
 * The number of 
 * bytes to be read is 'slotSize', as declared at the call of 
 * COS_FifoCreate(). IF there is no data in the FIFO, the macro changes the
 * task *pt to state TASK_STATE_BLOCKED. Another task writing to the 
 * FIFO will un-block it again. 
 *
 * @see
 * @arg  COS_FifoBlockingWriteSingleSlot(), _qReadSingleSlot()
 *
 * @par Macro Parameter: (CosTask_t *pt, CosFifo_t *q,  char *data)
 *
 * @param  pt              - IN/OUT, pointer to task struct
 * @param  q               - IN/OUT, pointer to FIFO struct
 * @param  data            - IN/OUT, pointer to data
 * @retval void
  * @par Example :
  Siehe bei COS_FifoBlockingWriteSingleSlot()
 * @verbatim
  @endverbatim
 ************************************************************************/
#define COS_FifoBlockingReadSingleSlot(pt, q,  data)   COS_SEM_WAIT(&((q)->rSema),(pt)); \
                                                       _qReadSingleSlot((q), (char *)(data))

#endif



