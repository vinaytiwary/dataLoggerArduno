/*
 * Config.h
 *
 * Created: 07-02-2021 3.11.34 PM
 *  Author: Asus
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_
#include <SoftwareSerial.h>


#define RS485_DIR_DELAY	10 //MS

char SOFTWARE_VERSION[]= "V1.0";

//Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
#define  SerialMon Serial  //(ESP_DGB_RX, ESP_DGB_TX); //RX TX
//#define SerialAT Serial1//(ESP_SIMCOM_RX, ESP_SIMCOM_TX); //RX TX
HardwareSerial SerialAT(1);//ESP_RS485_RX, ESP_RS485_TX); //RX TX


//SoftwareSerial SerialRF(47,46);//PA6, PA7); // RX, TX //12,11
//HardwareSerial SerialRF(HC12_RX, HC12_TX); //RX TX

// modem config
String imei;
#define TINY_GSM_MODEM_SIM7600

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG				SerialMon



// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS			true
#define TINY_GSM_USE_WIFI			false

// set GSM PIN, if any
#define GSM_PIN						""


#define TINY_GSM_USE_GPRS			true
#define TINY_GSM_USE_WIFI			false

// Your GPRS credentials, if any
const char apn[]					="airtelgprs.com";
const char gprsUser[]				="";
const char gprsPass[]				="";

// MQTT details
const char* broker					="150.129.239.115";

String baseURL						="mqttbase";

#define SIMCOM_SUCCESS				1
#define SIMCOM_FAILED				0


// debug enable
//#define DEBUG_ENABLE				1

#define RETURN_OK					1
#define RETURN_FAIL					0
#define DEVICE_NOT_FOUND			-1

#define LED_ON_TIME					100
#define LED_OFF_TIME				2000


#define TIME_SYNC_TIME				10000
#define MQTT_MAINTAIN_TIME			10
// flash settings

#define ADDR_PROV_STAT				0X01
#define ADDR_GPS_TIME				0x02
#define ADDR_HEALTH_TIME			0x03


#define DEF_PROV_STATE				0
#define GPS_UPLOAD_DEF				2 // MINUTES
#define HLTH_UPLOAD_DEF				2 // MINUTES

#define GPS_TIME_MIN				1	// 1 MIN
#define GPS_TIME_MAX				240 // 4 HOURS

#define HEALTH_TIME_MIN				1	// 1 MIN
#define HEALTH_TIME_MAX				240 // 4 HOURS





#define WIRED_PACKET_SEND_DELAY		50


#define WIRED_BAUD					2400

#define DEV_PROVISIONED				1
#define DEV_NOT_PROVISIONED			0


#define CMD_PROV					0X01
#define CMD_POWER					0X02
#define CMD_REPORT					0X03
#define CMD_CFG 					0X04




#define PRIORITY_COMMAND			1
#define NON_PRIORITY_COMMAND		0


#define EEPROM_SIZE	100
//#define EMIC_PRESENT 0

// variables


uint8_t gps_upload_time=GPS_UPLOAD_DEF;
uint8_t hlth_upload_time=HLTH_UPLOAD_DEF;
uint8_t is_mqtt_connected=0;
uint8_t gateway_prov_status=DEV_NOT_PROVISIONED;
uint8_t server_requested_config =0;
uint8_t mqtt_stat=0;
uint8_t received_mqtt_packet=0;

// topics to subscribe
String subs_gateway_cfg ;

String subs_power ;

// topics to publish

String pub_prov ;
String pub_gps ;
String pub_cfg ;
String pub_power ;	
String pub_health ;


#define DEV_OFF				0
#define DEV_ON				1
#define DEV_CHARGE_ONLY		2
#define DEV_REBOOT			3

uint8_t reset_reason = 0;
uint8_t is_board_charging=0;
uint8_t device_state=DEV_OFF;
uint8_t modem_rf_pwr_stat=DEV_OFF;
uint32_t shdn_cmd_init_millis=0;
uint8_t prev_power_state=0;

uint8_t no_of_errors=0;
#define MAX_ERROR_BEFORE_SLEEP	5
#define SWITCH_HLD_PWRON	1000 // MS

#define RESET_REASON_UNKNOWN	0
#define RESET_REASON_CHARGE		1
#define RESET_REASON_SWITCH		2
#define RESET_REASON_WDT		3
#define RESET_REASON_POWERON	4
#define RESET_REASON_RTC		5


#define ERROR_SIMCARD			1
#define ERROR_NETWORK			2
#define ERROR_MODEM_PWR			3
#define ERROR_INTERNET			4
#define ERROR_RF_MODULE			5

#define WIRED_MODE_TX				1
#define WIRED_MODE_RX				0

#endif /* CONFIG_H_ */
