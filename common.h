#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#define DEFAULT_YEAR  (2023)    
#define YEAR_OFFSET   (10)
#define LOG_COLUMN_COUNT 8
//#define correct 1
//#define incorrect 0
#define E2P_ADD_FILE_NAME    (16)
//#define EEPROM_SIZE (64)

typedef enum
{
  correct,
  incorrect,
}date_time_status_t;

typedef struct
{
  char dd;
  char mm;
  char yy;
  char QQ;
  unsigned char reserved[3];
  unsigned char chksum;
}__attribute__((packed)) e2p_date_t;


typedef struct
{
  char yy;
  char mm;
  char dd;
  char hr;
  char min;
  char sec;
  //char GPS_flag;
}time_stamp_t;
typedef struct
{
  int   year;   
  int   month;   
  int   day;    
  int   hour;    
  int   min;     
  int   sec;
}gps_date_time_t;

typedef struct
{
  float lat;
  float lon;
  float speed;  
  float alt;     
  int   vsat;     
  int   usat;   
  float accuracy; 
  //time_stamp_t time_stamp; 
  gps_date_time_t gps_date_time;
  char GPS_flag;     
}gps_data_t;

typedef struct
{
  int32_t lat;
  int32_t lon;
  int32_t speed;
  int32_t alt;
  int   vsat;
  int   usat;
  int32_t accuracy;
  //int   year;
  //int   month;
  //int   day;
  //int   hour;
  //int   min;
  //int   sec;
  time_stamp_t time_stamp;
}ram_data_t;

#endif
