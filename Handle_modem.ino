#include "Hardware_pins.h"
#include <TinyGsmClient.h>
#include "common.h"
#include "ESP32Time.h"
#include "debug.h"
extern ESP32Time rtc; 

gps_data_t gps_data;
ram_data_t ram_data;
date_time_status_t date_time_status;
uint8_t simcom_power_on()
{
	mcp.pinMode(MCP_SIM_PWREN,OUTPUT);
	mcp.digitalWrite(MCP_SIM_PWREN,HIGH);
	delay(100);
	#ifdef DEBUG_ENABLE
		SerialMon.println("Powering up simcom");
	#endif
	simcom_power_reset();
	uint8_t retry=0;
	while(!mcp.digitalRead(MCP_SIM_1V8))
	{
		mcp.digitalWrite(MCP_SIM_ONOFF,HIGH);
		delay(2000);
		mcp.digitalWrite(MCP_SIM_ONOFF,LOW);
		delay(2000);
		if (retry==5) break;
		retry++;
	}
	if (mcp.digitalRead(MCP_SIM_1V8))
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("Modem booted..");
		#endif
		return SIMCOM_SUCCESS;
	}
	#ifdef DEBUG_ENABLE
		SerialMon.println("Modem booting failed..");
	#endif
	return SIMCOM_FAILED;
}

uint8_t simcom_safe_shutdown()
{
	if(mcp.digitalRead(MCP_SIM_1V8))
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("Powering off simcom");
		#endif
		uint8_t retry=0;
		SerialAT.begin(115200);
		delay(100);
		modem.poweroff();
		if(mcp.digitalRead(MCP_SIM_1V8))
		{
			mcp.digitalWrite(MCP_SIM_ONOFF,HIGH);
			delay(3000);
			mcp.digitalWrite(MCP_SIM_ONOFF,LOW);
			delay(2000);
		}
		uint8_t modem_poweroff_wait=0;
		while(mcp.digitalRead(MCP_SIM_1V8))
		{
			#ifdef DEBUG_ENABLE
				SerialMon.printf("Modem shutdown wait : %d\r\n",modem_poweroff_wait);
			#endif
			delay(1000);
			modem_poweroff_wait++;
			if (modem_poweroff_wait==30) break;
		}
		if (!mcp.digitalRead(MCP_SIM_1V8))
		{
			#ifdef DEBUG_ENABLE
				SerialMon.println("Modem shutdown done..");
			#endif
			return SIMCOM_SUCCESS;
		}
		else
		return SIMCOM_FAILED;
	}
	return SIMCOM_SUCCESS;
	
}


void simcom_power_reset()
{
	mcp.digitalWrite(MCP_SIM_RST,HIGH);
	delay(500);
	mcp.digitalWrite(MCP_SIM_RST,LOW);
	delay(500);
}

void simcom_power_off()
{
	mcp.pinMode(MCP_SIM_PWREN,OUTPUT);
	mcp.digitalWrite(MCP_SIM_PWREN,LOW);
	delay(500);
}

void modem_loopback()
{
	//SerialMon.begin(115200);
	//SerialAT.begin(115200, SERIAL_8N1, ESP_SIMCOM_RX, ESP_SIMCOM_TX);
	delay(1000);
	SerialMon.println("Routing serial AT to serial monitor");
	while(1)
	{
		while(SerialMon.available())
		{
			SerialAT.write(SerialMon.read());
		}
		while(SerialAT.available())
		{
			SerialMon.write(SerialAT.read());
		}
	}
}

uint8_t init_modem()
{
	simcom_power_on();
	//modem_loopback(); // test function to loopback the modem to debug uart
	SerialAT.begin(115200, SERIAL_8N1, ESP_SIMCOM_TX, ESP_SIMCOM_RX);
	delay(100);
	//modem_loopback();
	#ifdef DEBUG_ENABLE
		SerialMon.println("Initializing modem");
	#endif
	modem.init();
 delay(2000);
  enable_gps();
  char retry=0;
	while(retry<5)
  {
    retry++;
    if ( modem.getSimStatus() == 1 )
    {
      #ifdef DEBUG_ENABLE
         SerialMon.println("Sim card detected...");
         break;
      #endif
    }
    delay(1000);
  }
	if(retry==5)
  {
    #ifdef DEBUG_ENABLE
         SerialMon.println("Sim card error...");
    #endif
    return SIMCOM_FAILED; // sim not inserted
  }
  //enable_gps();
	get_modem_imei();
	modem.sendAT(GF("CNV=/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable,0,01,1"));  // Echo Off
	modem.sendAT(GF("CNV=/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable,0,01,1"));  // Echo Off
	return SIMCOM_SUCCESS;
	
	
}

uint8_t setup_connect_network()
{
	//modem.gprsConnect(apn, gprsUser, gprsPass);
	#ifdef DEBUG_ENABLE
	SerialMon.println(F("Checking network.. "));
	#endif
	if (modem.isNetworkConnected())
	{
		#ifdef DEBUG_ENABLE
		SerialMon.println(F("Already Connected to network"));
		#endif
	}
	else if (!modem.waitForNetwork(120000))
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("Connecting to network failed.");
		#endif
		return SIMCOM_FAILED;
	}
	#ifdef DEBUG_ENABLE
		SerialMon.println(F("Checking GPRS connection.. "));
	#endif
	if (!modem.isGprsConnected())
	{
		#ifdef DEBUG_ENABLE
			SerialMon.print(F("GPRS not connected.. connecting using APN: "));
			SerialMon.println(apn);
		#endif
		if (!modem.gprsConnect(apn, gprsUser, gprsPass))
		{
			#ifdef DEBUG_ENABLE
				SerialMon.println(F("GPRS connection failed"));
			#endif
			return SIMCOM_FAILED;
		}
	}
	else
	{
		#ifdef DEBUG_ENABLE
		SerialMon.println(F("GPRS already connected.. "));
		#endif
	}
	return SIMCOM_SUCCESS;
}

void enable_gps()
{
	modem.enableGPS();
  delay(1000);
#ifdef DEBUG_ENABLE
    SerialMon.println(F("GPS enabled "));
#endif
}
void disable_gps()
{
	modem.disableGPS();
}


uint8_t get_gps_location()
{
  //enable_gps();
 // modem.enableGPS();
 //SerialAT.write("AT+CGPS=1\r\n");
 /*
	float lat      = 0;
	float lon      = 0;
	float speed    = 0;
	float alt      = 0;
	int   vsat     = 0;
	int   usat     = 0;
	float accuracy = 0;
	int   year     = 0;
	int   month    = 0;
	int   day      = 0;
	int   hour     = 0;
	int   min      = 0;
	int   sec      = 0;
 */
	/*
	#ifdef DEBUG_ENABLE
		DBG("Requesting current GPS/GNSS/GLONASS location");
	#endif
  */
	if (modem.getGPS(&gps_data.lat, &gps_data.lon, &gps_data.speed, &gps_data.alt, &gps_data.vsat, &gps_data.usat, &gps_data.accuracy,&gps_data.gps_date_time.year, &gps_data.gps_date_time.month, &gps_data.gps_date_time.day, &gps_data.gps_date_time.hour, &gps_data.gps_date_time.min, &gps_data.gps_date_time.sec)) 
	{
      /*
      Serial.print("y=");
      Serial.println(gps_data.gps_date_time.year);
      Serial.print("M=");
      Serial.println(gps_data.gps_date_time.month);
      Serial.print("D=");
      Serial.println(gps_data.gps_date_time.day);
      Serial.print("mn=");
      Serial.println(gps_data.gps_date_time.min);
      */
  /*
		#ifdef DEBUG_ENABLE
			DBG("Latitude:", String(gps_data.lat, 8), "\tLongitude:", String(gps_data.lon, 8));
			DBG("Speed:", gps_data.speed, "\tAltitude:", gps_data.alt);
			DBG("Visible Satellites:", gps_data.vsat, "\tUsed Satellites:", gps_data.usat);
			DBG("Accuracy:", gps_data.accuracy);
			DBG("Year:", gps_data.year, "\tMonth:", gps_data.month, "\tDay:", gps_data.day);
			DBG("Hour:", gps_data.hour, "\tMinute:", gps_data.min, "\tSecond:", gps_data.sec);
		#endif
    */
      check_GPS_time(&gps_data.gps_date_time);
	} 
	else  
	{
		//#ifdef DEBUG_ENABLE
			//DBG("Couldn't get GPS/GNSS/GLONASS location");
		//#endif
	}
	 prepare_data();
	//DBG("Retrieving GPS/GNSS/GLONASS location again as a string");
	//String gps_raw = modem.getGPSraw();
	//DBG("GPS/GNSS Based Location String:", gps_raw);
	return SIMCOM_SUCCESS;
}

uint8_t get_modem_imei()
{
	for (char i=0;i<5;i++)
	{
		imei = modem.getIMEI();
		DBG("IMEI:", imei);
		if(imei.length()==15) return SIMCOM_SUCCESS;
		delay(1000);
	}
	#ifdef DEBUG_ENABLE
		SerialMon.println("Error getting IMEI");
	#endif
	return SIMCOM_FAILED;
}
uint8_t sync_modem_time()
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int sec = 0;
	float timezone = 0;
	
	#ifdef DEBUG_ENABLE
		DBG("Requesting current network time");
	#endif
	if (modem.getNetworkTime(&year, &month, &day, &hour, &minute, &sec,&timezone))
	{
		#ifdef DEBUG_ENABLE
			DBG("Year:", year, "\tMonth:", month, "\tDay:", day);
			DBG("Hour:", hour, "\tMinute:", minute, "\tSecond:", sec);
			DBG("Timezone:", timezone);
		#endif
		setTime(hour,minute,sec,day,month,year); // set time to Saturday 8:29:00am Jan 1 2011
	}
	else
	{
		#ifdef DEBUG_ENABLE
			DBG("Couldn't get network time");
		#endif
		return SIMCOM_FAILED;
	}
	
	return SIMCOM_SUCCESS;
}
void update_gps_data()  //Vinay 21-07-2023
{
   ram_data.lat = (gps_data.lat * 1000000L);
   ram_data.lon = gps_data.lon * 1000000L;
   ram_data.speed = gps_data.speed * 10;  
   ram_data.alt = gps_data.alt * 10;     
   ram_data.vsat = gps_data.vsat;     
   ram_data.usat  = gps_data.usat;   
   ram_data.accuracy = gps_data.accuracy * 10; 
   //ram_data.year = gps_data.year;   
   //ram_data.month = gps_data.month;   
   //ram_data.day = gps_data.day;    
   //ram_data.hour = gps_data.hour;    
   //ram_data.min = gps_data.min;     
   //ram_data.sec = gps_data.sec;
#ifdef DEBUG_RAM_DATA
   SerialMon.print("ram.lat:");
   SerialMon.println(ram_data.lat);
#endif

#ifdef DEBUG_GPS_DATA
   SerialMon.print("gps.lat:");
   SerialMon.println(gps_data.lat,6); 
#endif

}


uint8_t check_RTC_time()
{
  //temp_time.year= rtc.getYear();
  if (((rtc.getYear() >= DEFAULT_YEAR) && (rtc.getYear() <= (DEFAULT_YEAR + YEAR_OFFSET))) &&
      (((rtc.getMonth()+1)>= 1) && ((rtc.getMonth()+1) <= 12)) &&
      ((rtc.getDay() >= 1) && (rtc.getDay() <= 31)) &&
      ((rtc.getHour(true) >= 0) && (rtc.getHour(true) <= 23)) &&
      ((rtc.getMinute() >= 0) && (rtc.getMinute() <= 59)) &&
      ((rtc.getSecond() >= 0) && (rtc.getSecond() <= 59)))
      {
        //time_main.RTC_flag =true;
#ifdef DEBUG_FLAG
        Serial.println("RTC_flag=true");
        Serial.print(rtc.getYear());
#endif

        return correct;
      }
   Serial.println("RTC_flag=false");
   return incorrect;
}
void check_GPS_time(gps_date_time_t* date_time)
{
  time_stamp_t temp_time;
  if (((date_time->day <= 0) || (date_time->day > 31)) ||
  ((date_time->month <= 0) || (date_time->month > 12)) ||
  ((date_time->year < DEFAULT_YEAR) || (date_time->year > (DEFAULT_YEAR + YEAR_OFFSET)))||      // Assuming that RTC will never go below 2016.
  ((date_time->hour < 0) || (date_time->hour > 23)) ||
  ((date_time->min < 0) || (date_time->min > 59)) ||
  ((date_time->sec < 0) || (date_time->sec > 59)))
  {
  }
  else
  {
    temp_time.yy = (date_time->year)%100;
    temp_time.mm = date_time->month;
    temp_time.dd = date_time->day;
    temp_time.hr = date_time->hour;
    temp_time.min = date_time->min;
    temp_time.sec = date_time->sec;
    
    temp_time = utcTOlocal(temp_time);
    
    if (((temp_time.yy >= (DEFAULT_YEAR)%100) && (temp_time.yy <= ((DEFAULT_YEAR)%100) + YEAR_OFFSET)) &&
    ((temp_time.mm >= 1) && (temp_time.mm <= 12)) &&
    ((temp_time.dd >= 1) && (temp_time.dd <= 31)) &&
    ((temp_time.hr >= 0) && (temp_time.hr <= 23)) &&
    ((temp_time.min >= 0) && (temp_time.min <= 59)) &&
    ((temp_time.sec >= 0) && (temp_time.sec <= 59)))
    {
      date_time->year = temp_time.yy;
      date_time->month = temp_time.mm;
      date_time->day = temp_time.dd;
      date_time->hour = temp_time.hr;
      date_time->min = temp_time.min;
      date_time->sec = temp_time.sec;
      
      //make gps time aval flag TRUE

      gps_data.GPS_flag = true;
      
#ifdef DEBUG_FLAG
      Serial.println("GPS_flag=true");
#endif
    }
    else
    {

      gps_data.GPS_flag = false;
      
#ifdef DEBUG_FLAG
      Serial.print("GPS_flag=false");
#endif
    }
  }
}
time_stamp_t utcTOlocal(time_stamp_t timeT)   
{
  //time_stamp_t localTime;
  int DayNum;
  
  timeT.yy = timeT.yy;
  timeT.mm = timeT.mm; 
  timeT.dd = timeT.dd;
  timeT.hr = timeT.hr + 5;
  timeT.min = timeT.min + 30;
  timeT.sec = timeT.sec;

#ifdef DEBUG_UTC_TO_LOCAL
  Serial.print("G.Y+");
  Serial.println(timeT.yy);
  Serial.print("G.M+");
  Serial.println(timeT.mm);
  Serial.print("G.D+");
  Serial.println(timeT.dd);
  Serial.print("G.hr+");
  Serial.println(timeT.hr);
  Serial.print("G.Mn+");
  Serial.println(timeT.min);
  Serial.print("G.sc+");
  Serial.println(timeT.sec);
#endif

  if( timeT.min > 59)
  {
    int m;
    m = ( timeT.min/ 60);
    timeT.hr = ( timeT.hr + m);
    timeT.min = (timeT.min % 60);
  }
  if( timeT.hr > 23)
  {
    int h;
    h = ( timeT.hr / 24);
    timeT.dd = ( timeT.dd + h);
    timeT.hr = ( timeT.hr % 24);
  }
  if(( timeT.mm == 1) || (timeT.mm == 3) || (timeT.mm == 5) || (timeT.mm == 7) || (timeT.mm == 8) || (timeT.mm == 10) || (timeT.mm == 12))
  {
    DayNum = 31;
  }
  if((timeT.mm == 4) || (timeT.mm == 6) || (timeT.mm == 9) || (timeT.mm == 11))
  {
    DayNum = 30;
  }
  if(timeT.mm == 2)
  {
      if (( timeT.yy % 4) == 0)
      {
          if(( timeT.yy % 100) == 0)
          {
              if(( timeT.yy % 400) == 0)
              {
                  DayNum = 29;
              }
              else
            {
              DayNum = 28;
            }
          }
          else
        {
          DayNum = 29;
        }
      }
      else
    {
      DayNum = 28;
    }
    /*if(((timeT.yr % 4) == 0) && ((timeT.yr % 100) == 0) || (timeT.dd % 400) ==0)
    {
      DayNum = 29;
    }
    else
    {
      DayNum = 28;
    }*/
  }
  if( timeT.dd > DayNum)
  {
    int d;
    d = ( timeT.dd / DayNum);
    timeT.mm = (timeT.mm + d);
    timeT.dd = (timeT.dd % DayNum);
  }
  if(timeT.mm > 12)
  {
    int M;
    M = (timeT.mm / 12);
    timeT.yy = ( timeT.yy + M);
    timeT.mm = (timeT.mm % 12);
  }
#ifdef DEBUG_UTC_TO_LOCAL
  Serial.print("year=");
  Serial.println(timeT.yy);
  Serial.print("month=");
  Serial.println(timeT.mm);
  Serial.print("day=");
  Serial.println(timeT.dd);
  Serial.print("hour=");
  Serial.println(timeT.hr);
  Serial.print("min=");
  Serial.println(timeT.min);
  Serial.print("sec=");
  Serial.println(timeT.sec);
#endif
  
  return timeT;
  
}
