
#include <ModbusMaster.h>
ModbusMaster node;
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define Serial485        Serial1
#define RS485_BAUD       9600
#define KEY_PIN          2
#define RS485_RX_PIN     3
#define RS485_TX_PIN     10
#define RS485_CON_PIN    5
#define RS485_TX_ENABLE  HIGH
#define RS485_RX_ENABLE  LOW
#define LED_PIN          4
#define NUMPIXELS        1

bool RS485mode = true;

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
void RS485_Mode(int Mode);

void setup()
{
    Serial.begin(9600);
    pinMode(RS485_CON_PIN, OUTPUT);
    Serial485.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    // communicate with Modbus slave ID 2 over Serial (port 0)
    node.begin(2, Serial485);

    RS485_Mode(RS485_TX_ENABLE);
    delay(20);


    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"
        ,  1024
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
    RS485_Mode(RS485_TX_ENABLE);
    vTaskDelay(20);
    // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
    node.setTransmitBuffer(0, lowWord(i));

    // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
    node.setTransmitBuffer(1, highWord(i));

    // slave: write TX buffer to (2) 16-bit registers starting at register 0
    result = node.writeMultipleRegisters(0, 2);

    RS485_Mode(RS485_RX_ENABLE);
    delay(20);
    // slave: read (6) 16-bit registers starting at register 2 to RX buffer
    result = node.readHoldingRegisters(2, 6);

    // do something with data if read is successful
    if (result == node.ku8MBSuccess) {
        for (j = 0; j < 6; j++) {
            data[j] = node.getResponseBuffer(j);
        }
    }

}

void Task_RS485(void *pvParameters)  // This is a task.
{
    (void) pvParameters;

    for (;;) {

        if (digitalRead(KEY_PIN) == 0) {
            vTaskDelay(100);
            if (digitalRead(KEY_PIN) == 0) {
                while (digitalRead(KEY_PIN) == 0);
                RS485mode = !RS485mode;
                digitalWrite(RS485_CON_PIN, RS485mode);
                if (RS485mode) {
                    Serial.println("******RS485 TX Mode******");
                } else {
                    Serial.println("******RS485 RX Mode******");
                }
            }
        }

        vTaskDelay(10);
    }
}


void RS485_Mode(int Mode)
{
    digitalWrite(RS485_CON_PIN, Mode);
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
        colorWipe(pixels.Color(255,   0,   0), 500); // Red
        colorWipe(pixels.Color(  0, 255,   0), 500); // Green
        colorWipe(pixels.Color(  0,   0, 255), 500); // Blue
        vTaskDelay(5);
    }
}

