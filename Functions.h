/*
 * Functions.h
 *
 * Created: 14-02-2021 11.07.19 AM
 *  Author: Asus
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "common.h"

//handle_gpio
void init_gpio();
void init_sdcard();
void indicate_led(int red, int green, int blue, uint16_t delay_on, uint16_t delay_off);

//handle_eic
uint8_t init_emic();

//handle_modem

uint8_t simcom_power_on();
void simcom_power_reset();
uint8_t init_modem();
uint8_t setup_connect_network();
uint8_t get_gps_location();
void update_gps_data();
uint8_t sync_modem_time();
uint8_t get_modem_imei();
void enable_gps();
void disable_gps();
 void timer_1sec();
uint8_t mcp_digitalRead(uint8_t pin);
void mcp_digitalWrite(uint8_t pin,uint8_t mode);
void mcp_pinMode(uint8_t pin,uint8_t mode);
// alarm functions
void activate_alarms();
void Repeats();
void set_rtc_time();
time_stamp_t utcTOlocal(time_stamp_t);


// flash functions
uint8_t read_flash_config();
void    write_defaults();




// handle mqtt

void manage_subs_gateway_cfg(char* topic, byte* payload, unsigned int len);

char getChecksum(unsigned char* buff, char len);
void read_eeprom();
void write_eeprom();
void date_change_e2p();

uint8_t init_mqtt();
#endif /* FUNCTIONS_H_ */
