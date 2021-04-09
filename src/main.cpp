#include <Arduino.h>
#include <ArduinoLowPower.h>
#include <gps.h>

// GPSModule gpsModule;

char buffer[128];
void setup()
{
    // put your setup code here, to run once:
    gpsModule.setup();
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    static unsigned long start = millis();
    static unsigned long ledtime=start;
    static unsigned long gpstime=start;
    static unsigned long led_interval = 1000;
    static unsigned int seconds = 0;
    unsigned long now = millis();

    if(gpsModule.checkGPS())
    {
        gpsModule.getFixStr(buffer);
        SerialUSB.print(buffer);
    }

    if (now-gpstime > 1000)
    {
        gpstime = now;
        seconds +=1;
    }

    switch(gpsModule.state())
    {
        case GPSModule::gps_state::gps_on:
            break;

        case GPSModule::gps_state::gps_on_first_fix:
            led_interval = 500;
            break;

        case GPSModule::gps_state::gps_on_fix:
            if (seconds > 10)
            {
                SerialUSB.println("gps power off");
                gpsModule.powerOff();
                seconds = 0;
                led_interval = 1000;
            }

        case GPSModule::gps_state::gps_off:
            if (seconds > 10)
            {
                SerialUSB.println("gps power on");
                gpsModule.powerOn();
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
