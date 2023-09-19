/**
 * @file      RS232_loopback.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-04-12
 *
 */

#include "Arduino.h"
#include "utilities.h"

uint32_t timestamp = 0;

void setup()
{
    Serial.begin(9600);
    Serial232.begin(RS485_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
}

// Short circuit the RX TX of RS232, you can check whether the RS232 hardware is normal
void loop()
{
    if (millis() > timestamp) {
        timestamp = millis() + 1000;
        Serial.print("Send ->"); Serial.println(millis());
        Serial232.print("Running ["); Serial232.print(millis() / 1000); Serial232.println("]");
    }
    while (Serial232.available()) {
        Serial.write(Serial232.read());
    }
}
