#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <ModbusMaster.h>
ModbusMaster node;
#define Serial232        Serial1
#define RS232_BAUD       9600
#define KEY_PIN          2
#define RS232_RX_PIN     1
#define RS232_TX_PIN     0
#define LED_PIN          4
#define NUMPIXELS        1

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
void RS485_Mode(int Mode);

void setup()
{
    Serial.begin(9600);
    Serial232.begin(RS232_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
    // communicate with Modbus slave ID 2 over Serial (port 0)
    node.begin(2, Serial232);

    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"
        ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priority
        ,  NULL
        ,  ARDUINO_RUNNING_CORE);

}

void loop()
{
    static uint32_t i;
    uint8_t j, result;
    uint16_t data[6];
    i++;

    // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
    node.setTransmitBuffer(0, lowWord(i));

    // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
    node.setTransmitBuffer(1, highWord(i));

    // slave: write TX buffer to (2) 16-bit registers starting at register 0
    result = node.writeMultipleRegisters(0, 2);

    // slave: read (6) 16-bit registers starting at register 2 to RX buffer
    result = node.readHoldingRegisters(2, 6);

    // do something with data if read is successful
    if (result == node.ku8MBSuccess) {
        for (j = 0; j < 6; j++) {
            data[j] = node.getResponseBuffer(j);
        }
    }
}


void colorWipe(uint32_t color, int wait)
{
    for (int i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, color);
        pixels.show();
        vTaskDelay(wait);
    }
}


void TaskBlink(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    for (;;) {
        Serial.println("setup");
        colorWipe(pixels.Color(255,   0,   0), 500); // Red
        colorWipe(pixels.Color(  0, 255,   0), 500); // Green
        colorWipe(pixels.Color(  0,   0, 255), 500); // Blue
        vTaskDelay(5);
    }
}

