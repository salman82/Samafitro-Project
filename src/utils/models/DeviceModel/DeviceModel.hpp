#pragma once

#include <Arduino.h>
#include "../common/CommonModel.hpp"

using namespace CommonNamespace;

namespace DeviceNamespace {
    class DeviceModel : CommonModel {
        private:
            uint32_t baudRate;
            int      pinLED;
            uint32_t fBaudRate;
            int      pinFTX;
            int      pinFRX;
        public:
            DeviceModel();
            DeviceModel(
                uint32_t baudRate,
                int      pinLED,
                uint32_t fBaudRate,
                int      pinFTX,
                int      pinFRX
            );   
            uint32_t getBaudRate();  void setBaudRate (uint32_t baudRate);
            int      getPinLED();    void setPinLED   (int pinLED);
            uint32_t getFBaudRate(); void setFBaudRate(uint32_t fBaudRate);
            int      getPinFTX();    void setPinFTX   (int pinFTX);
            int      getPinFRX();    void setPinFRX   (int pinFRX);
            String   toString();
    };
}