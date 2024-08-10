
// Basic demo for tap/doubletap readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 80

uint8_t init_acc(void) 
{
	if (! lis.begin(0x18)) 
	{  
		#ifdef DEBUG_ENABLE
			SerialMon.println("Accelerometer Init error");
		#endif
		
		return RETURN_FAIL;
		
	}
	#ifdef DEBUG_ENABLE
		SerialMon.println("LIS3DH found!");
	#endif
	lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!


	// 0 = turn off click detection & interrupt
	// 1 = single click only interrupt output
	// 2 = double click only interrupt output, detect single click
	// Adjust threshhold, higher numbers are less sensitive
	lis.setClick(2, CLICKTHRESHHOLD); //. accelerometer interrupt threshold
	delay(100);
	return RETURN_OK;
}


void acc_loop() 
{
	uint8_t click = lis.getClick();
	if (click == 0) return;
	if (! (click & 0x30)) return;
	SerialMon.print("Click detected (0x"); SerialMon.print(click, HEX); SerialMon.print("): ");
	if (click & 0x10) SerialMon.print(" single click");
	if (click & 0x20) SerialMon.print(" double click");
	SerialMon.println();

	delay(100);
	return;
}