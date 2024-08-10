
#include <EEPROM.h>
#include "Config.h"

uint16_t read_uint16_eeprom(uint16_t addr)
{
	return (EEPROM.read(addr) << 8) + EEPROM.read(addr + 1);
}

uint16_t write_uint16_eeprom_temp(uint16_t addr,uint16_t tmpr)
{
	EEPROM.write(addr, tmpr >> 8);
	EEPROM.write(addr + 1, tmpr & 0xFF);
	EEPROM.commit();
	return 1;
}
int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
	byte len = strToWrite.length();
	EEPROM.write(addrOffset, len);
	for (int i = 0; i < len; i++)
	{
		EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
	}
	EEPROM.commit();
	return addrOffset + 1 + len;
}
int readStringFromEEPROM(int addrOffset, String *strToRead)
{
	int newStrLen = EEPROM.read(addrOffset);
	char data[newStrLen + 1];
	for (int i = 0; i < newStrLen; i++)
	{
		data[i] = EEPROM.read(addrOffset + 1 + i);
	}
	data[newStrLen] = '\0'; // the character may appear in a weird way, you should read: 'only one backslash and 0'
	*strToRead = String(data);
	return addrOffset + 1 + newStrLen;
}


uint8_t read_flash_config()
{

	#ifdef DEBUG_ENABLE
		SerialMon.println(F("Reading flash parameters.."));
	#endif
	delay(100);
	
	EEPROM.begin(EEPROM_SIZE);
	delay(100);
	uint8_t temp_8bit =0;
	temp_8bit=EEPROM.read(ADDR_PROV_STAT);
	if ((temp_8bit!=DEV_NOT_PROVISIONED) &&(temp_8bit!=DEV_PROVISIONED)) return RETURN_FAIL;
	gateway_prov_status=temp_8bit;
	#ifdef DEBUG_ENABLE
		SerialMon.printf("Gateway prov stat:%d \n",gateway_prov_status);
	#endif
	
	temp_8bit=EEPROM.read(ADDR_GPS_TIME);
	if ((temp_8bit<GPS_TIME_MIN) ||(temp_8bit>GPS_TIME_MAX)) return RETURN_FAIL;
	gps_upload_time=temp_8bit;
	#ifdef DEBUG_ENABLE
		SerialMon.printf("Gateway GPS upload time:%d \n",gps_upload_time);
	#endif
	
	temp_8bit=EEPROM.read(ADDR_HEALTH_TIME);
	if ((temp_8bit<HEALTH_TIME_MIN) ||(temp_8bit>HEALTH_TIME_MAX)) return RETURN_FAIL;
	hlth_upload_time=temp_8bit;
	#ifdef DEBUG_ENABLE
		SerialMon.printf("Gateway health upload time:%d \n",hlth_upload_time);
	#endif

  #ifdef DEBUG_ENABLE
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
      Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
    } 
  #endif
  
	return RETURN_OK;
}


void write_defaults()
{
	#ifdef DEBUG_ENABLE
		SerialMon.print("Writing Flash Defaults ");
		delay(100);
	#endif
	EEPROM.write(ADDR_PROV_STAT,DEF_PROV_STATE);
	EEPROM.write(ADDR_GPS_TIME,GPS_UPLOAD_DEF);
	EEPROM.write(ADDR_HEALTH_TIME,ADDR_HEALTH_TIME);
	EEPROM.commit();
	#ifdef DEBUG_ENABLE
	SerialMon.println("Done");
	#endif
}
