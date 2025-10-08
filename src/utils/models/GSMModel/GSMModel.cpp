#include "GSMModel.hpp"

using namespace GSMNamespace;

GSMModel::GSMModel(){};

GSMModel::GSMModel(
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
) {
    this->apn            = {apn};
    this->user           = {user};
    this->pass           = {pass};
    this->pin            = {pin};
    this->resource       = {resource};
    this->server         = {server};
    this->port           = {port};
    this->baudRate       = {baudRate};
    this->pinRX          = {pinRX};
    this->pinTX          = {pinTX};
    this->pinPWRKEY      = {pinPWRKEY};
    this->pinDTR         = {pinDTR};
    this->pinRI          = {pinRI};
    this->pinFlight      = {pinFlight};
    this->pinStatus      = {pinStatus};
    this->broker         = {broker};
    this->topicLed       = {topicLed};
    this->topicInit      = {topicInit};
    this->topicLedStatus = {topicLedStatus};
};

GSMModel::GSMModel(
    int pinRX,
    int pinTX,
    int pinPWRKEY,
    int pinDTR,
    int pinRI,
    int pinFlight,
    int pinStatus
) {
    this->pinRX     = {pinRX};
    this->pinTX     = {pinTX};
    this->pinPWRKEY = {pinPWRKEY};
    this->pinDTR    = {pinDTR};
    this->pinRI     = {pinRI};
    this->pinFlight = {pinFlight};
    this->pinStatus = {pinStatus};
};

String   GSMModel::getAPN()            { return this->apn;            }; void GSMModel::setAPN           (String apn)            { this->apn            = apn;            };
String   GSMModel::getUser()           { return this->user;           }; void GSMModel::setUser          (String user)           { this->user           = user;           };
String   GSMModel::getPass()           { return this->pass;           }; void GSMModel::setPass          (String pass)           { this->pass           = pass;           };
String   GSMModel::getPin()            { return this->pin;            }; void GSMModel::setPin           (String pin)            { this->pin            = pin;            };
String   GSMModel::getResource()       { return this->resource;       }; void GSMModel::setResource      (String resource)       { this->resource       = resource;       };
String   GSMModel::getServer()         { return this->server;         }; void GSMModel::setServer        (String server)         { this->server         = server;         };
uint16_t GSMModel::getPort()           { return this->port;           }; void GSMModel::setPort          (uint16_t port)         { this->port           = port;           };
uint32_t GSMModel::getBaudRate()       { return this->baudRate;       }; void GSMModel::setBaudRate      (uint32_t baudRate)     { this->baudRate       = baudRate;       };
int      GSMModel::getPinRX()          { return this->pinRX;          }; void GSMModel::setPinRX         (int pinRX)             { this->pinRX          = pinRX;          };
int      GSMModel::getPinTX()          { return this->pinTX;          }; void GSMModel::setPinTX         (int pinTX)             { this->pinTX          = pinTX;          };
int      GSMModel::getPinPWRKEY()      { return this->pinPWRKEY;      }; void GSMModel::setPinPWRKEY     (int pinPWRKEY)         { this->pinPWRKEY      = pinPWRKEY;      };
int      GSMModel::getPinDTR()         { return this->pinDTR;         }; void GSMModel::setPinDTR        (int pinDTR)            { this->pinDTR         = pinDTR;         };
int      GSMModel::getPinRI()          { return this->pinRI;          }; void GSMModel::setPinRI         (int pinRI)             { this->pinRI          = pinRI;          };
int      GSMModel::getPinFlight()      { return this->pinFlight;      }; void GSMModel::setPinFlight     (int pinFlight)         { this->pinFlight      = pinFlight;      };
int      GSMModel::getPinStatus()      { return this->pinStatus;      }; void GSMModel::setPinStatus     (int pinStatus)         { this->pinStatus      = pinStatus;      };
String   GSMModel::getBroker()         { return this->broker;         }; void GSMModel::setBroker        (String broker)         { this->broker         = broker;         };
String   GSMModel::getTopicLed()       { return this->topicLed;       }; void GSMModel::setTopicLed      (String topicLed)       { this->topicLed       = topicLed;       };
String   GSMModel::getTopicInit()      { return this->topicInit;      }; void GSMModel::setTopicInit     (String topicInit)      { this->topicInit      = topicInit;      };
String   GSMModel::getTopicLedStatus() { return this->topicLedStatus; }; void GSMModel::setTopicLedStatus(String topicLedStatus) { this->topicLedStatus = topicLedStatus; };
String   GSMModel::toString()          { return "gsm []";        };
