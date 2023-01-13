#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "Arduino.h"
#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>

#define Serial232        Serial1
#define RS485_BAUD       9600
#define RS232_RX_PIN     1
#define RS232_TX_PIN     0
#define LED_PIN          4
#define KEY_PIN          2
#define BOOT_PIN         9
#define NUMPIXELS        1
#define DELAYVAL 1000
#define RS485_TX_ENABLE  HIGH
#define RS485_RX_ENABLE  LOW

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool RS485mode = true;

void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void Task_RS232( void *pvParameters );
void colorWipe(uint32_t color, int wait);

void setup()
{
    Serial.begin(9600);
    pixels.begin();
    pixels.setBrightness(255);
    pixels.clear();

    Serial232.begin(RS485_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
    delay(100);


    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"
        ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priority
        ,  NULL
        ,  ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_RS232
        ,  "Task_RS232"
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

void TaskBlink(void *pvParameters)
{
    (void) pvParameters;
    for (;;) {

        colorWipe(pixels.Color(255,   0,   0), 500); // Red
        colorWipe(pixels.Color(  0, 255,   0), 500); // Green
        colorWipe(pixels.Color(  0,   0, 255), 500); // Blue
        vTaskDelay(5);
    }
}

void Task_RS232(void *pvParameters)
{
    (void) pvParameters;

    for (;;) {
        while (Serial.available())
            Serial232.write(Serial.read());
        while (Serial232.available())
            Serial.write(Serial232.read());
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
