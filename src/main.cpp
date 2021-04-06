#include <Arduino.h>
#include "TinyGPS.h"
#include <ArduinoLowPower.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <NVStorage.h>

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


enum class test_state
{
    init,
    active,
    sleeping
};

void loop()
{
    static test_state state = test_state::init;

    static unsigned long start = millis();
    static unsigned long ledtime=start;
    static unsigned long second=start;
    static unsigned long led_interval = 1000;
    static unsigned int seconds = 0;

    unsigned long now = millis();

    // count seconds
    if (now-second > 1000)
    {
        second = now;
        seconds +=1;
    }

    switch(state)
    {
        case test_state::init:
            if (seconds > 2)
            {
                SerialUSB.println("test_state::init");
                state = test_state::active; // next state

                SerialUSB.println("radio init");
                rf95.init();

                uint8_t data;

                data = EEPROM.read(0);
                SerialUSB.print(data);

                if(data==0xff)
                {
                    EEPROM.write(0, 100);
                    EEPROM.commit();
                }
 
                // if (!EEPROM.isValid()) 
                // {
                //     SerialUSB.println("EEPROM is empty");
                // }
                seconds = 0;
            }
            break;

        case test_state::active:
            if (seconds > 10)
            {
                SerialUSB.println("test_state::active");
                state = test_state::sleeping; // next state

                SerialUSB.println("gps power off");
                digitalWrite(PIN_GPS_POWER, GPS_OFF);


                SerialUSB.println("radio sleep");
                rf95.sleep(); // put radio into sleep mode

                digitalWrite(LED_BUILTIN, LOW);
                LowPower.sleep(8000);

                seconds = 0;
            }
            break;

        case test_state::sleeping:
            if (seconds > 2)
            {
                SerialUSB.println("test_state::sleeping");
                state = test_state::active; // next state

                SerialUSB.println("gps power on");
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
