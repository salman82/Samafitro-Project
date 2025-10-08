#pragma once

/* arduino */
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

/* gps++ */
#include <TinyGPS++.h>

/* gsm */
#define TINY_GSM_MODEM_SIM7600

// Set RX buffer to 1Kb
#define TINY_GSM_RX_BUFFER 1024

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Use TinyGPS NMEA math analysis library
#define USING_TINYGPS_LIBRARY

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
// These defines are only for this example; they are not needed in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

#include <TinyGsmClient.h>
#include <PubSubClient.h>

#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef USING_TINYGPS_LIBRARY
    // Use TinyGPS NMEA math analysis library
    #include <TinyGPS++.h>
#endif

#ifdef DUMP_AT_COMMANDS
    #include <StreamDebugger.h>
#endif


#define TINY_GSM_TEST_GPRS        true
// #define TINY_GSM_TEST_TCP         true
// #define TINY_GSM_TEST_CALL        true
// #define TINY_GSM_TEST_SMS         true
// #define TINY_GSM_TEST_USSD        true
// #define TINY_GSM_TEST_TEMPERATURE true
// #define TINY_GSM_TEST_TIME        true
// #define TINY_GSM_TEST_GPS         true

/* gsm */