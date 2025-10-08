#include "DeviceHandler.hpp"

using namespace DeviceModule;

/* Constructor */
DeviceHandler::DeviceHandler(){}
DeviceHandler::DeviceHandler(DeviceModel devModel)
{
    this->devModel = {devModel};
}

/* Methods */
void DeviceHandler::initialization()
{
    SerialMon.begin(this->devModel.getBaudRate());

    pinMode(this->devModel.getPinLED(), OUTPUT);
    digitalWrite(this->devModel.getPinLED(), HIGH);

    this->fakeSer = new SoftwareSerial(this->devModel.getPinFRX(),
                                       this->devModel.getPinFTX());
    this->fakeSer->begin(this->devModel.getFBaudRate(), SWSERIAL_8N1);
}

void DeviceHandler::routine()
{
    if (this->fakeSer->available()) {
        SerialMon.print("Received: ");
        SerialMon.println(this->fakeSer->readStringUntil('\n'));
    }

    if (SerialMon.available()) {
        this->fakeSer->write(SerialMon.read());
    }
}

SoftwareSerial *DeviceHandler::getSerial()
{
    return this->fakeSer;
}