#include <CosScheduler.h>

/*! new 3D vector data type */
typedef struct
{ double x; /**< x-coordinate */
  double y; /**< y-coordinate */
  double z; /**< z-coordinate */
}Vector_t;

CosTask_t *tProducer, *tConsumer;    /*!< pointer to tasks */
CosFifo_t fifo_1;               /*!< global fifo structure for inter-task communication */

/*!
 **********************************************************************
 * @par Description:
 *  The task sends a value of type 'Vector_t' through a FIFO to the
 *  consumerTask() in a loop. The producer synchronizes to the
 *  consumer.
 *
 * @see consumerTask()
 * @arg  -
 *
 *
 * @param  pt              - IN/OUT, pointer to task
 ************************************************************************/
 void producerTask(CosTask_t *pt)
{   static int16_t x=0;
    static Vector_t vec = {1,1,1};
    COS_TASK_BEGIN(pt);

#if COS_PLATFORM == PLATFORM_ARDUINO
    Serial.print("\r\nProd started\r\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    SerialUSB.print("\r\nProd started\r\n"); 
#endif
    
    for (x=0; x<100; x++)
    {    
         vec.x += 1.5;
         vec.y *= 1.1;
         vec.z -= 0.3;
#if COS_PLATFORM == PLATFORM_ARDUINO
         Serial.print("prod sends: vec=[");
         Serial.print(vec.x); Serial.print(", ");
         Serial.print(vec.y); Serial.print(", ");
         Serial.print(vec.z); Serial.print("]");
         Serial.print("\r\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
         SerialUSB.print("prod sends: vec=[");
         SerialUSB.print(vec.x); SerialUSB.print(", ");
         SerialUSB.print(vec.y); SerialUSB.print(", ");
         SerialUSB.print(vec.z); SerialUSB.print("]");
         SerialUSB.print("\r\n");
#endif
         COS_FifoBlockingWriteSingleSlot(pt, &fifo_1, &vec);
         COS_TASK_SLEEP(pt,_milliSecToTicks(200));
    }
#if COS_PLATFORM == PLATFORM_ARDUINO
    Serial.print("Prod ends\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    SerialUSB.print("Prod ends\n");
#endif
    
    COS_TASK_END(pt);
}

/*---------------------------------------------------*/


/*!
 **********************************************************************
 * @par Description:
 *  This task receives a value of type 'Vector_t' from producerTask() through
 *  a FIFO. The consumerTask() synchronizes with  producerTask().
 *
 * @see consumerTask()
 * @arg  -
 *
 *
 * @param  pt              - IN/OUT, pointer to task
 ************************************************************************/
 void consumerTask(CosTask_t *pt)
{
    static Vector_t vec={0,0,0};
    COS_TASK_BEGIN(pt);

#if COS_PLATFORM == PLATFORM_ARDUINO
    Serial.print("\r\nCons started\r\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    SerialUSB.print("\r\nCons started\r\n");
#endif
    while(1)
    {    COS_FifoBlockingReadSingleSlot(pt, &fifo_1, &vec);
#if COS_PLATFORM == PLATFORM_ARDUINO
         Serial.print("cons: vec=[");
         Serial.print(vec.x); Serial.print(", ");
         Serial.print(vec.y); Serial.print(", ");
         Serial.print(vec.z); Serial.print("]");
         Serial.print("\r\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
         SerialUSB.print("cons: vec=[");
         SerialUSB.print(vec.x); SerialUSB.print(", ");
         SerialUSB.print(vec.y); SerialUSB.print(", ");
         SerialUSB.print(vec.z); SerialUSB.print("]");
         SerialUSB.print("\r\n");
#endif    
         COS_TASK_SLEEP(pt,_milliSecToTicks(1000));
    }
#if COS_PLATFORM == PLATFORM_ARDUINO
    Serial.print("Cons ends\r\n");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    SerialUSB.print("Cons ends\r\n");
#endif    

    COS_TASK_END(pt);
}


/*--------------------------------------------------*/





void setup()
{
#if COS_PLATFORM == PLATFORM_ARDUINO
        Serial.begin(9600);
#endif // COS_PLATFORM
    CosInitTaskList();
    COS_FifoCreate(&fifo_1, sizeof(Vector_t), 5);

    tProducer = CosCreateTask(4, NULL, producerTask);
    tConsumer = CosCreateTask(5, NULL, consumerTask);
}

void loop()
{   delay(2000);
#if COS_PLATFORM == PLATFORM_ARDUINO
    Serial.print("\r\nopenCM test: send a vector via FIFO\r");
#endif
#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04
    SerialUSB.print("\r\nopenCM test: send a vector via FIFO\r");
#endif    
    delay(2000);
    CosVersionInfo();
    CosPrintTaskList();
    CosRunScheduler();  // endless loop
}



