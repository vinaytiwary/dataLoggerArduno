#include <EEPROM.h>


uint32_t lastReconnectAttempt = 0;



uint8_t init_mqtt()
{
	#ifdef DEBUG_ENABLE
	SerialMon.println(F("Connecting to MQTT server"));
	#endif
	mqtt.setServer(broker, 1883);
	mqtt.setCallback(mqttCallback);
	delay(500);
	create_pub_sub_strings();
	mqtt_subscribe();
	return RETURN_OK;
}

boolean mqttConnect()
{
	if (setup_connect_network() != SIMCOM_SUCCESS)
	{
		#ifdef DEBUG_ENABLE
		SerialMon.println("Connection to GPRS failed");
		#endif
		return SIMCOM_FAILED;
	}
	#ifdef DEBUG_ENABLE
	SerialMon.print("Connecting to ");
	SerialMon.println(broker);
	#endif

	// Or, if you want to authenticate MQTT:
	//boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
	
	char imei_str[30] ;
	imei.toCharArray(imei_str, imei.length()+1);
	
	if (mqtt.connect(imei_str) == false)
	{
		#ifdef DEBUG_ENABLE
		SerialMon.println(F("MQTT connection failed"));
		#endif
		return SIMCOM_FAILED;
	}
	#ifdef DEBUG_ENABLE
	SerialMon.println(F("MQTT connected"));
	#endif
	mqtt_subscribe();
	return mqtt.connected();
}


void mqttCallback(char* topic, byte* payload, unsigned int len)
{
	#ifdef DEBUG_ENABLE
		SerialMon.print("Message arrived [");
		SerialMon.print(topic);
		SerialMon.print("]: ");
		SerialMon.write(payload, len);
		SerialMon.println();
	#endif
	
	String inc_topic= String(topic);

	if (inc_topic.substring(0,subs_gateway_cfg.length())==subs_gateway_cfg) manage_subs_gateway_cfg(topic, payload,  len);
	
	if (gateway_prov_status==DEV_PROVISIONED)
	{
	 if (inc_topic.substring(0,subs_power.length())==subs_power) manage_power(topic, payload,  len);
	}
	
	received_mqtt_packet=1;
	
}

void manage_subs_gateway_cfg(char* topic, byte* payload, unsigned int len)
{
	#ifdef DEBUG_ENABLE
		SerialMon.println("subs_gateway_cfg");
	#endif
	String inc_payload= String((char*)payload);
	inc_payload[len]='\0';
	String temp; 
	uint8_t temp_8bit=0;
	//uint16_t temp_16bit=0;
	
	int ind1 =		inc_payload.indexOf(',');
	int ind2 =		inc_payload.indexOf(',', ind1+1 );
	int ind3 =		inc_payload.indexOf(',', ind2+1 );
	int ind4 =		inc_payload.indexOf(',', ind3+1 );
	int ind5 =		inc_payload.indexOf(',', ind4+1 );
	int ind6 =		inc_payload.indexOf(',', ind5+1 );
	#ifdef DEBUG_ENABLE
		SerialMon.printf("IDX1:%d,IDX2:%d,IDX3:%d,IDX4:%d,IDX5:%d,IDX5:%d\n",ind1,ind2,ind3,ind4,ind5);
	#endif
	temp =	inc_payload.substring(0, ind1);   //captures first data String
	temp_8bit=temp.toInt();
	#ifdef DEBUG_ENABLE
		SerialMon.println(temp);
	#endif
	if (temp_8bit==DEV_NOT_PROVISIONED || temp_8bit==DEV_PROVISIONED)
	{
		gateway_prov_status= temp_8bit;
		#ifdef DEBUG_ENABLE
			SerialMon.printf("Provision status = %d saved\n",gateway_prov_status);
		#endif
		if (EEPROM.read(ADDR_PROV_STAT) !=gateway_prov_status)
		{
			EEPROM.write(ADDR_PROV_STAT,gateway_prov_status);
			EEPROM.commit();
		}
		
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println(F("Invalid provision status.. discarded"));
		#endif
	}
	
	temp =	inc_payload.substring(ind1+1, ind2);
	temp_8bit= temp.toInt();
	#ifdef DEBUG_ENABLE
		SerialMon.println(temp);
	#endif
	if (temp_8bit==0 || temp_8bit==1)
	{
		server_requested_config= temp_8bit;
		#ifdef DEBUG_ENABLE
			SerialMon.printf("server config req = %d\n",server_requested_config);
		#endif
		
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println(F("Invalid server req config.. discarded"));
		#endif
	}
	
	
	temp = inc_payload.substring(ind2+1, ind3);
	temp_8bit= temp.toInt();
	#ifdef DEBUG_ENABLE
		SerialMon.println(temp);
	#endif
	if (temp_8bit>=GPS_TIME_MIN && temp_8bit<=GPS_TIME_MAX)
	{
		gps_upload_time= temp_8bit;
		#ifdef DEBUG_ENABLE
			SerialMon.printf("GPS upload time = %d\n",gps_upload_time);
		#endif
		EEPROM.write(ADDR_GPS_TIME,gps_upload_time);
		EEPROM.commit();
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println(F("Invalid GPS upload time.. discarded"));
		#endif
	}
	
	
	
	temp =	inc_payload.substring(ind3+1,ind4);
	temp_8bit= temp.toInt();
	#ifdef DEBUG_ENABLE
		SerialMon.println(temp);
	#endif
	if (temp_8bit>=HEALTH_TIME_MIN && temp_8bit<=HEALTH_TIME_MAX)
	{
		hlth_upload_time= temp_8bit;
		#ifdef DEBUG_ENABLE
			SerialMon.printf("Health upload time = %d\n",hlth_upload_time);
		#endif
		EEPROM.write(ADDR_HEALTH_TIME,hlth_upload_time);
		EEPROM.commit();
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println(F("Invalid Health upload time.. discarded"));
		#endif
	}
	

	if (server_requested_config==1)
	{
		server_requested_config=0;  
		String device_config= String(gateway_prov_status) +","+ String(server_requested_config) +","+ String(gps_upload_time) +","+  String(hlth_upload_time); 
		char topic[100] , data[150];
		pub_cfg.toCharArray(topic, pub_cfg.length()+1);
		device_config.toCharArray(data, device_config.length()+1);
		
		
		#ifdef DEBUG_ENABLE
			SerialMon.printf("Sending config to server on topic : %s , data : %s\n",topic,data);
		#endif
		mqtt.publish(topic,data);
	}
	activate_alarms();
}



void manage_power(char* topic, byte* payload, unsigned int len)
{
	#ifdef DEBUG_ENABLE
		SerialMon.println("Subs_power_control");
	#endif
	String inc_payload= String((char*)payload);
	inc_payload[len]='\0';
	uint8_t power_ctrl= inc_payload.toInt();
	
	if (power_ctrl==0) board_shutdown(1);
}



uint8_t manage_mqtt_loop()
{
	if (!mqtt.connected())
	{
		static uint8_t count=0;
		is_mqtt_connected=0;
		uint32_t t = millis();
		if (t - lastReconnectAttempt > 1000L)
		{
			#ifdef  DEBUG_ENABLE
				SerialMon.println(F("Reconnecting MQTT"));
			#endif
			lastReconnectAttempt = t;
			if (mqttConnect()!=SIMCOM_FAILED)
			{
				count=0;
				lastReconnectAttempt = 0;
			}
		}
		if (count++>5)
		{
			count=0;
			device_state=DEV_REBOOT;
		}
		return RETURN_FAIL;
	}
	
	mqtt.loop();
	is_mqtt_connected=1;
	if (prev_power_state==0)
	{
		publish_power_state(1);
		prev_power_state=1;
	}
	return RETURN_OK;
}


void create_pub_sub_strings()
{
	// topics to subscribe
	subs_gateway_cfg =baseURL+'/'+imei+"/GATEWAY/CONFIG/UPLOAD";
	
	// topics to publish
	pub_prov =baseURL+'/'+imei;
	pub_gps =baseURL+'/'+imei+"/GATEWAY/LAC";
	pub_cfg =baseURL+'/'+imei+"/GATEWAY/CONFIG/DOWNLOAD";
	pub_health =baseURL+'/'+imei+"/GATEWAY/HEALTH";
	pub_power =baseURL+'/'+imei+"/GATEWAY/DOWNLOAD/POWER";

}
void mqtt_subscribe()
{
	char topic[150];
	
	#ifdef DEBUG_ENABLE
		SerialMon.println("Subscribing to following:");
	#endif
	memset(topic,'0',sizeof(topic));
	//subs_gateway_cfg+="/#";
	subs_gateway_cfg.toCharArray(topic, subs_gateway_cfg.length()+1);
	uint8_t strln=strlen(topic);
	topic[strln]='/';
	topic[strln+1]='#';
	topic[strln+2]='\0';
	mqtt.subscribe(topic);
	#ifdef DEBUG_ENABLE
		SerialMon.println(topic);
	#endif
	
	if (gateway_prov_status==DEV_PROVISIONED)
	{
		
		memset(topic,'0',sizeof(topic));
		subs_power.toCharArray(topic, subs_power.length()+1);
		strln=strlen(topic);
		topic[strln]='/';
		topic[strln+1]='#';
		topic[strln+2]='\0';
		mqtt.subscribe(topic);
		#ifdef DEBUG_ENABLE
			SerialMon.println(topic);
		#endif
	}
	
	
	
}

void publish_power_state(uint8_t state)
{
	if (is_mqtt_connected==1)
	{
		char topic[100] ;
		pub_power.toCharArray(topic, pub_power.length()+1);
		#ifdef DEBUG_ENABLE
			SerialMon.printf("Sending Power state to server on topic : %s , data : %d\n",topic,state);
		#endif
		if (state)mqtt.publish(topic,"1");
		else mqtt.publish(topic,"0");
		delay(1000);
	}
	else
	{
		#ifdef DEBUG_ENABLE
			SerialMon.println("Cannot send power state to server");
		#endif
	}
	
}