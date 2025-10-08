#pragma once

#include <Arduino.h>
#include "../common/CommonModel.hpp"

using namespace CommonNamespace;

namespace GSMNamespace {
    class GSMModel : CommonModel {
        private:
            String   apn;
            String   user;
            String   pass;
            String   pin;
            String   resource;
            String   server;
            uint16_t port;
            uint32_t baudRate;
            int      pinRX;
            int      pinTX;
            int      pinPWRKEY;
            int      pinDTR;
            int      pinRI;
            int      pinFlight;
            int      pinStatus;
            String   broker;
            String   topicLed;
            String   topicInit;
            String   topicLedStatus;
        public:
            GSMModel();
            GSMModel(
                String   apn,
                String   user,
                String   pass,
                String   pin,
                String   resource,
                String   server,
                uint16_t port,
                uint32_t baudRate,
                int      pinRX,
                int      pinTX,
                int      pinPWRKEY,
                int      pinDTR,
                int      pinRI,
                int      pinFlight,
                int      pinStatus,
                String   broker,
                String   topicLed,
                String   topicInit,
                String   topicLedStatus
            );
            GSMModel(                
                int pinRX,
                int pinTX,
                int pinPWRKEY,
                int pinDTR,
                int pinRI,
                int pinFlight,
                int pinStatus
            );
            String   getAPN();             void setAPN      (String apn);
            String   getUser();            void setUser     (String user);
            String   getPass();            void setPass     (String pass);
            String   getPin();             void setPin      (String pin);
            String   getResource();        void setResource (String resource);
            String   getServer();          void setServer   (String server);
            uint16_t getPort();            void setPort     (uint16_t port);
            uint32_t getBaudRate();        void setBaudRate (uint32_t baudRate);
            int      getPinRX();           void setPinRX    (int pinRX);
            int      getPinTX();           void setPinTX    (int pinTX);
            int      getPinPWRKEY();       void setPinPWRKEY(int pinPWRKEY);
            int      getPinDTR();          void setPinDTR   (int pinDTR);
            int      getPinRI();           void setPinRI    (int pinRI);
            int      getPinFlight();       void setPinFlight(int pinFlight);
            int      getPinStatus();       void setPinStatus(int pinStatus);
            String   getBroker();          void setBroker        (String broker);
            String   getTopicLed();        void setTopicLed      (String topicLed);
            String   getTopicInit();       void setTopicInit     (String topicInit);
            String   getTopicLedStatus();  void setTopicLedStatus(String topicLedStatus);
            String   toString();
    };
}