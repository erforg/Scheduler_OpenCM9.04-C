/*!
 ********************************************************************
   @file            cos_data_fifo.c
   @par Project   : co-operative scheduler (COS)
   @par Module    : FIFO Mailbox for COS scheduler

   @brief  Data-FIFO (queue) for COS.


   @par Author    : Ernst Forgber (Fgb)
   @par Company   : Hochschule Hannover - University of Applied Sciences and Arts, Germany
   @par Department: Faculty 1


   @par Description
   This module implements a simple FIFO for inter-task communication for COS.
   A FIFO has several data slots to hold a maximum of 255 Byte each.

   @verbatim

 |---------|            -----------            |---------|
 | producer|-----------> |  |  |  | ---------->| consumer|
 |---------|            -----------            |---------|
                           FIFO
   @endverbatim
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 08.09. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 08.10. 2015 | Fgb           | Umbau auf renesas controller
   0.3     | 21.11. 2016 | Fgb           | english docu
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
#include <string.h>  // for memcpy()
#include "cos_ser.h"
#include "cos_data_fifo.h"




/*------------- DEBUGGING ---------------------------------*/
#define DEBUG_MODULE 0  /*!< 0 for no additional debug code, 1 otherwise */ 

#if DEBUG_MODULE
  #define DebugCode( code_fragment ) { code_fragment } /*!< for debugging only: insert some code */ 
#else
  #define DebugCode( code_fragment ) /*!< for debugging only: insert some code */ 
#endif

#if DEBUG_MODULE
static void _msg(char *msg)
{
    DebugCode(serPuts(msg););
}
#endif
/*---------------------------------------------------------------*/



/*!
 **********************************************************************
 * @par Description:
 * This function creates a FIFO for data transfer among tasks. The FIFO
 * has 'nSlots' storage slots, each of size 'slotSize' bytes.
 *
 * @see
 * @arg  COS_FifoDestroy()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 * @param  slotSize        - IN, size of data slot (1..255) in bytes
 * @param  nSlots          - IN, number of slots (1..255) in the FIFO
 *
 * @retval 0               - no error
 * @retval negative        - an error occurrred
 *
 * @par Code example: 
 *    A global FIFO with 5 slots for float variables and a second queue with 2
 *    slots for int arrays with 5 elements each  may be created
 *    and destroyed like this:
 * @verbatim
CosFifo_t q01, q02;

int main(void)
{
  ...
  if(0!= COS_FifoCreate(&q01, sizeof(float),5))
    serPuts("error creating queue");
  if(0!= COS_FifoCreate(&q02, 5*sizeof(int8_t),2))
    serPuts("error creating queue");
  ...
  COS_FifoDestroy(&q01);
  COS_FifoDestroy(&q02);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
uint8_t COS_FifoCreate(CosFifo_t *q, uint8_t slotSize, uint8_t nSlots)
{
  /* create buffer */
  q->buffer = (char *) malloc(slotSize * nSlots * sizeof(char));
  if(NULL == q->buffer)
  { DebugCode(_msg("FifoCreate:malloc!"););
    return -1;
  }
  q->maxSlots  = nSlots;
  q->slotSize  = slotSize;
  q->rIndex    = 0;         /* empty queue */
  q->wIndex    = 0;
  q->usedSlots = 0;
  if(0!= COS_SemCreate(&(q->rSema), 0))  // nothing to read yet
  {  DebugCode(_msg("FifoCreate:SemCreate!"););
     return -1;
  }
  if(0!= COS_SemCreate(&(q->wSema), nSlots)) // all slots are still free
  {  DebugCode(_msg("FifoCreate:SemCreate!"););
     return -1;
  }
  q->isInitialized = 1;
  return 0;
}



/*!
 **********************************************************************
 * @par Description:
 * This function deletes a FIFO and frees its memory.
 *
 * @see
 * @arg  COS_FifoCreate()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 *
 * @retval 0               - no error 
 * @retval negative        - an error occurred
 *
 * @par Code-Beispiel :
 *   @see
 *   @arg COS_FifoCreate()
 *
 * @verbatim
  @endverbatim
 ************************************************************************/
uint8_t COS_FifoDestroy(CosFifo_t *q)
{
  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoDestroy:not init."););
    return -1;
  }
  /* delete buffer */
  if(q->buffer != NULL)
  { free(q->buffer);
    q->buffer = NULL;
  }
  q->isInitialized = 0;
  COS_SemDestroy(&(q->rSema));
  COS_SemDestroy(&(q->wSema));
  return 0;
}



/*!
 **********************************************************************
 * @par Description:
 * This function checks if a FIFO is empty.
 *
 * @see
 * @arg  COS_FifoIsFull()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 *
 * @retval 0               - FIFO not empty
 * @retval 1               - FIFO empty
 * @retval negative        - error
 *
 * @par Code example:
 * @verbatim
CosFifo_t q01;

void Task_A(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    ...
    if(!COS_FifoIsEmpty(&q01))
    {  // FIFO ist nicht leer
       ...
    }
    ...
    COS_TASK_END(pt);
}
  @endverbatim
 ************************************************************************/
int8_t COS_FifoIsEmpty(CosFifo_t *q)
{ int8_t retval=0;

  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoIsEmpty:not init"););
    return -1;
  }
  if(q->usedSlots == 0)
    retval = 1;
  else
    retval = 0;
  return retval;
}



/*!
 **********************************************************************
 * @par Description:
 * This function checks if a FIFO is full.
 *
 * @see
 * @arg  COS_FifoIsEmpty()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 *
 * @retval 0               - FIFO is not full
 * @retval 1               - FIFO is full
 * @retval negative        - error
 *
 * @par Code example:
 * @verbatim
CosFifo_t q01;

void Task_A(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    ...
    if(COS_FifoIsFull(&q01))
    {  // FIFO ist voll
       ...
    }
    ...
    COS_TASK_END(pt);
}

  @endverbatim
 ************************************************************************/
int8_t COS_FifoIsFull(CosFifo_t *q)
{ int8_t retval=0;

  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoIsFull:not init"););
    return -1;
  }
  if(q->usedSlots == q->maxSlots)
    retval = 1;
  else
    retval = 0;
  return retval;
}





/*!
 **********************************************************************
 * @par Description:
 * PLEASE NOTE: This function is for internal use only! 
 * The normal application should
 * use the macro COS_FifoBlockingWriteSingleSlot() instead!
 * The macro calls this function and implements blocking write to
 * a FIFO. This function writes a data slot to the FIFO. 
 * The number of 
 * bytes to be written is 'slotSize', as declared at the call of 
 * COS_FifoCreate(). If the FIFO is full, nothing is written. 
 * 
 *
 * @see
 * @arg  _qReadSingleSlot(), COS_FifoBlockingWriteSingleSlot()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 * @param  data            - IN, pointer to data
 *
 * @retval number of slots actually written,  0 if FIFO is full
 * @retval -1              - error
 ************************************************************************/
int8_t _qWriteSingleSlot(CosFifo_t *q, const char *data)
{ int8_t retval = 0;
  if(q->isInitialized == 0)
  { DebugCode(_msg("_qWriteSingleSlot:not init"););
    return -1;
  }
  /* if FIFO is full, return 0 without writing any data */
  if(q->usedSlots >= q->maxSlots)
  { retval = 0;  /* FIFO is full, don't write */
  }
  else  /* at least one slot is free, write data */
  { retval = 1;
    memcpy(&(q->buffer[q->wIndex]), data, q->slotSize); /* copy to FIFO */
    q->wIndex += q->slotSize;                  /* next slot */
    q->wIndex %= (q->maxSlots * q->slotSize);  /* circular buffer */
    q->usedSlots  += 1;
    COS_SEM_SIGNAL(&(q->rSema));  // unblock tasks that wait for reading,
  }
  return retval;
}










/*!
 **********************************************************************
 * @par Description:
 * PLEASE NOTE: This function is for internal use only! 
 * The normal application should
 * use the macro COS_FifoBlockingReadSingleSlot() instead!
 * The macro calls this function and implements blocking read from
 * a FIFO.
 * This function reads a data slot from the FIFO. 
 * The number of 
 * bytes to be read is 'slotSize', as declared at the call of 
 * COS_FifoCreate(). If the FIFO is empty, nothing is read. 
 *
 * @see
 * @arg  _qWriteSingleSlot(), COS_FifoBlockingReadSingleSlot()
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 * @param  data            - IN/OUT, pointer to data
 *
 * @retval number of slots actually read,  0 if FIFO is empty
 * @retval -1              - error
 ************************************************************************/
int8_t _qReadSingleSlot(CosFifo_t *q, char *data)
{ int8_t retval =0;
  if(q->isInitialized == 0)
  { DebugCode(_msg("_qReadSingleSlot:not init"););
    return -1;
  }
  /* if FIFO is empty, nothing is read */
  if (q->usedSlots <=0)
  {   retval = 0;
  }
  else /* at least one slot has data */
  {   retval = 1;
       memcpy(data, &(q->buffer[q->rIndex]), q->slotSize); /* read from queue */
       q->rIndex += q->slotSize;                  /* next slot to read */
       q->rIndex %= (q->maxSlots * q->slotSize);  /* circular buffer */
       q->usedSlots  -= 1;
       COS_SEM_SIGNAL(&(q->wSema));  // unblock tasks that wait for writing
  }
  return retval;
}






/*!
 **********************************************************************
 * @par Description:
 * This function returns the number of used slots in a FIFO.
 *
 * @see COS_FifoGetMaxSlots(), COS_FifoGetSlotSize()
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO struct
 *
 * @retval number of used slots
 ************************************************************************/
uint8_t COS_FifoGetUsedSlots(CosFifo_t *q)
{   return q->usedSlots;
}




/*!
 **********************************************************************
 * @par Description:
 * This fuction retruns the total number of slots in the FIFO.
 *
 * @see  COS_FifoGetUsedSlots(), COS_FifoGetSlotSize()
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO
 *
 * @retval total number of slots in the FIFO
 ************************************************************************/
uint8_t COS_FifoGetMaxSlots(CosFifo_t *q)
{   return q->maxSlots;
}


/*!
 **********************************************************************
 * @par Description:
 * This fuction retruns the size of a data slot in the FIFO.
 *
 * @see
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, pointer to FIFO
 *
 * @retval slot size in bytes
 ************************************************************************/
uint8_t COS_FifoGetSlotSize(CosFifo_t *q)
{
  return q->slotSize;
}
















