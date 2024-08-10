#ifndef SCHEDULAR_H_
#define SCHEDULAR_H_

#define SCHEDULE_10MS_CNT  10/TIMER0_VALUE
#define SCHEDULE_50MS_CNT 50/TIMER0_VALUE
#define SCHEDULE_100MS_CNT  100/TIMER0_VALUE
#define SCHEDULE_1SEC_CNT 1000/TIMER0_VALUE

typedef struct                               //VINAY 15/07/2023
{
  volatile char flg_10ms;
  volatile char flg_50ms;
  volatile char flg_100ms;
  volatile char flg_1sec;
}schedular_flg_t;

void init_schedular();

#endif
