#include "common.h"
#include "EEPROM.h"
#include "ESP32Time.h"
#include "time.h"
#include "Functions.h"
#include "debug.h"
extern ESP32Time rtc;
//extern time_main_t time_main;
e2p_date_t e2p_date;

void write_eeprom()              //Vinay 31-07-2023
{
  int ret = 0;
  //memset(&e2p_date, 0, sizeof(e2p_date_t));
  //tm_config->chksum = getChecksum((unsigned char*)(tm_config), (sizeof(tm_config_t) - 1));
//  e2p_date.yy = rtc.getYear();
//  e2p_date.mm = rtc.getMonth();
//  e2p_date.dd = rtc.getDay();
//  e2p_date.QQ = (rtc.getHour(true)/6)+1;
  e2p_date.chksum = getChecksum((unsigned char*)(&e2p_date), (sizeof(e2p_date_t) - 1));
  
  EEPROM.put(E2P_ADD_FILE_NAME, e2p_date);

//  EEPROM.write(E2P_ADD_FILE_NAME,e2p_date.yy);
//  EEPROM.write(E2P_ADD_FILE_NAME+1,e2p_date.mm);
//  EEPROM.write(E2P_ADD_FILE_NAME+2,e2p_date.dd);
//  EEPROM.write(E2P_ADD_FILE_NAME+3,e2p_date.QQ);

  ret = EEPROM.commit();

  Serial.print("com=");
  Serial.print(ret);
  /*
  Serial.print("e2p_yy:");
  Serial.print(e2p_date.yy);
  Serial.print("e2p_mm:");
  Serial.print(e2p_date.mm);
  Serial.print("e2p_dd:");
  Serial.print(e2p_date.dd);
  Serial.print("e2p_hr:");
  Serial.print(e2p_date.hr);
  Serial.print("e2p_min:");
  Serial.print(e2p_date.min);
  Serial.print("e2p_sec:");
  Serial.print(e2p_date.sec);
  */
  //Serial.println("e2p_write:");
  //Serial.println((char*)&e2p_date);
#ifdef DEBUG_EEPROM
  Serial.print("e2p_write.yy=");
  Serial.println(e2p_date.yy);
  Serial.print("e2p_write.mm=");
  Serial.println(e2p_date.mm);
  Serial.print("e2p_write.dd=");
  Serial.println(e2p_date.dd);
  Serial.print("e2p_write.qq=");
  Serial.println(e2p_date.QQ);
#endif
}

void read_eeprom()//e2p_date_t *e2p_date)          //Vinay 31-07-2023
{
  EEPROM.get(E2P_ADD_FILE_NAME,e2p_date);
  //Serial.println("e2p_read:");
  //Serial.println((char*)&time_main);
  //Serial.println((char*)&e2p_date);
  
#ifdef DEBUG_EEPROM
  Serial.print("e2p_rd.yy=");
  Serial.println(e2p_date.yy);
  Serial.print("e2p_rd.mm=");
  Serial.println(e2p_date.mm);
  Serial.print("e2p_rd.dd=");
  Serial.println(e2p_date.dd);
  Serial.print("e2p_rd.qq=");
  Serial.println(e2p_date.QQ);
#endif

}

/*unsigned*/ char getChecksum(unsigned char* buff, char len)
{
  /*unsigned*/ char chksum=0;
  int i=0;

  for(i=0 ; i<len ; i++)
  {
    chksum+=buff[i];
  }
  
  chksum = (~chksum) + 1;

  return chksum;
}
void date_change_e2p()             //Vinay 02-08-2023           
{
  if (((rtc.getYear() == 2000) && ((rtc.getMonth()+1) == 01) && (rtc.getDay() == 01)))
  {
    Serial.println("rtc=default");
  }
  else if((e2p_date.mm == (rtc.getMonth()+1)) && (e2p_date.dd == rtc.getDay()) && (e2p_date.QQ == ((rtc.getHour(true)/6)+1)))
    {
      Serial.println("rtc=default_time");
    }
  else
  {
    if ((e2p_date.QQ != ((rtc.getHour(true)/6)+1)))
      { 
        memset(&e2p_date, 0, sizeof(e2p_date_t));
        e2p_date.yy = (rtc.getYear())%100;
        e2p_date.mm = (rtc.getMonth()+1);
        e2p_date.dd = rtc.getDay();
        e2p_date.QQ = ((rtc.getHour(true)/6)+1);
        Serial.println("e2p_qq!=H/6+1"); 
        write_eeprom();
      }
      else if(e2p_date.dd != rtc.getDay())
      {  
        memset(&e2p_date, 0, sizeof(e2p_date_t));  
        e2p_date.yy = (rtc.getYear())%100;
        e2p_date.mm = (rtc.getMonth()+1);
        e2p_date.dd = rtc.getDay();
        e2p_date.QQ = ((rtc.getHour(true)/6)+1);
        Serial.println("e2p_D!=RTC_D"); 
        write_eeprom();
      }
      else if(e2p_date.mm != (rtc.getMonth()+1))
      {      
        memset(&e2p_date, 0, sizeof(e2p_date_t));      
        e2p_date.yy = (rtc.getYear())%100;
        e2p_date.mm = (rtc.getMonth()+1);
        e2p_date.dd = rtc.getDay();
        e2p_date.QQ = ((rtc.getHour(true)/6)+1);
        Serial.println("e2p_M!=RTC_M");     
        write_eeprom();
      }
      else if (e2p_date.yy != (rtc.getYear())%100)
      {     
        memset(&e2p_date, 0, sizeof(e2p_date_t));        
        e2p_date.yy = (rtc.getYear())%100;
        e2p_date.mm = (rtc.getMonth()+1);
        e2p_date.dd = rtc.getDay();
        e2p_date.QQ = ((rtc.getHour(true)/6)+1);
        Serial.println("e2p_Y!=RTC_Y");     
        write_eeprom();    
      } 
  }
     
}
