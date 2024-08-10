
#include "Config.h"



void toggle_tp4056()
{
	if (mcp.digitalRead(MCP_TP_CHARGING) )
	{
		mcp.digitalWrite(MCP_TP_CH_EN,0);
		delay(500);
		mcp.digitalWrite(MCP_TP_CH_EN,1);
	}
}


void board_shutdown(uint8_t shutdown_type)
{
	/*if (shutdown_type==1)NVIC_SystemReset();
	publish_power_state(0);
	#ifdef DEBUG_ENABLE
		SerialMon.println("Initiating board shutdown");
	#endif
	simcom_safe_shutdown();
	SerialMon.end();
	SerialRF.end();
	SerialAT.end();
	
	dinit_gpio();
	
	LowPower.begin();
	LowPower.attachInterruptWakeup(SWITCH_PIN, wakeup_action, FALLING);
	LowPower.attachInterruptWakeup(TP_CHARGING, wakeup_action, FALLING);
	//LowPower.enableWakeupFrom(&rtc, alarm_match, &atime);
	//rtc.setAlarmEpoch( rtc.getEpoch() + 86400); // alarm every 24 hrs
	delay(2);
	LowPower.shutdown();
	//LowPower.deepSleep();
	setup();*/
}
