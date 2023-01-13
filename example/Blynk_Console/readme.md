<h1 align = "center">🌟LILYGO T-RSC3🌟</h1>

# Steps:
1. Need to install the following dependencies
     - [blynk](https://github.com/blynkkk/blynk-library)
     - [TinyGSM](https://github.com/vshymanskyy/TinyGSM)
     - [StreamDebugger](https://github.com/vshymanskyy/StreamDebugger)
     - [ArduinoHttpClient](https://github.com/ricemices/ArduinoHttpClient)
     - [ESP32-Arduino-CAN](https://github.com/miwagner/ESP32-Arduino-CAN)

2. Register and login [blynk.cloud](https://blynk.cloud/dashboard/login)
3. Create a new template or use an existing template (The name can be customized)

  ![](../../image/Blynk/1-Create_Template-T-RSC3.png)

4. When the Template is ready, go to Search -> Devices - Create New Device

  ![](../../image/Blynk/2-Create_device-T-RSC3.png)

5. Configuration module

  ![](../../image/Blynk/3-Configuration_module.png)

  ![](../../image/Blynk/3-1-Configuration_module.png)

  ![](../../image/Blynk/3-2-Configuration_module.png)

  ![](../../image/Blynk/3-3-Configuration_module.png)

6. Set up the Web dashboard 

  ![](../../image/Blynk/4-1-Dash_board.png)

  ![](../../image/Blynk/4-2-Dash_board.png)

  ![](../../image/Blynk/4-3-Dash_board.png)


7. Replace the macro definition in the code

    #define BLYNK_TEMPLATE_ID "" 
    #define BLYNK_DEVICE_NAME "" 
    #define BLYNK_AUTH_TOKEN ""

  ![](../../image/Blynk/6-RSC3_code.png)

8. Upload the code. Now you can send the data and fetch it in the cloud
   ①Press key K1(IO0) to switch the mode of RS485
   ②RS485 can send data directly on the serial port
   ③RS232 needs to add the prefix "RS232" to send (you can customize the prefix by modifying the value of rs232_prefix_str). If you send 123, it is RS232 123. The space in between is    necessary.

## Blynk References

1. [Getting Started Guide](https://www.blynk.cc/getting-started)
2. [Documentation](https://docs.blynk.io/en/blynk.apps/overview)
3. [Sketch generator](https://examples.blynk.cc/)
