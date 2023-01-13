

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
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


/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include "Arduino.h"
#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

bool RS485mode = true;
static TaskHandle_t AppTaskCreate_Handle = NULL;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

String rs232_prefix_str = "RS232";



Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void Task_RS232( void *pvParameters );
void colorWipe(uint32_t color, int wait);

BLYNK_WRITE(V0)
{
    String string = param.asStr();
    Serial.print("RS232_Send:");
    Serial.println(string);
    char RS232_str[255];
    strcpy(RS232_str, string.c_str());
    Serial232.println(RS232_str);
}

BLYNK_WRITE(V1)
{
    String string = param.asStr();
    Serial.print("RS485_Send:");

    char RS485_str[255];
    strcpy(RS485_str, string.c_str());
    Serial485.println(RS485_str);
    Serial.println(string);

}

BLYNK_WRITE(V2)
{

}



//Syncing the output state with the app at startup
BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V0);  // will cause BLYNK_WRITE(V0) to be executed
    Blynk.syncVirtual(V1);  // will cause BLYNK_WRITE(V1) to be executed
    Blynk.syncVirtual(V2);  // will cause BLYNK_WRITE(V2) to be executed
}

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

    Blynk.begin(auth, ssid, pass);
    xTaskCreatePinnedToCore(
        Task_Create
        ,  "Task_Create"
        , 4096   // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  1  // Priority
        ,  &AppTaskCreate_Handle
        ,  ARDUINO_RUNNING_CORE);



}

void loop()
{
    Blynk.run();
}


static void Task_Create(void *pvParameters)
{
    (void) pvParameters;

    BaseType_t xReturn = pdPASS;

    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"
        ,  1024
        ,  NULL
        ,  2  // Priority
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

    vTaskDelete(AppTaskCreate_Handle); //Delete AppTaskCreate

    for (;;) {


    }

}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

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
        // while (Serial.available())
        //     Serial485.write(Serial.read());
        while (Serial485.available())
            Serial.write(Serial485.read());
        vTaskDelay(10);
    }
}

void colorWipe(uint32_t color, int wait)
{
    for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in strip...
        pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
        pixels.show();                          //  Update strip to match
        vTaskDelay(wait);                           //  Pause for a moment
    }
}

void RS485_Mode(int Mode)
{
    digitalWrite(RS485_CON_PIN, Mode);
}