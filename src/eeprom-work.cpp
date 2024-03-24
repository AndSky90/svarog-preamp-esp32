#include "EEPROM.h"

#define EEPROM_SIZE 200           // EEPROM size
// #define LENGTH(x) (strlen(x) + 1) // length of char string

bool eepromInitialized()
{
    return !EEPROM.begin(EEPROM_SIZE);
}
    // работает с байтами, char = 1 байт = 8 бит
String readStringFromFlash(int startAddr, int size)
{
    char in[size];
    int i = 0;
    for (; i < size; i++)
    {
        in[i] = EEPROM.read(startAddr + i);
    }
    return in;
}

void writeStringToFlash(const char *toStore, int startAddr)
{
    int i = 0;
    for (; i < sizeof(toStore); i++)
    {
        EEPROM.write(startAddr + i, toStore[i]);
    }
    EEPROM.write(startAddr + i, '\0');
    EEPROM.commit();
}

void commitFlash()
{
    EEPROM.commit();
    EEPROM.end();
}
