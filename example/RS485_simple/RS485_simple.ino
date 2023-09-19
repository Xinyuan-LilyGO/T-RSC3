#include "Arduino.h"
#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>
#include "utilities.h"


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool RS485mode = true;


void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void colorWipe(uint32_t color, int wait);

void setup()
{
    Serial.begin(9600);
    pinMode(RS485_CON_PIN, OUTPUT);
    pinMode(KEY_PIN, INPUT);

    pixels.begin();
    pixels.setBrightness(255);
    pixels.clear();

    Serial485.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    delay(100);

    RS485_Mode(RS485_RX_ENABLE);

    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"
        ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,  NULL
        ,  ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_RS485
        ,  "Task_RS485"
        ,  1024  // Stack size
        ,  NULL
        ,  1  // Priority
        ,  NULL
        ,  ARDUINO_RUNNING_CORE);

}

void loop()
{
    vTaskDelay(10);
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

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


void Task_RS485(void *pvParameters)
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

        while (Serial.available())
            Serial485.write(Serial.read());
        while (Serial485.available())
            Serial.write(Serial485.read());
        vTaskDelay(10);
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

void RS485_Mode(int Mode)
{
    digitalWrite(RS485_CON_PIN, Mode);
}