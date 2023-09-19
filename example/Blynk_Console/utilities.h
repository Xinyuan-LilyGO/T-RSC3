/**
 * @file      utilities.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-19
 *
 */

#pragma once

#include <Arduino.h>

#define Serial232               Serial0
#define Serial485               Serial1
#define RS485_BAUD              9600
#define RS232_BAUD              9600
#define RS485_TX_ENABLE         HIGH
#define RS485_RX_ENABLE         LOW
#define NUMPIXELS               1

#if CONFIG_IDF_TARGET_ESP32C3

#define RS485_RX_PIN            3
#define RS485_TX_PIN            10
#define RS485_CON_PIN           5
#define RS232_RX_PIN            1
#define RS232_TX_PIN            0
#define LED_PIN                 4
#define KEY_PIN                 2
#define BOOT_PIN                9

#elif CONFIG_IDF_TARGET_ESP32S3


#define RS485_RX_PIN            2
#define RS485_TX_PIN            3
#define RS485_CON_PIN           4

#define RS232_RX_PIN            42
#define RS232_TX_PIN            41
#define LED_PIN                 1
#define KEY_PIN                 5
#define BOOT_PIN                0
#endif



