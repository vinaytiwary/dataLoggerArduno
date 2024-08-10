#include "timer.h"
#include "schedular.h"
#include <string.h>
#include <Arduino.h>
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
schedular_flg_t schedular_flg;

void init_timer()                                 //VINAY 15/07/2023
{
 timer = timerBegin(0, 80, true);             // timer 0, prescalar: 80, UP counting
 timerAttachInterrupt(timer, &onTimer, true);   // Attach interrupt
 timerAlarmWrite(timer, 10000, true);     // Match value= 1000000 for 1 sec. delay.
 timerAlarmEnable(timer);                 // Enable Timer with interrupt (Alarm Enable)
 init_schedular();
}

void IRAM_ATTR onTimer()                         //VINAY 15/07/2023
{      //Defining Inerrupt function with IRAM_ATTR for faster access
 portENTER_CRITICAL_ISR(&timerMux);
 //interruptCounter++;
 static unsigned char cnt_50ms = 0;
 static unsigned char cnt_100ms = 0;
 static unsigned char cnt_1sec = 0;
 cnt_50ms++;
 cnt_100ms++;
 cnt_1sec++;
 schedular_flg.flg_10ms  = true;
 if(cnt_50ms == SCHEDULE_50MS_CNT)
 {
    cnt_50ms = 0;
    schedular_flg.flg_50ms  = true;
  }
  /* 100msec flag check to perform 100msec task */
  if(cnt_100ms == SCHEDULE_100MS_CNT)
  {
    cnt_100ms = 0;
    schedular_flg.flg_100ms = true;
  }
  /* 1sec flag check to perform 1sec task */
  if(cnt_1sec == 100)
  {
    cnt_1sec = 0;
    schedular_flg.flg_1sec = true;
  }
 //Serial.print("ISR 1\n ");
 portEXIT_CRITICAL_ISR(&timerMux);
}

void init_schedular( void )                           //VINAY 15/07/2023
{
  schedular_flg.flg_10ms = false;
  schedular_flg.flg_50ms = false;
  schedular_flg.flg_100ms = false;
  schedular_flg.flg_1sec = false;
}
