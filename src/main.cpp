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
    digitalWrite(PIN_GPS_POWER, HIGH);    // GPS power OFF

    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // delay(1000);
    // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
}

static bool smartdelay(unsigned long ms)
{
    unsigned long start = millis();
    bool gpsOK = false;
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
                    gpsOK = true;
                }
            }
        }
    } while (millis() - start < ms);
    return gpsOK;
}

static bool checkGPS()
{
    bool gpsFix = false;

    while (Serial1.available())
    {
        char ch;
        ch = Serial1.read();

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

void loop()
{
    static int gpsFixes = 0;
    static unsigned long hundreds = 0;
    static unsigned long previousMillis = 0;
    static unsigned long interval = 100;
    static unsigned long led_interval = 100;
    unsigned long currentMillis = millis();
    static int ledState = HIGH;

    if (currentMillis - previousMillis >= interval) 
    {
        previousMillis = currentMillis;
        hundreds += 1;                          // count hundreds of seconds
        if(checkGPS())
        {
            showFix();
            // led_interval = 10;
        }
    }

    if(hundreds == 20 )
    {
        digitalWrite(PIN_GPS_POWER, LOW); // GPS power ON
    }

    if(hundreds % led_interval == 0)
    {
        digitalWrite(LED_BUILTIN, ledState); // turn the LED on (HIGH is the voltage level)
        ledState ^= 1;
        SerialUSB.println(hundreds);
    }

    // if(hundreds % 100 == 0)
    // {
    //     digitalWrite(LED_BUILTIN, ledState); // turn the LED on (HIGH is the voltage level)
    //     ledState ^= 1;
    // }


    // if (checkGPS())
    // {
    //     interval = 50;
    //     showFix();
    // }
    // else
    // {
    //     interval = 1000;
    // }
    
 
    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // if (checkGPS(1000))
    // {
    //     showFix()
    // }
 
 
    // gpsFix = smartdelay(1000);                // wait for a second
    // if (gpsFix)
    // {
    //     digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    //     smartdelay(50);                 // wait for a second
    //     digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    // }

    // put your main code here, to run repeatedly:
    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // smartdelay(interval);                // wait for a second
    // // SerialUSB.println("ready");
    // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    // smartdelay(interval);
}