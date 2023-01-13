#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "Arduino.h"
#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>

#define Serial232        Serial0
#define Serial485        Serial1
#define RS485_BAUD       9600
#define RS232_BAUD       9600
#define RS485_RX_PIN     3
#define RS485_TX_PIN     10
#define RS485_CON_PIN    5
#define RS232_RX_PIN     1
#define RS232_TX_PIN     0
#define LED_PIN          4
#define KEY_PIN          2
#define BOOT_PIN         9
#define NUMPIXELS        1
#define DELAYVAL 1000 // Time (in milliseconds) to pause between pixels
#define RS485_TX_ENABLE  HIGH
#define RS485_RX_ENABLE  LOW

String rs232_prefix_str = "RS232";


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool RS485mode = true;

void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void Task_RS232( void *pvParameters );
void colorWipe(uint32_t color, int wait);

void setup()
{
    Serial.begin(9600);
    pinMode(RS485_CON_PIN, OUTPUT);

    pixels.begin();
    pixels.setBrightness(255);
    pixels.clear();

    Serial485.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    Serial232.begin(RS232_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
    delay(100);

    RS485_Mode(RS485_RX_ENABLE);

    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"   // A name just for humans
        ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priorit
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
    String rx_value = "";
    String rs232_str = "";
    while (Serial.available() > 0) {
        rx_value += char(Serial.read());
    }
    if (rx_value.length() > 0) {
        if (rx_value.indexOf(' ') > 0) { //Found ' ' space
            if (rs232_prefix_str.compareTo(rx_value.substring(0, rx_value.indexOf(' ') )) == 0) {
                rs232_str = rx_value.substring(rx_value.indexOf(' ') + 1, rx_value.length());
                Serial232.print(rs232_str);
            } else {
                Serial485.print(rx_value);
            }
        } else {
            Serial485.print(rx_value);
        }
        rx_value = "";
        rs232_str = "";
    }
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

void Task_RS232(void *pvParameters)
{
    (void) pvParameters;

    for (;;) {
        // while (Serial.available())
        //     Serial232.write(Serial.read());
        while (Serial232.available())
            Serial.write(Serial232.read());
        vTaskDelay(10);
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

        // while (Serial.available())
        //     Serial485.write(Serial.read());
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
