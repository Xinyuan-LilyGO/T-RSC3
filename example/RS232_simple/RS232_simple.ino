#include "Arduino.h"
#include "utilities.h"

void setup()
{
    Serial.begin(9600);
    Serial232.begin(RS485_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
}

void loop()
{
    while (Serial.available())
        Serial232.write(Serial.read());
    while (Serial232.available())
        Serial.write(Serial232.read());
}
