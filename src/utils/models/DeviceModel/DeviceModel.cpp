#include "DeviceModel.hpp"

using namespace DeviceNamespace;

DeviceModel::DeviceModel(){};

DeviceModel::DeviceModel(
    uint32_t baudRate,
    int      pinLED,
    uint32_t fBaudRate,
    int      pinFTX,
    int      pinFRX
) {
    this->baudRate  = {baudRate};
    this->pinLED    = {pinLED};
    this->fBaudRate = {fBaudRate};
    this->pinFTX    = {pinFTX};
    this->pinFRX    = {pinFRX};
};

uint32_t DeviceModel::getBaudRate()  { return this->baudRate;  }; void DeviceModel::setBaudRate (uint32_t baudRate)  { this->baudRate  = baudRate;  };
int      DeviceModel::getPinLED()    { return this->pinLED;    }; void DeviceModel::setPinLED   (int pinLED)         { this->pinLED    = pinLED;    };
uint32_t DeviceModel::getFBaudRate() { return this->fBaudRate; }; void DeviceModel::setFBaudRate(uint32_t fBaudRate) { this->fBaudRate = fBaudRate; };
int      DeviceModel::getPinFTX()    { return this->pinFTX;    }; void DeviceModel::setPinFTX   (int pinFTX)         { this->pinFTX    = pinFTX;    };
int      DeviceModel::getPinFRX()    { return this->pinFRX;    }; void DeviceModel::setPinFRX   (int pinFRX)         { this->pinFRX    = pinFRX;    };
String   DeviceModel::toString()     { return "device []";     };