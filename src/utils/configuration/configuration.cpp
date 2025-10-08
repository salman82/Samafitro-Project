#include "./configuration.hpp"

using namespace DefaultConfiguration;

Configuration::Configuration() {
  this->devModel = {DeviceModel(
    this->DEVICE_BAUD,
    this->DEVICE_LED,
    this->DEVICE_FBAUD,
    this->DEVICE_FTX,
    this->DEVICE_FRX
  )};
  this->gsmModel = {GSMModel(
    this->GSM_APN,
    this->GSM_USER,
    this->GSM_PASS,
    this->GSM_PIN,
    this->GSM_RESOURCE,
    this->GSM_SERVER,
    this->GSM_PORT,
    this->GSM_BAUD,
    this->GSM_RX,
    this->GSM_TX,
    this->GSM_PWRKEY,
    this->GSM_DTR,
    this->GSM_RI,
    this->GSM_FLIGHT,
    this->GSM_STATUS,
    this->BROKER,
    this->TOPIC_LED,
    this->TOPIC_INIT,
    this->TOPIC_LED_STATUS
  )};
};

DeviceModel Configuration::getDeviceModel() { return this->devModel; };
GSMModel    Configuration::getGSMModel()    { return this->gsmModel; };