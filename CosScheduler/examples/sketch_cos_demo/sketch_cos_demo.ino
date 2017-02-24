#include <CosScheduler.h>

#if COS_PLATFORM == PLATFORM_ARDUINO
    #define PRINT_TO_TERMINAL(x) Serial.print(x)
#endif // COS_PLATFORM
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    #define PRINT_TO_TERMINAL(x) SerialUSB.print(x)
#endif // COS_PLATFORM




CosTask_t *tA, *tB, *tC;                  /*!< pointer to tasks */
CosTask_t *tProducer, *tConsumer;         /*!< pointer to tasks */
CosFifo_t fifo_1; /*!< global fifo for inter-task communication */
CosSema_t sema_1; /*!< global semaphor for task synchornisation */


/*!
 *****************************************************************
 * @par Description:
 * This task has several scheduling points. It carries out steps
 *   0,..,4. Before the task terminates, it waits (blocking wait)
 *   at a semaphore, synchronizing to task myTask2().
 *
 * @see myTask2()
 * @arg  -
 * @param  pt              - IN/OUT, pointer to task struct
 ****************************************************************/
 void myTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);

    PRINT_TO_TERMINAL("T started\r\n");
    PRINT_TO_TERMINAL("T: step0\n");
    COS_TASK_SCHEDULE(pt);
    PRINT_TO_TERMINAL("T:s1\n");
    COS_TASK_SCHEDULE(pt);
    PRINT_TO_TERMINAL("T:s2\n");
    COS_TASK_SCHEDULE(pt);
    PRINT_TO_TERMINAL("T:s3\n");
    COS_TASK_SCHEDULE(pt);
    PRINT_TO_TERMINAL("T:s4\n");

    COS_SEM_WAIT(&sema_1,pt);
    PRINT_TO_TERMINAL("T:end\r\n");
    COS_TASK_END(pt);
}

/*!
 *****************************************************************
 * @par Description:
 *  This task increments a counter and prints its value on the
 *  termial. It runs in a loop with additional sleep-time.
 *  Before it terminates, is sends a signal to a semaphore, thus
 *  synchronizing to task myTask().
 *
 * @see myTask()
 * @arg  -
 * @param  pt              - IN/OUT, pointer to task struct
 ****************************************************************/
 void myTask2(CosTask_t *pt)
{   static int16_t count =0;
    COS_TASK_BEGIN(pt);

    PRINT_TO_TERMINAL("T2 started\r\n");
    while(count < 10)
    {    PRINT_TO_TERMINAL("T2 cnt=");
         PRINT_TO_TERMINAL(count++);
         PRINT_TO_TERMINAL("\r\n");
         COS_TASK_SLEEP(pt,_milliSecToTicks(500));
    }
    COS_SEM_SIGNAL(&sema_1);
    PRINT_TO_TERMINAL("T2 end\r\n");
    COS_TASK_END(pt);
}

/*!
 *****************************************************************
 * @par Description:
 *  This task increments a counter and prints its value on the
 *  termial. It runs in a loop with additional sleep-time.
 *
 * @see myTask()
 * @arg  -
 * @param  pt              - IN/OUT, pointer to task struct
 ****************************************************************/
 void myTask3(CosTask_t *pt)
{   static int16_t count =0;
    COS_TASK_BEGIN(pt);

    PRINT_TO_TERMINAL("T3 started\r\n");
    while(count < 10)
    {    PRINT_TO_TERMINAL("T3 cnt=");
         PRINT_TO_TERMINAL(count++);
         PRINT_TO_TERMINAL("\r\n");
         COS_TASK_SLEEP(pt,_milliSecToTicks(100));
    }
    PRINT_TO_TERMINAL("T3 ends\r\n");
    COS_TASK_END(pt);
}

/*!
 *****************************************************************
 * @par Description:
 *  The task runs in a loop and sends integer values via FIFO to
 *  the consumer task. The producer synchronizes to the consumer.
 *
 * @see consumerTask()
 * @arg  -
 * @param  pt              - IN/OUT, pointer to task struct
 ****************************************************************/
 void producerTask(CosTask_t *pt)
{   static int16_t x=0;
    COS_TASK_BEGIN(pt);

    PRINT_TO_TERMINAL("\r\nProd started\r\n");
    for (x=0; x<10; x++)
    {    PRINT_TO_TERMINAL("prod sends:");
         PRINT_TO_TERMINAL(x);
         PRINT_TO_TERMINAL("\r\n");
         COS_FifoBlockingWriteSingleSlot(pt, &fifo_1, &x);
         COS_TASK_SLEEP(pt,_milliSecToTicks(200));
    }
    PRINT_TO_TERMINAL("Prod ends\n");
    COS_TASK_END(pt);
}

/*!
 *****************************************************************
 * @par Description:
 * The consumer task runs in an endless loop an receives integer
 * values via FIFO from the producer task. The consumer
 * synchronizes to the producer.
 *
 * @see consumerTask()
 * @arg  -
 * @param  pt              - IN/OUT, pointer to task struct
 ****************************************************************/
 void consumerTask(CosTask_t *pt)
{   static int16_t x=0;
    COS_TASK_BEGIN(pt);

    PRINT_TO_TERMINAL("\r\nCons started\r\n");
    while(1)
    {    COS_FifoBlockingReadSingleSlot(pt, &fifo_1, &x);
         PRINT_TO_TERMINAL("cons x=");
         PRINT_TO_TERMINAL(x);
         PRINT_TO_TERMINAL("\r\n");
         COS_TASK_SLEEP(pt,_milliSecToTicks(1000));
    }
    PRINT_TO_TERMINAL("Cons ends\r\n");
    COS_TASK_END(pt);
}


/*--------------------------------------------------*/
void setup()
{
    #if COS_PLATFORM == PLATFORM_ARDUINO
        Serial.begin(9600);
    #endif // COS_PLATFORM

    if(0!=CosInitTaskList())
    {   PRINT_TO_TERMINAL("error in COS_InitTaskList!\r\n");
    }
    COS_SemCreate(&sema_1, 0);
    COS_FifoCreate(&fifo_1, sizeof(int16_t), 5);

    tA = CosCreateTask(1, NULL, myTask);
    tB = CosCreateTask(2, NULL, myTask2);
    tC = CosCreateTask(3, NULL, myTask3);
    tProducer = CosCreateTask(4, NULL, producerTask);
    tConsumer = CosCreateTask(5, NULL, consumerTask);
}
/*--------------------------------------------------*/
void loop()
{   delay(2000);
    CosVersionInfo();
    CosPrintTaskList();
    CosRunScheduler();
} 




