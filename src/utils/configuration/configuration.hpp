#pragma once

#include "../models/GSMModel/GSMModel.hpp"
#include "../models/DeviceModel/DeviceModel.hpp"

using namespace GSMNamespace;
using namespace DeviceNamespace;

namespace DefaultConfiguration {
    class Configuration {
        private:
            const uint32_t DEVICE_BAUD {115200};
            const int      DEVICE_LED  {12};
            const uint32_t DEVICE_FBAUD{115200};
            const int      DEVICE_FTX  {2};
            const int      DEVICE_FRX  {15};

            const String   GSM_APN     {"cmnet"};
            const String   GSM_USER    {};
            const String   GSM_PASS    {};
            const String   GSM_PIN     {};
            const String   GSM_RESOURCE{"/TinyGSM/logo.txt"};
            const String   GSM_SERVER  {"vsh.pp.ua"};
            const uint16_t GSM_PORT    {80};
            const uint32_t GSM_BAUD    {115200};
            const int      GSM_TX      {27};
            const int      GSM_RX      {26};
            const int      GSM_PWRKEY  {4};
            const int      GSM_DTR     {32};
            const int      GSM_RI      {33};
            const int      GSM_FLIGHT  {25};
            const int      GSM_STATUS  {34};
            
            const String   BROKER      {"mqtt.miota.io"};
            // const String   BROKER      {"broker.hivemq.com"};
            // port    : 1883
            // username: iot
            // password: iot
            const String   TOPIC_LED       {"GsmClientTest/led"};
            const String   TOPIC_INIT      {"sewacopy/init"};
            const String   TOPIC_LED_STATUS{"GsmClientTest/ledStatus"};

            DeviceModel    devModel;
            GSMModel       gsmModel;
        public:
            Configuration();
            GSMModel getGSMModel();
            DeviceModel getDeviceModel();
    };
}