/*
 * General options
 */

#define BOARD_HARDWARE_VERSION        "1.0.0"

/*
 * Board configuration (see examples below).
 */

#if defined(USE_NODE_MCU_BOARD)

  #warning "NodeMCU board selected"

  // Example configuration for NodeMCU v1.0 Board
  #define BOARD_BUTTON_PIN            0
  #define BOARD_BUTTON_ACTIVE_LOW     true

  #define BOARD_LED_PIN_R             D8
  #define BOARD_LED_PIN_G             D7
  #define BOARD_LED_PIN_B             D6
  #define BOARD_LED_INVERSE           false
  #define BOARD_LED_BRIGHTNESS        64

#elif defined(USE_SPARKFUN_BLYNK_BOARD)

  #warning "Sparkfun Blynk board selected"

  // Example configuration for SparkFun Blynk Board
  #define BOARD_BUTTON_PIN            0
  #define BOARD_BUTTON_ACTIVE_LOW     true

  #define BOARD_LED_PIN_WS2812        4
  #define BOARD_LED_BRIGHTNESS        64

#elif defined(USE_WITTY_CLOUD_BOARD)

  #warning "Witty Cloud board selected"

  // Example configuration for Witty cloud Board
  #define BOARD_BUTTON_PIN            4
  #define BOARD_BUTTON_ACTIVE_LOW     true

  #define BOARD_LED_PIN_R             15
  #define BOARD_LED_PIN_G             12
  #define BOARD_LED_PIN_B             13
  #define BOARD_LED_INVERSE           false
  #define BOARD_LED_BRIGHTNESS        64

#else

  // Custom board configuration
  #define BOARD_BUTTON_PIN            0                     // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true                  // true if button is "active-low"

  #define BOARD_LED_PIN               15                    // Set LED pin - if you have a single-color LED attached
  //#define BOARD_LED_PIN_R           15                    // Set R,G,B pins - if your LED is PWM RGB 
  //#define BOARD_LED_PIN_G           12
  //#define BOARD_LED_PIN_B           13
  //#define BOARD_LED_PIN_WS2812      4                     // Set if your LED is WS2812 RGB
  #define BOARD_LED_INVERSE           false                 // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64                    // 0..255 brightness control

#endif


/*
 * Advanced options
 */

#define BUTTON_HOLD_TIME_INDICATION   3000
#define BUTTON_HOLD_TIME_ACTION       10000

#define BOARD_PWM_MAX                 1023

#define CONFIG_AP_URL                 "blynk.setup"
#define CONFIG_DEFAULT_SERVER         "blynk.cloud"
#define CONFIG_DEFAULT_PORT           443

#define WIFI_NET_CONNECT_TIMEOUT      30000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_AP_CONFIG_PORT           80
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)
//#define WIFI_CAPTIVE_PORTAL_ENABLE

#define USE_TICKER
//#define USE_TIMER_ONE
//#define USE_TIMER_THREE

#define BLYNK_NO_DEFAULT_BANNER

#if defined(APP_DEBUG)
  #define DEBUG_PRINT(...) BLYNK_LOG1(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
#endif

