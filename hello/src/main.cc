#include <Arduino.h>


void
setup()
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.println("boot OK");
}


void
loop()
{

}
