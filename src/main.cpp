#include <Arduino.h>
// #include "TinyGPS.h"
#include <ArduinoLowPower.h>
#include <SPI.h>
#include <RH_RF95.h>

#define DIO0 17
static RH_RF95 rf95(SS,DIO0);

// TinyGPS gps;
char buffer[128];
void setup()
{
    Serial1.begin(9600);

    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_GPS_POWER, OUTPUT);
    // digitalWrite(PIN_GPS_POWER, GPS_OFF);    // GPS power OFF
    digitalWrite(PIN_GPS_POWER, GPS_ON);    // GPS power ON
    digitalWrite(LED_BUILTIN, HIGH); // toggle the LED on (HIGH is the voltage level)
    // delay(8000);
    // rf95.sleep(); // put radio into sleep mode
}


enum class led_state
{
    led_on,
    led_off
};

void loop()
{
    static led_state state = led_state::led_on;

    static unsigned long start = millis();
    static unsigned long ledtime=start;
    static unsigned long second=start;
    static unsigned long led_interval = 1000;
    static unsigned int seconds = 0;

    unsigned long now = millis();

    if (now-second > 1000)
    {
        second = now;
        seconds +=1;
    }

    switch(state)
    {
        case led_state::led_on:
            if (seconds > 10)
            {
                state = led_state::led_off;
                SerialUSB.println(" gps power off");
                digitalWrite(PIN_GPS_POWER, GPS_OFF);
                seconds = 0;

                digitalWrite(LED_BUILTIN, LOW);
                LowPower.sleep(8000);
            }
            break;

        case led_state::led_off:
            if (seconds > 10)
            {
                state = led_state::led_on;
                SerialUSB.println(" gps power on");
                digitalWrite(PIN_GPS_POWER, GPS_ON);
                seconds = 0;
                start = now;
            }
            break;
    }

    if (now-ledtime > led_interval)
    {
        ledtime = now;

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED on (HIGH is the voltage level)
    }
}
