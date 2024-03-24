#include <NecRelay.h>

#define SEND_PIN 8  //TODO

const uint8_t VOLUME_COMMAND = 0b00000001;
const uint8_t SELECT_INPUT_COMMAND = 0b00100001;

NecRelay necRelay(SEND_PIN, true);

void sendVolume(uint8_t value)
{
    necRelay.SendCommand(VOLUME_COMMAND, value);
}

void selectInput(uint8_t value)
{
    necRelay.SendCommand(SELECT_INPUT_COMMAND, value << 4);
}

// Will send 0x00FF 0xAB54.
// necRelay.SendCommand(0x00, 0xAB);
//  delay(1000);

/*
    Формат передачи данных Att7 (Antecom)

    1 байт (команда)		2 байт (данные)
    0bXX XXX XXX				0bXXXXXXXX

    1 байт:
    |	0bХХ------	- резерв;
    |	0b--ХХХ---	- тип данных (для 2 байта):
    |	|	"000"	- громкость для всех аттенюаторов;  // если несколько аттенюаторов
    |	|	"001"	- постоянная коррекция громкости;   // для конкретного аттенюатора
    |	|	"010"	- баланс, +-24 dB;                  // не использую
    |	|	"011"	- временная коррекция громкости,  +-24 dB;
    |	|	"100"	- номер входа и выхода, 0...15:
    |	|	|	0b----ХХХХ	- номер выхода;
    |	|	|	0bХХХХ----	- номер входа;
    |	0b-----ХХХ	- номер аттенюатора (0...7);

    2 байт:
    |	0bХХХХХХХХ	- данные, int8_t;
*/