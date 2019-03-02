#include <Arduino.h>
#include <Streaming.h>
#include <lora.h>

#include <lmic.h>
#include <gps.h>
#include <graphics.h>
#include <Scheduler.h>


bool		fixed = false;
unsigned long	last_print = 0;


void
radioLoop()
{
}


void setup()
{
	struct Position	pos;
	struct Time	time;
	struct Fix	fix;
	char		buf[21];

	OLED::setup();
	OLED::print(0, "GPS NO FIX");
	OLED::print(1, "BOOT START");
	startGPS();
	Serial.begin(9600);
	// while (!Serial) ;
	Serial << "BOOT START" << endl;

	lmic_init();
	Serial << "RADIO INIT" << endl;
	OLED::print(2, "RADIO INIT");
	while (!getPosition(pos)) {
		sprintf(buf, "NO FIX - %02d SATS Q %d", pos.fix.satellites, pos.fix.quality);
		OLED::print(0, buf);
		delay(10);
	}
	OLED::print(0, "GPS OK");
	OLED::print(1, "BOOT OK");
	Serial << sizeof(time) << " byte timestamp" << endl;
	Serial << sizeof(fix) << " byte fix" << endl;
	Serial << sizeof(pos) << " byte payload" << endl;
	lmic_transmit();
}


void
printTime()
{
	struct Time	time;
	unsigned long	now = millis();
	char		buf[21];

	if (last_print > now) {
		return;
	}

	getTime(time);
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		time.year, time.month, time.day,
		time.hour, time.minute, time.second);
	OLED::print(2, buf);
	last_print = now + 250;
}


void
loop()
{
	struct Position		pos;
	char			buf[21];

	gpsUpdate();
	os_runloop_once();
	lmic_transmit();
	// gpsUpdate();
	// if (getPosition(pos)) {
	// 	sprintf(buf, "GPS FIX (%d SATS)", pos.fix.satellites);
	// 	OLED::print(0, buf);
	// 	if (!fixed) {
	// 		Serial.println("GPS fix acquired");
	// 		fixed = true;
	// 	}
	// }
	// else {
	// 	OLED::print(0, "GPS NO FIX");
	// 	if (fixed) {
	// 		Serial.println("GPS fix lost");
	// 		fixed = false;
	// 	}
	// }

	// printTime();
	// lmic_transmit();
	// os_runloop_once();
	// delay(10);
}
