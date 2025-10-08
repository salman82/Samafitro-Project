#pragma once

#include "Library.h"
#include "../../utils/models/GSMModel/GSMModel.hpp"
#include "../../utils/models/DeviceModel/DeviceModel.hpp"
#include "../Abstract/AbstractModule.hpp"
#include "../Abstract/DataAcquisition.hpp"

using namespace GSMNamespace;
using namespace AbstractModule;
using namespace DataAcquisition;
using namespace DeviceNamespace;

namespace GSMModule
{
    class GSMHandler : public AbstractHandler, public DataHandler
    {
    private:
        DeviceModel devModel;
        GSMModel gsmModel;

        TinyGsm *modem;
        TinyGsmClient *client;
        PubSubClient *mqtt;
        SoftwareSerial *fakeSer;

        #ifdef USING_TINYGPS_LIBRARY
            TinyGPSPlus *gps;
        #endif

        #ifdef DUMP_AT_COMMANDS
            StreamDebugger *debugger;
        #endif

        double latitude;
        double longitude;
        String systemMode;
        String mcc_mnc;
        String tac;
        String s_cell_id;
        String p_cell_id;
        String freq_band;
        String earfcn;
        String rsrq;
        String rsrp;
        String rssi;
        String rssnr;
        bool gps_status;
        int ledStatus = LOW;
        int led;

        struct Printer
        {
            String sysDescr               = "None";
            String sysUpTime              = "None";
            String sysName                = "None";
            String prtInputCurrentLevel11 = "None";
            String prtInputCurrentLevel12 = "None";
            String prtInputCurrentLevel13 = "None";
            String prtMarkerStatus        = "None";
            String prtMarkerLifeCount     = "None";
            String tonerLevelBlack        = "None";
            String tonerLevelCyan         = "None";
            String tonerLevelMagenta      = "None";
            String tonerLevelYellow       = "None";
            String arrOID[12] = {
                ".1.3.6.1.2.1.1.1.0",           // sysDescr
                ".1.3.6.1.2.1.1.3.0",           // sysUpTime
                ".1.3.6.1.2.1.1.5.0",           // sysName
                ".1.3.6.1.2.1.43.8.2.1.10.1.1", // prtInputCurrentLevel 1.1
                ".1.3.6.1.2.1.43.8.2.1.10.1.2", // prtInputCurrentLevel 1.2
                ".1.3.6.1.2.1.43.8.2.1.10.1.3", // prtInputCurrentLevel 1.3
                ".1.3.6.1.2.1.43.10.2.1.5.1.1", // prtMarkerStatus
                ".1.3.6.1.2.1.43.10.2.1.4.1.1", // prtMarkerLifeCount
                ".1.3.6.1.2.1.43.11.1.1.9.1.1", // prtMarkerSuppliesLevel 1.1
                ".1.3.6.1.2.1.43.11.1.1.9.1.2", // prtMarkerSuppliesLevel 1.2
                ".1.3.6.1.2.1.43.11.1.1.9.1.3", // prtMarkerSuppliesLevel 1.3
                ".1.3.6.1.2.1.43.11.1.1.9.1.4", // prtMarkerSuppliesLevel 1.4
            };
        };
        Printer printer;
        
        uint32_t lastReconnectAttempt;

    public:
        GSMHandler();
        GSMHandler(GSMModel gsmModel, DeviceModel devModel);
        void initialization();
        bool startup();
        void netCheck();
        bool mqttConnect();
        void mqttCallback(char *topic, byte *payload, unsigned int len);
        void getData();
        void showData();
        void insertData(String param, String value);
        void publish(String topic, String payload, int test);
        void publish();
        void resetRingCount();
        PubSubClient *getMQTT();
        void getInfo();
        void test(const char *obj, String param);
        String sendUSSD(String ussd);
        void powerdown();
        void light_sleep(uint32_t sec);
    };
}