#include "WString.h"

long long ts = 0;
uint8_t state[8];

void preampPower(bool state);
void pilotPower(bool state);
void inputChannel(short state);
void outputChannel(bool state);
void mute(bool state);
void volume(short state);
void correction(short channel, short state);
void restartEsp();
void resetSettings();
void resetAll();
void powerState(short state);

void handleInit();

void send(String state);

void handleCommand(String command)
{
    // timestamp newTs
    uint8_t tsBytes[8];
    command.substring(0, 8).getBytes(tsBytes, 8);

    long long newTs = 0;
    for (int i = 0; i < 8; ++i)
    {
        newTs |= (static_cast<long long>(tsBytes[i]) << (i * 8));
    }

    uint8_t caddress = command[8];

    if (command.length() == 9)
    {
        if (caddress == 0)
        {
            handleInit();
            ts = newTs;
           // return state; // TODO
        }
    }
    else
    {
        char cvalue = command[9];
        switch (caddress)
        {
        case 1:
        {
            preampPower(cvalue);
            // для третьего элемента массива
            // state[2] |= (1 << 7);  // Установить седьмой бит в единицу
            // state[2] &= ~(1 << 6); // Установить шестой бит в ноль

            if (cvalue == 0)
            {
                state[0] &= ~(1 << 0);
                state[0] &= ~(1 << 1);
            }
            else
            {
                state[0] |= (1 << 0);
                state[0] |= (1 << 1);
            }
            break;
        }
        case 2:
        {
            pilotPower(cvalue);
            if (cvalue == 0)
            {
                state[0] &= ~(1 << 2);
                state[0] &= ~(1 << 3);
            }
            else
            {
                state[0] |= (1 << 2);
                state[0] |= (1 << 3);
            }
            break;
        }
        case 3:
        {
            inputChannel(cvalue);
            uint8_t mask = 0b111 << 4; // Маска для битов 4, 5 и 6
            state[0] &= ~(mask);
            state[0] |= (cvalue & mask);
            break;
        }
        case 4:
        {
            outputChannel(cvalue);
            (cvalue == 0) ? state[0] &= ~(1 << 7) : state[0] |= (1 << 7);
            break;
        }
        case 5:
        {
            mute(cvalue);
            (cvalue == 0) ? state[1] &= ~(1 << 0) : state[1] |= (1 << 0);
            break;
        }
        case 6:
        {
            volume(cvalue);
            uint8_t mask = 0b1111111 << 1;
            state[1] &= ~(mask);
            state[1] |= (cvalue & mask);
            break;
        }
        case 7:
        {
            correction(0, cvalue);
            uint8_t mask = 0b1111 << 0;
            state[2] &= ~(mask);
            state[2] |= (cvalue & mask);
            break;
        }
        case 8:
        {
            correction(1, cvalue);
            uint8_t mask = 0b1111 << 4;
            state[2] &= ~(mask);
            state[2] |= (cvalue & mask);
            break;
        }
        case 9:
        {
            correction(2, cvalue);
            uint8_t mask = 0b1111 << 0;
            state[3] &= ~(mask);
            state[3] |= (cvalue & mask);
            break;
        }
        case 10:
        {
            correction(3, cvalue);
            uint8_t mask = 0b1111 << 4;
            state[3] &= ~(mask);
            state[3] |= (cvalue & mask);
            break;
        }
        case 11:
        {
            correction(4, cvalue);
            uint8_t mask = 0b1111 << 0;
            state[4] &= ~(mask);
            state[4] |= (cvalue & mask);
            break;
        }
        case 12:
        {
            correction(5, cvalue);
            uint8_t mask = 0b1111 << 4;
            state[4] &= ~(mask);
            state[4] |= (cvalue & mask);
            break;
        }
        case 13:
        {
            correction(6, cvalue);
            uint8_t mask = 0b1111 << 0;
            state[5] &= ~(mask);
            state[5] |= (cvalue & mask);
            break;
        }
        case 14:
        {
            correction(7, cvalue);
            uint8_t mask = 0b1111 << 4;
            state[5] &= ~(mask);
            state[5] |= (cvalue & mask);
            break;
        }
        case 15:
        {
            restartEsp();
            (cvalue == 0) ? state[6] &= ~(1 << 0) : state[6] |= (1 << 0);
            break;
        }
        case 16:
        {
            resetSettings();
            (cvalue == 0) ? state[6] &= ~(1 << 1) : state[6] |= (1 << 1);
            break;
        }
        case 17:
        {
            resetAll();
            (cvalue == 0) ? state[6] &= ~(1 << 2) : state[6] |= (1 << 2);
            break;
        }
        case 18:
        {
            powerState(cvalue);
            uint8_t mask = 0b11 << 6;
            state[1] &= ~(mask);
            state[1] |= (cvalue & mask);
            break;
        }
        }
    }
}

String deviceCommand(String command)
{
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