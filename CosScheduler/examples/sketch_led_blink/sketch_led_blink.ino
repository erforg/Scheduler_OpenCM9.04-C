#include <CosScheduler.h>

const int pin = BOARD_LED_PIN;
CosTask_t *tA, *tB;        /*!< pointer to tasks */


void myClockTask(CosTask_t *pt)
{   static uint8_t second=0, minute=0, hour=0;
    COS_TASK_BEGIN(pt);

    SerialUSB.print("Clock started\r\n");
    while(1)
    {
         SerialUSB.print(hour);   SerialUSB.print(":");
         SerialUSB.print(minute); SerialUSB.print(":");
         SerialUSB.print(second); SerialUSB.print("\r\n");
         COS_TASK_SLEEP(pt,_milliSecToTicks(1000));
         second++;
         if(second>=60)
         {  second = 0;
            minute++;
            if(minute>=60)
            {  minute = 0;
               hour++;
               if(hour>=24)
               {  hour=0;
               }
            }
         }
    }
    COS_TASK_END(pt);
}

/*---------------------------------------------------*/

 void myLedTask(CosTask_t *pt)
{   COS_TASK_BEGIN(pt);

    SerialUSB.print("LED blink started\r\n");
    while(1)
    {   COS_TASK_SLEEP(pt,_milliSecToTicks(200));
        digitalWrite(pin, HIGH);
        COS_TASK_SLEEP(pt,_milliSecToTicks(200));
        digitalWrite(pin, LOW);
    }
    COS_TASK_END(pt);
}

/*--------------------------------------------------*/

void setup()
{
    pinMode(pin, OUTPUT);
    CosInitTaskList();
    tA = CosCreateTask(1, NULL, myClockTask);
    tB = CosCreateTask(2, NULL, myLedTask);
}

void loop()
{   delay(2000);
    CosVersionInfo();
    CosPrintTaskList();
    CosRunScheduler();  // endless loop...
} 





