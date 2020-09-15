#include <Arduino.h>
#include "TinyGPS.h"
TinyGPS gps;
char buffer[128];
void setup()
{
    Serial1.begin(9600);

    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_GPS_POWER, OUTPUT);
    //   digitalWrite(PIN_GPS_POWER, HIGH);    // GPS power OFF
    digitalWrite(PIN_GPS_POWER, LOW); // GPS power ON
}

static void smartdelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (Serial1.available())
        {
            char ch;
            ch = Serial1.read();
            // SerialUSB.print(ch);
            if (gps.encode(ch))
            {
                long lat, lon;
                unsigned long fix_age; // returns +- latitude/longitude in degrees
                unsigned long date, time;
                gps.get_position(&lat, &lon, &fix_age);
                gps.get_datetime(&date, &time, &fix_age);
                if (fix_age == TinyGPS::GPS_INVALID_AGE)
                    SerialUSB.println("No fix detected");
                else if (fix_age > 5000)
                    SerialUSB.println("Warning: possible stale data!");
                else
                {
                    sprintf(buffer, "Date: %lu, Time: %lu, LAT: %ld, LON: %ld\n", date, time, lat, lon);
                    SerialUSB.print(buffer);
                }
            }
        }
    } while (millis() - start < ms);
}
void loop()
{
    // put your main code here, to run repeatedly:
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    smartdelay(1000);                // wait for a second
    // SerialUSB.println("ready");
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    smartdelay(1000);
}