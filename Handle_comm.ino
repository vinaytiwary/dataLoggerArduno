
#include "Hardware_pins.h"
#include "Config.h"
#include "Functions.h"
#include "SoftwareSerial.h"

HardwareSerial SerialRS485(2);//ESP_RS485_RX, ESP_RS485_TX); //RX TX
SoftwareSerial SerialRS232();//ESP_RS232_RX, ESP_RS232_TX); //RX TX

void init_comm()
{
	SerialRS485.begin(115200,SERIAL_8N1, ESP_RS485_RX, ESP_RS485_TX);
	//SerialRS232.begin(115200, SWSERIAL_8N1, ESP_RS232_RX, ESP_RS232_TX, false, 256);
	//SerialRS232.begin(115200);
	mcp.pinMode(MCP_RS485_DIR,OUTPUT);
	mcp.digitalWrite(MCP_RS485_DIR,WIRED_MODE_RX);
}
