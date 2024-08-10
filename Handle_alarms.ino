
#include <SparkFunBQ27441.h>
#include <TimeAlarms.h>
#include "Functions.h"
#include "config.h"

AlarmId id1,id2,id3,id4,id5;

void activate_alarms()
{
	Alarm.free(id1);
	Alarm.free(id2);
	#ifdef DEBUG_ENABLE
		SerialMon.println(F("Canceling and activating alarms.."));
	#endif
	id1=Alarm.timerRepeat(gps_upload_time*60, alarm_gps); // 60 IS TIME IN SECONDS
	id2=Alarm.timerRepeat(hlth_upload_time*60, alarm_health);
  id3=Alarm.timerRepeat(1,Repeats);

	
}
void Repeats()
{
  Serial.println("1 second timer");         
}

void Disable_alarms()
{
	Alarm.free(id1);
	Alarm.free(id2);
	#ifdef DEBUG_ENABLE
		SerialMon.println(F("Canceling alarms.."));
	#endif
}



void alarm_gps()
{
	if (is_mqtt_connected==1) // mqtt alive
	{
		
		/*float lat      = 0;
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
		
		
		if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,&year, &month, &day, &hour, &min, &sec ))
		{
			#ifdef DEBUG_ENABLE
			DBG("Latitude:", String(lat, 8), "\tLongitude:", String(lon, 8));
			DBG("Speed:", speed, "\tAltitude:", alt);
			DBG("Visible Satellites:", vsat, "\tUsed Satellites:", usat);
			DBG("Accuracy:", accuracy);
			DBG("Year:", year, "\tMonth:", month, "\tDay:", day);
			DBG("Hour:", hour, "\tMinute:", min, "\tSecond:", sec);
			#endif
		}
		else
		{
			#ifdef DEBUG_ENABLE
			SerialMon.println("Couldn't get GPS/GNSS/GLONASS location");
			#endif
			
		}*/
		 String gps_raw = modem.getGPSraw();
		 #ifdef DEBUG_ENABLE
			SerialMon.print(F("GPS/GNSS Based Location String:"));
			 SerialMon.println(gps_raw);
		 #endif
		 if(gps_raw.length()>20)
		 {
			 char topic[150] , data[200];
			 pub_gps.toCharArray(topic, pub_gps.length()+1);
			 gps_raw.toCharArray(data, gps_raw.length()+1);
			 mqtt.publish(topic,data);
		 }
		
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("GPS- MQTT not connected");
		#endif
	}
	
	
}
void alarm_health()
{
	if (is_mqtt_connected==1) // mqtt alive
	{
	//ESP_ADC_BATTIN
		uint8_t charge_stat=(!mcp.digitalRead(MCP_TP_CHARGING) );
		uint8_t csq=modem.getSignalQuality();
		String op_name=modem.getOperator();
		String health=  String(analogRead(ESP_ADC_BATTIN)) +","+String(analogRead(ESP_ADC_ACC)) +","+String(charge_stat) +","+  String(csq) +","+ String(op_name) ;//+","+ year +","+ hour +","+ min +","+sec;
		char topic[100] , data[100];
		pub_health.toCharArray(topic, pub_health.length()+1);
		health.toCharArray(data, health.length()+1);
		mqtt.publish(topic,data);
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("Health- MQTT not connected");
		#endif
	}
	
}
