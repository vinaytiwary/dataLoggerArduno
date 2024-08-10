/*
 * Data_logger_v1.ino
 *
 * Created: 7/3/2022 4:45:52 PM
 * Author: rohitsoni
 */

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Functions.h"
#include "Hardware_pins.h"
#include "Config.h"
#include "timer.h"
#include "schedular.h"
#include "common.h"
#include "debug.h"
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <TimeAlarms.h>
#include "Switch.h"
#include "Adafruit_MCP23017.h"

#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
PubSubClient mqtt(client);
Switch SWITCH_ONOFF = Switch(ESP_SWITCH, INPUT_PULLUP, LOW, 50, 2000, 250);

bool toggle10ms = false;
bool toggle50ms = false;
bool toggle100ms = false;
bool toggle1sec = false;
extern schedular_flg_t schedular_flg;
extern e2p_date_t e2p_date;

void setup()
{
	get_reset_reason();
	SerialMon.begin(115200);
	delay(100);
#ifdef DEBUG_ENABLE
	SerialMon.println(F("Processor Booting up.."));
	SerialMon.println(SOFTWARE_VERSION);
	SerialMon.printf("Last reset reason = %d\n", reset_reason);
#endif

	init_gpio();
	// read_eeprom();
	indicate_led(1, 1, 1, 500, 0);
	init_acc();
	init_sdcard();

	set_rtc_time();

	can_setup();
	init_timer();
	SWITCH_ONOFF.setLongPressCallback(&switchcallback, (void *)"lng press");
	device_state = DEV_ON;
	modem_rf_pwr_stat = DEV_OFF;

	// delay(100);
	// simcom_power_on();
	// delay(100);
	// init_modem();
	// delay(100);
	// enable_gps();
	delay(100);
	init_modem();
	// delay(15000);
	// enable_gps();
#ifdef DEBUG_ENABLE
	SerialMon.print(F("Dev state:"));
	SerialMon.println(device_state);
#endif
	if (read_flash_config() != RETURN_OK)
		write_defaults();

	read_eeprom();
}

void timer_sec()
{
	static const unsigned long REFRESH_INTERVAL = 100; // ms
	static unsigned long lastRefreshTime = 0;
	if (millis() - lastRefreshTime >= REFRESH_INTERVAL)
	{
		lastRefreshTime += REFRESH_INTERVAL;
		get_gps_location();
	}
}

void loop()
{
	// timer_sec();

	if (schedular_flg.flg_10ms == true)
	{
		schedular_flg.flg_10ms = false;
	}
	if (schedular_flg.flg_50ms == true)
	{
		schedular_flg.flg_50ms = false;
		// setup_connect_network();
		get_gps_location(); // VINAY 14/07/2023
	}
	if (schedular_flg.flg_100ms == true)
	{
		schedular_flg.flg_100ms = false;
		update_gps_data();
		// update_sd_card();
		// prepare_data();
		// update_sd_card();
	}
	if (schedular_flg.flg_1sec == true)
	{
		schedular_flg.flg_1sec = false;
		prepare_data();
		update_sd_card();
	}
	/*
	   switch (device_state)
	   {
		   case DEV_ON:
		   {
			   if (modem_rf_pwr_stat==DEV_OFF)
			   {

				   if (init_modem() != SIMCOM_SUCCESS)
				   {
					   indicate_error(ERROR_SIMCARD);
					   return ;
				   }

				   if (setup_connect_network() != SIMCOM_SUCCESS)
				   {
					   indicate_error(ERROR_NETWORK);
					   return ;
				   }

				   //init_mqtt();
				   //mqtt.loop();
				   enable_gps();
				   sync_modem_time();
				   activate_alarms();
				   modem_rf_pwr_stat=DEV_ON;
				   no_of_errors=0;
			   }
			   static uint16_t mqtt_maintain=0,time_sync=0;
			   if (mqtt_maintain++>MQTT_MAINTAIN_TIME)
			   {
				   mqtt_maintain=0;
				   mqtt_stat=manage_mqtt_loop();
			   }
			   if (time_sync++>TIME_SYNC_TIME)
			   {
				   time_sync=0;
				   sync_modem_time();
			   }
			   timer_sec();
			   handle_led_status();
		   }break;

		   case DEV_OFF:
		   {
			   if(millis()>shdn_cmd_init_millis+SWITCH_HLD_PWRON)board_shutdown(1);

		   }break;
		   case DEV_CHARGE_ONLY:
		   {
			   if (modem_rf_pwr_stat==DEV_OFF)
			   {
				   simcom_power_off();
				   static uint8_t count=0;

				   if (mcp_digitalRead(MCP_TP_CHARGING) )
				   {
					   count++;
					   delay(10);
				   }
				   else count=0;
				   if (count>100)
				   {
					   #ifdef DEBUG_ENABLE
					   SerialMon.println(F("Device not charging. Going to sleep"));
					   #endif
					   board_shutdown(1);
				   }

			   }

		   }break;
		   case DEV_REBOOT:
		   {
			   simcom_power_off();
			   device_state=DEV_ON;
			   modem_rf_pwr_stat=DEV_OFF;
		   }break;
	   }
	   handle_led_status();
	 //get_gps_location();
	   Alarm.delay(1); // wait one msecond
	   SWITCH_ONOFF.poll();
	   acc_loop() ;
	   can_loop();
	   */
}

void handle_led_status()
{
	static uint16_t led_stat = 0, led_count = 0;
	if (led_stat)
	{
		if (led_count++ > LED_ON_TIME)
		{
			indicate_led(0, 0, 0, 0, 0);
			led_count = 0;
			led_stat = 0;
		}
	}
	else
	{
		if (led_count++ > LED_OFF_TIME)
		{

			if (received_mqtt_packet == 1)
				indicate_led(0, 1, 1, 0, 0); // cyan led on rf packet
			else if (!mcp_digitalRead(MCP_TP_CHARGING))
			{
				indicate_led(1, 1, 0, 0, 0); // charging
#ifdef DEBUG_ENABLE
				SerialMon.println(F("Device is charging.."));
#endif
				is_board_charging = 1;
			}
			else if (mcp_digitalRead(MCP_TP_CHARGING))
			{
				indicate_led(0, 0, 1, 0, 0); // charging full
#ifdef DEBUG_ENABLE
				SerialMon.println(F("Device is not charging .."));
#endif
				is_board_charging = 0;
				// toggle_tp4056();
			}

			else // server connection status
			{
				if (mqtt_stat)
					indicate_led(0, 1, 0, 0, 0);
				else
					indicate_led(1, 0, 0, 0, 0);
				is_board_charging = 0;
			}
			led_count = 0;
			led_stat = 1;
			received_mqtt_packet = 0;
		}
	}
}

uint8_t switch_poweron()
{
	uint16_t i = 0;
	while (!digitalRead(ESP_SWITCH))
	{
		SWITCH_ONOFF.poll();
		if (device_state == DEV_ON)
			break;
	}

	while (!digitalRead(ESP_SWITCH))
	{
		indicate_led(0, 1, 0, 100, 0);
	}
	if (device_state == DEV_ON)
		return RETURN_OK;
	else
		return RETURN_FAIL;
}

void switchcallback(void *s)
{
#ifdef DEBUG_ENABLE
	SerialMon.println("Switch long pressed");
#endif

	Disable_alarms();
	if (device_state == DEV_CHARGE_ONLY)
	{
		indicate_led(0, 1, 0, 10, 0);
	}

	if (device_state == DEV_ON)
		device_state = DEV_OFF;
	else
		device_state = DEV_ON;

	if (device_state == DEV_OFF)
	{
		indicate_led(1, 1, 1, 10, 0);
	}
	if (device_state == DEV_OFF && !mcp_digitalRead(MCP_TP_CHARGING))
	{
		device_state = DEV_CHARGE_ONLY;
		modem_rf_pwr_stat = DEV_OFF;
	}
	else if (device_state == DEV_OFF)
		board_shutdown(1);
}

void indicate_error(uint8_t err_code)
{
	switch (err_code)
	{
	case ERROR_SIMCARD:
	{
		for (char i = 0; i < 10; i++)
		{
			indicate_led(1, 0, 0, 500, 500);
		}
		board_shutdown(1);
	}
	break;
	case ERROR_NETWORK:
	{
		for (char i = 0; i < 10; i++)
		{
			indicate_led(1, 0, 0, 100, 500);
		}
		device_state = DEV_REBOOT;
	}
	break;
	case ERROR_MODEM_PWR:
	{

		for (char i = 0; i < 10; i++)
		{
			indicate_led(1, 0, 0, 900, 100);
		}
		device_state = DEV_REBOOT;
	}
	break;
	case ERROR_INTERNET:
	{

		indicate_led(1, 0, 0, 1000, 0);
		device_state = DEV_REBOOT;
	}
	break;
	case ERROR_RF_MODULE:
	{

		for (char i = 0; i < 10; i++)
		{
			indicate_led(1, 0, 0, 250, 250);
			indicate_led(0, 1, 0, 250, 250);
		}
		board_shutdown(1);
	}
	break;
	}
	no_of_errors++;
	if (no_of_errors > MAX_ERROR_BEFORE_SLEEP)
	{
		indicate_led(1, 0, 0, 1000, 0);
		indicate_led(0, 1, 0, 1000, 0);
		indicate_led(0, 0, 1, 1000, 0);
		board_shutdown(1);
	}
}
