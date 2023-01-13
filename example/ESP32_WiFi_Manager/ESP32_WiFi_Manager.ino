#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

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

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);


// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "";
const char *password = "";
bool RS485mode = true;
long int late_Millis = 0;
long int Millis = 0;

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void Task_RS232( void *pvParameters );
void colorWipe(uint32_t color, int wait);
void RS485_Mode(int Mode);

void recvMsg(uint8_t *data, size_t len)
{
    WebSerial.println("Received Data...");

    String Web_str = "";
    String str2 = "";
    String RS232_str = "RS232";
    String RS485_str = "RS485";
    for (int i = 0; i < len; i++) {
        Web_str += char(data[i]);
    }
    WebSerial.println(Web_str);
    if (Web_str.length() > 0) {
        if (Web_str.indexOf(' ') > 0) { //Found the ' 'space
            if (RS232_str.compareTo(Web_str.substring(0, Web_str.indexOf(' ') )) == 0) { //RS232 beginning
                str2 = Web_str.substring(Web_str.indexOf(' ') + 1, Web_str.length());
                Serial232.print(str2);
            }  if (RS485_str.compareTo(Web_str.substring(0, Web_str.indexOf(' ') )) == 0) {//RS485 beginning
                str2 = Web_str.substring(Web_str.indexOf(' ') + 1, Web_str.length());
                Serial485.print(str2);
                Serial.print(str2);
            }
        }
        if (Web_str == "RS485_TX_MODE") {
            Serial.print(" RS485_TX_MODE");
            RS485_Mode(RS485_TX_ENABLE);
        }
        if (Web_str == "RS485_RX_MODE") {
            Serial.print(" RS485_RX_MODE");
            RS485_Mode(RS485_RX_ENABLE);
        }
        str2 = "";
    }
}



void setup()
{
    Serial.begin(115200);

    Serial485.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    Serial232.begin(RS232_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
    delay(100);

    RS485_Mode(RS485_RX_ENABLE);

    pixels.begin();
    pixels.setBrightness(255);
    pixels.clear();
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("wifi connection");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("In the same LAN. Open the website:");
    Serial.print(WiFi.localIP() ); Serial.println("/webserial");
    colorWipe(pixels.Color(  0, 255,  0 ), 500); // Blue

    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);
    server.begin();


    xTaskCreatePinnedToCore(
        TaskBlink
        ,  "TaskBlink"   // A name just for humans
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
}

void loop()
{
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
    // WebSerial.println("Hello!");
    vTaskDelay(20);
}


void Task_RS232(void *pvParameters)
{
    (void) pvParameters;

    for (;;) {
        // while (Serial.available())
        //     Serial232.write(Serial.read());
        String str = "";
        while (Serial232.available() > 0) {
            str += char(Serial232.read());
        }
        if (str.length() > 0) {
            WebSerial.println(str);
            vTaskDelay(10);
            str = "";
        }
    }
}
void Task_RS485(void *pvParameters)
{
    (void) pvParameters;

    for (;;) {


        String rs485_str = "";
        while (Serial485.available() > 0) {
            rs485_str += char(Serial485.read());
        }
        if (rs485_str.length() > 0) {
            WebSerial.println(rs485_str);
            vTaskDelay(10);
            rs485_str = "";
        }
        vTaskDelay(10);
    }
}


void TaskBlink(void *pvParameters)   // This is a task.
{
    (void) pvParameters;
    for (;;) {


        colorWipe(pixels.Color(255,   0,   0), 500); // Red
        colorWipe(pixels.Color(  0, 255,   0), 500); // Green
        colorWipe(pixels.Color(  0,   0, 255), 500); // Blue
        vTaskDelay(5);
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
