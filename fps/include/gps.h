#ifndef __GPS_H
#define __GPS_H


#include <stdint.h>


#define GPSSerial	Serial1


struct Time {
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
}; // 7 bytes

struct Fix {
	uint8_t	quality;
	uint8_t	satellites;
}; // 2 bytes

struct Position {
	float		latitude;
	float		longitude;
	struct Time	timestamp;
	struct Fix	fix;
}; // 8 bytes + 7 bytes + 2 bytes = 17 bytes


void	startGPS();
void	gpsUpdate();
bool	checkFix();
// getPosition returns true if the GPS has a fix and the
// position data is filled.
bool	getPosition(struct Position &pos);
void	getTime(struct Time &time);


#endif
