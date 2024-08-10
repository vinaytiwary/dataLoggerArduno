/*
 * Hardware_pins.h
 *
 * Created: 06-02-2021 5.27.23 PM
 *  Author: Asus
 */ 


#ifndef HARDWARE_PINS_H_
#define HARDWARE_PINS_H_


#define ESP_DGB_TX				1
#define ESP_DGB_RX				3

#define ESP_SIMCOM_TX			13	
#define ESP_SIMCOM_RX			4	

#define MCP_PWR_LTCH			0
#define MCP_RS485_DIR			1
#define MCP_IO_IN				2
#define MCP_IO_OUT				3
#define MCP_CAN_SO				4
#define MCP_TP_CHARGING		5
#define MCP_TP_CH_EN			6
#define MCP_FREE				  7
#define MCP_SIM_PWREN			8
#define MCP_SIM_DTR				9
#define MCP_LED_GREEN			10
#define MCP_LED_BLUE			11
#define MCP_LED_RED				12
#define MCP_SIM_1V8				13	
#define MCP_SIM_ONOFF			14	
#define MCP_SIM_RST				15	

#define ESP_RS485_TX			21
#define ESP_RS485_RX			22
#define ESP_RS232_TX			12
#define ESP_RS232_RX			27

#define	ESP_MISO				19
#define	ESP_MOSI				23
#define	ESP_SCK					18
#define	ESP_SD_CS				5
#define	ESP_NRF_CS				32
#define	ESP_NRF_CE				33

#define ESP_SWITCH				0
#define ESP_PWREN				14
#define ESP_SDA					15
#define ESP_SCL					2

#define ESP_ADC_BATTIN			36
#define ESP_ADC_ACC				39
#define ESP_ADC_LVL				34

#define ESP_ACCL_INT			35

#define ESP_CAN_TX				25
#define ESP_CAN_RX				26

#endif /* HARDWARE_PINS_H_ */
