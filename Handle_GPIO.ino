
#include "Hardware_pins.h"
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;
void init_mcp23017()
{
	Wire.begin(ESP_SDA, ESP_SCL);
	Scanner(); // for test purposes
	mcp.begin(0);
}

void init_gpio()
{
	pinMode(ESP_SDA,INPUT_PULLUP);
	pinMode(ESP_SCL,INPUT_PULLUP);
	pinMode(ESP_PWREN,OUTPUT);
	digitalWrite(ESP_PWREN,HIGH);
	delay(100);
	init_mcp23017();
	pinMode(ESP_MISO,INPUT);
	pinMode(ESP_MOSI,OUTPUT);
	pinMode(ESP_SCK,OUTPUT);
	pinMode(ESP_SD_CS,OUTPUT);
	digitalWrite(ESP_SD_CS,HIGH);
	//pinMode(ESP_NRF_CS,OUTPUT);
	//digitalWrite(ESP_NRF_CS,HIGH);
	//pinMode(ESP_NRF_CE,OUTPUT);
	//digitalWrite(ESP_NRF_CE,HIGH);
	pinMode(ESP_SWITCH,INPUT_PULLUP);
	pinMode(ESP_ACCL_INT,INPUT_PULLUP);

	mcp.digitalWrite(MCP_LED_GREEN,LOW);
	mcp.digitalWrite(MCP_LED_BLUE,LOW);
	mcp.digitalWrite(MCP_LED_RED,LOW);
	mcp.pinMode(MCP_LED_GREEN,OUTPUT);
	mcp.pinMode(MCP_LED_BLUE,OUTPUT);
	mcp.pinMode(MCP_LED_RED,OUTPUT);
	

	mcp.pinMode(MCP_PWR_LTCH,OUTPUT);
	mcp.pinMode(MCP_RS485_DIR,OUTPUT);
	mcp.pinMode(MCP_IO_IN,INPUT);
	mcp.pinMode(MCP_IO_OUT,INPUT);
	mcp.pinMode(MCP_CAN_SO,OUTPUT);
	mcp.pinMode(MCP_TP_CHARGING,INPUT);
	mcp.pinMode(MCP_TP_CH_EN,OUTPUT);
	mcp.digitalWrite(MCP_TP_CH_EN,HIGH);
	
	mcp.pinMode(MCP_SIM_PWREN,OUTPUT);
	mcp.pinMode(MCP_SIM_DTR,OUTPUT);
	mcp.pinMode(MCP_CAN_SO,OUTPUT);

	
	mcp.pinMode(MCP_SIM_1V8,INPUT);
	mcp.pinMode(MCP_SIM_ONOFF,OUTPUT);
	mcp.pinMode(MCP_SIM_RST,OUTPUT);
	
	#ifdef DEBUG_ENABLE
		SerialMon.println(F("GPIO init done.."));
	#endif
}
void mcp_pinMode(uint8_t pin,uint8_t mode)
{
	mcp.pinMode(pin,mode);
}
void mcp_digitalWrite(uint8_t pin,uint8_t mode)
{
	mcp.digitalWrite(pin,mode);
}
uint8_t mcp_digitalRead(uint8_t pin)
{
	return mcp.digitalRead(pin);
}


void dinit_gpio()
{
	mcp.digitalWrite(MCP_PWR_LTCH,LOW);
	pinMode(ESP_PWREN,ANALOG);
	pinMode(ESP_MISO,ANALOG);
	pinMode(ESP_MOSI,ANALOG);
	pinMode(ESP_SCK,ANALOG);
	pinMode(ESP_SD_CS,ANALOG);
	digitalWrite(ESP_SD_CS,HIGH);
	//pinMode(ESP_NRF_CS,ANALOG);
	//digitalWrite(ESP_NRF_CS,HIGH);
	//pinMode(ESP_NRF_CE,ANALOG);
	//digitalWrite(ESP_NRF_CE,HIGH);
	pinMode(ESP_SWITCH,ANALOG);
	pinMode(ESP_ACCL_INT,ANALOG);
	
	#ifdef DEBUG_ENABLE
	SerialMon.println(F("GPIO Dinit done.."));
	#endif
}


void indicate_led(int red, int green, int blue, uint16_t delay_on, uint16_t delay_off)
{
	if (delay_on)
	{
		mcp.digitalWrite(MCP_LED_RED,LOW);
		mcp.digitalWrite(MCP_LED_GREEN,LOW);
		mcp.digitalWrite(MCP_LED_BLUE,LOW);
		delayMicroseconds(10);
		mcp.digitalWrite(MCP_LED_RED,red);
		mcp.digitalWrite(MCP_LED_GREEN,green);
		mcp.digitalWrite(MCP_LED_BLUE,blue);
		delay(delay_on);
		mcp.digitalWrite(MCP_LED_RED,LOW);
		mcp.digitalWrite(MCP_LED_GREEN,LOW);
		mcp.digitalWrite(MCP_LED_BLUE,LOW);
		if (delay_off)delay(delay_off);
	}
	else
	{
		mcp.digitalWrite(MCP_LED_RED,red);
		mcp.digitalWrite(MCP_LED_GREEN,green);
		mcp.digitalWrite(MCP_LED_BLUE,blue);
	}
	
}

void Scanner ()
{
	SerialMon.println ();
	SerialMon.println ("I2C scanner. Scanning ...");
	byte count = 0;

	Wire.begin();
	for (byte i = 8; i < 120; i++)
	{
		Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
		if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response)
		{
			SerialMon.print ("Found address: ");
			SerialMon.print (i, DEC);
			SerialMon.print (" (0x");
			SerialMon.print (i, HEX);     // PCF8574 7 bit address
			SerialMon.println (")");
			count++;
		}
	}
	SerialMon.print ("Found ");
	SerialMon.print (count, DEC);        // numbers of devices
	SerialMon.println (" device(s).");
}
