#include "WString.h"
#include <Arduino.h>

#define WiFi_rst 0 // WiFi credential reset pin (0 = Boot button on ESP32)

unsigned long wifiResetMs;

void eraseSsidAndPass();


void pinSetup()
{
    pinMode(WiFi_rst, INPUT);
}

void loopResetWifiPin()
{
    wifiResetMs = millis(); // todo move in while
    while (digitalRead(WiFi_rst) == LOW)
    {
        // Wait till boot button is pressed
    }
    // check the button press time if it is greater than 3sec clear wifi cred and restart ESP
    if (millis() - wifiResetMs >= 1000)
    {
        Serial.println("Reseting the WiFi credentials");
        eraseSsidAndPass();
        Serial.println("Wifi credentials erased, Restarting the ESP");
        delay(500);
        ESP.restart();
    }
}

void preampPower(bool state)
{
    // if(state) вкл else выкл
}