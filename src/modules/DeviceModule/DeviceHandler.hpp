#pragma once

#include "Library.h"
#include "../../utils/models/DeviceModel/DeviceModel.hpp"
#include "../Abstract/AbstractModule.hpp"
#include "../Abstract/DataAcquisition.hpp"

using namespace DeviceNamespace;
using namespace AbstractModule;
using namespace DataAcquisition;

namespace DeviceModule {
    class DeviceHandler : public AbstractHandler, public DataHandler {
        private:
            DeviceModel devModel;
            SoftwareSerial *fakeSer;

        public:
            DeviceHandler();
            DeviceHandler(DeviceModel devModel);
            void initialization();
            void routine();
            SoftwareSerial *getSerial();
    };
}