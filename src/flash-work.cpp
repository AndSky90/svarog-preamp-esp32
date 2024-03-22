#include "WString.h"

String readStringFromFlash(int startAddr, int size);
void writeStringToFlash(const char *toStore, int startAddr);
void commitFlash();

void writeSsidToFlash(String s)
{
    writeStringToFlash(s.c_str(), 0); // storing ssid at address 0
}

void writePassToFlash(String s)
{
    writeStringToFlash(s.c_str(), 40); // storing pss at address 40
}

String readSsidFromFlash()
{
    return readStringFromFlash(0, 40); // Read SSID stored at address 0
}

String readPassFromFlash()
{
    return readStringFromFlash(40, 40); // Read Password stored at address 40
}

void eraseSsidAndPass()
{
    writeStringToFlash("", 0);  // Reset the SSID
    writeStringToFlash("", 40); // Reset the Password
}

void finishFlash() 
{
    commitFlash();
}
