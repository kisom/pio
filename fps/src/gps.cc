#include <Arduino.h>
#include <Adafruit_GPS.h>

#include <gps.h>


static Adafruit_GPS	gps(&GPSSerial);
int			ledPin = 13;


#ifndef	GPS_DEBUG
#define GPS_DEBUG	false
#endif


void
startGPS()
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
	gps.begin(9600);
	gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
	gps.sendCommand(PGCMD_ANTENNA);
	delay(1000);
}


void
gpsUpdate()
{
	char ch;
	if (GPSSerial.available()) {
		ch = gps.read();
		if (GPS_DEBUG)	Serial.print(ch);
	}
	if (gps.newNMEAreceived()) {
		gps.parse(gps.lastNMEA());
	}
}


bool
checkFix()
{
	gps.read();
	if (gps.newNMEAreceived()) {
		gps.parse(gps.lastNMEA());
	}

	if (gps.fix) {
		digitalWrite(ledPin, HIGH);
		return true;
	}
	digitalWrite(ledPin, LOW);
	return false;
}


bool
getPosition(struct Position &pos)
{
	gpsUpdate();

	pos.latitude = gps.latitude;
	pos.longitude = gps.longitude;
	pos.fix.quality = gps.fixquality;
	pos.fix.satellites = gps.satellites;
	getTime(pos.timestamp);

	return gps.fix;
}


void
getTime(struct Time &time)
{
	time.year = gps.year;
	if (time.year)	time.year += 2000;
	time.month = gps.month;
	time.day = gps.day;
	time.hour = gps.hour;
	time.minute = gps.minute;
	time.second = gps.seconds;
}
