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
    // digitalWrite(PIN_GPS_POWER, GPS_OFF);    // GPS power OFF
    digitalWrite(PIN_GPS_POWER, GPS_ON);    // GPS power ON
}

static bool checkGPS()
{
    bool gpsFix = false;

    while (Serial1.available())
    {
        char ch;
        ch = Serial1.read();
        // SerialUSB.print(ch);

        if (gps.encode(ch))
        {
            gpsFix = true;
        }
    }
    return gpsFix;
}

static void showFix()
{
    long lat, lon;
    unsigned long fix_age; // returns +- latitude/longitude in degrees
    unsigned long date, time;
    gps.get_position(&lat, &lon, &fix_age);
    gps.get_datetime(&date, &time, &fix_age);
    // SerialUSB.println(fix_age);
    if (fix_age == TinyGPS::GPS_INVALID_AGE)
    {
        SerialUSB.println("No fix detected");
    }
    else if (fix_age > 5000)
    {
        SerialUSB.println("Warning: possible stale data!");
    }
    else
    {
        sprintf(buffer, "Date: %lu, Time: %lu, LAT: %ld, LON: %ld\n", date, time, lat, lon);
        SerialUSB.print(buffer);
    }
}
enum class gps_state
{
    gps_on,
    gps_on_fix,
    gps_off
};


void loop()
{
    static gps_state state = gps_state::gps_on;


    static unsigned long start = millis();
    static unsigned long ledtime=start;
    static unsigned long gpstime=start;
    static unsigned long led_interval = 1000;
    static unsigned int seconds = 0;
    static bool gotFix = false;
    static unsigned long ttff = 0;

    unsigned long now = millis();

    if(checkGPS())
    {
        gotFix = true;
        showFix();
    }

    if (now-gpstime > 1000)
    {
        gpstime = now;
        seconds +=1;
    }

    switch(state)
    {
        case gps_state::gps_on:
            if (gotFix)
            {
                state = gps_state::gps_on_fix;
                ttff = now - start;
                SerialUSB.print(ttff);
                SerialUSB.println(" TTFF");
                seconds = 0;
                led_interval = 500;
            }
            break;

        case gps_state::gps_on_fix:
            if (seconds > 10)
            {
                state = gps_state::gps_off;
                gotFix = false;
                ttff = 0;
                SerialUSB.println(" gps power off");
                digitalWrite(PIN_GPS_POWER, GPS_OFF);
                seconds = 0;
                led_interval = 1000;
            }
            break;

        case gps_state::gps_off:
            if (seconds > 10)
            {
                state = gps_state::gps_on;
                SerialUSB.println(" gps power on");
                digitalWrite(PIN_GPS_POWER, GPS_ON);
                start = now;
                seconds = 0;
            }
            break;
    }

    if (now-ledtime > led_interval)
    {
        ledtime = now;

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED on (HIGH is the voltage level)
    }
}
