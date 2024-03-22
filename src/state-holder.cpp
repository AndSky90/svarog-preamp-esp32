#include "WString.h"

long ts = 0;
String state = "00000000";

String getDefaultState()
{
    return "00000000";
}



// 00 = preampPower, 00 = pilotPower, 000 = input channel, 0 = output channel
// 0 = mute, 0000000 = volume,

// 0000 = corr1, 0000 = corr2
// 0000 = corr3, 0000 = corr4

// 0000 = corr5, 0000 = corr6
// 0000 = corr7, 0000 = corr8

// 0 = restartEsp, 0 = resetSettings, 0 = CResetConnection (15-17), 000- unused, 00 - powerState, 18
// val b7 = byteArray[7] // reserve settings

// val b8 = byteArray[8] //00000000 - dac
// val b9 = byteArray[9] //00000000 - dac