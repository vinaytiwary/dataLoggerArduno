/*
#include <driver/twai.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


// define your CAN messages here, OR you can define them locally...
// standard 11-bit frame ID = 0
// extended 29-bit frame ID = 1
// format:   can_message_t (name of your message) = {std/ext frame, message ID, message DLC, {data bytes here}};

can_message_t myMessageToSend = {0, 0x123, 8, {0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x99}};

uint32_t previousMillis;
const uint32_t interval = 1000;

//=================================================

void setup_can_driver()
{
	can_general_config_t general_config = {
		.mode = CAN_MODE_NORMAL,
		.tx_io = (gpio_num_t)ESP_CAN_TX,
		.rx_io = (gpio_num_t)ESP_CAN_RX,
		.clkout_io = (gpio_num_t)CAN_IO_UNUSED,
		.bus_off_io = (gpio_num_t)CAN_IO_UNUSED,
		.tx_queue_len = 100,
		.rx_queue_len = 65,
		.alerts_enabled = CAN_ALERT_NONE,
	.clkout_divider = 0};
	can_timing_config_t timing_config = CAN_TIMING_CONFIG_500KBITS();
	can_filter_config_t filter_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
	esp_err_t error;

	error = can_driver_install(&general_config, &timing_config, &filter_config);
	if (error == ESP_OK)
	{
		Serial.println("CAN Driver installation success...");
	}
	else
	{
		Serial.println("CAN Driver installation fail...");
		return;
	}

	// start CAN driver
	error = can_start();
	if (error == ESP_OK)
	{
		Serial.println("CAN Driver start success...");
	}
	else
	{
		Serial.println("CAN Driver start FAILED...");
		return;
	}
}

//=======================================================================

void can_loop()
{
	can_message_t rx_frame;
	if (can_receive(&rx_frame, pdMS_TO_TICKS(1000)) == ESP_OK)
	{
		//do whatever you need with your received CAN messages here
		// follow the can_message_t struct to learn how to decode/process the received frame.
	}
	
	if (millis() - previousMillis > interval)  // send out your CAN frame once every second
	{
		previousMillis = millis();
		can_transmit(&myMessageToSend, pdMS_TO_TICKS(1000));
	}
}

*/

// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

void can_setup() 
{
  Serial.println("CAN Receiver");


  mcp_digitalWrite(MCP_CAN_SO, LOW);
  //CAN.setPins(ESP_CAN_RX, ESP_CAN_TX);
  // start the CAN bus at 500 kbps

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
  }
  Serial.println("Starting CAN success!");
}



void can_loop() {
	// try to parse packet

	CAN.beginPacket(0x12);
	CAN.write('h');
	CAN.write('e');
	CAN.write('l');
	CAN.write('l');
	CAN.write('o');
	CAN.endPacket();

	//Serial.println("can packet send done");
	int packetSize = CAN.parsePacket();

	if (packetSize) {
		// received a packet
		Serial.print("Can packet Received ");

		if (CAN.packetExtended()) {
			Serial.print("extended ");
		}

		if (CAN.packetRtr()) {
			// Remote transmission request, packet contains no data
			Serial.print("RTR ");
		}

		Serial.print("packet with id 0x");
		Serial.print(CAN.packetId(), HEX);

		if (CAN.packetRtr()) {
			Serial.print(" and requested length ");
			Serial.println(CAN.packetDlc());
		}
		else {
			Serial.print(" and length ");
			Serial.println(packetSize);

			// only print packet data for non-RTR packets
			while (CAN.available()) {
				Serial.print((char)CAN.read());
			}
			Serial.println();
		}

		Serial.println();
	}
}
