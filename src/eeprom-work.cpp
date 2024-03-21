#include "EEPROM.h"

#define EEPROM_SIZE 200           // EEPROM size
#define LENGTH(x) (strlen(x) + 1) // length of char string

bool eepromInitialized()
{
    return !EEPROM.begin(EEPROM_SIZE);
}

String readStringFromFlash(int startAddr)
{
    char in[128]; // char array of size 128 for reading the stored data
    int i = 0;
    for (; i < 128; i++)
    {
        in[i] = EEPROM.read(startAddr + i);
    }
    return String(in);
}

void writeStringToFlash(const char *toStore, int startAddr)
{
    int i = 0;
    for (; i < LENGTH(toStore); i++)
    {
        EEPROM.write(startAddr + i, toStore[i]);
    }
    EEPROM.write(startAddr + i, '\0');
    EEPROM.commit();
}
