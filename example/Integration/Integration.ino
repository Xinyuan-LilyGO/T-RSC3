#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "utilities.h"
#include <WiFi.h>

String rs232_prefix_str = "RS232";


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool RS485mode = true;

void TaskBlink( void *pvParameters );
void Task_RS485( void *pvParameters );
void Task_RS232( void *pvParameters );
void colorWipe(uint32_t color, int wait);

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Factory example start !");

    pinMode(RS485_CON_PIN, OUTPUT);
    pinMode(KEY_PIN, INPUT_PULLUP);

    pixels.begin();
    pixels.setBrightness(255);
    pixels.clear();

    Serial485.begin(RS485_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    Serial232.begin(RS232_BAUD, SERIAL_8N1, RS232_RX_PIN, RS232_TX_PIN);
    delay(100);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scan start");

    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.printf("%2d", i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    RS485_Mode(RS485_RX_ENABLE);

    xTaskCreate(
        TaskBlink
        ,  "TaskBlink"   // A name just for humans
        ,   1024 * 10  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  10  // Priorit
        ,  NULL
    );

    xTaskCreate(
        Task_RS485
        ,  "Task_RS485"
        ,   1024 * 10  // Stack size
        ,  NULL
        ,  11  // Priority
        ,  NULL
    );


    xTaskCreate(
        Task_RS232
        ,  "Task_RS232"
        ,  1024 * 10 // Stack size
        ,  NULL
        ,  12  // Priority
        ,  NULL
    );
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
        while (Serial232.available()) {
            Serial.write(Serial232.read());
        }
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
                while (digitalRead(KEY_PIN) == 0) {
                    delay(1);
                };
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
        while (Serial485.available()) {
            Serial.write(Serial485.read());
        }
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
