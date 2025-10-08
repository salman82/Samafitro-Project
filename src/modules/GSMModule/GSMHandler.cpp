#include "GSMHandler.hpp"
using namespace GSMModule;

static volatile int ringCount = 0;

/* Constructor */
GSMHandler::GSMHandler() {}
GSMHandler::GSMHandler(GSMModel gsmModel, DeviceModel devModel)
{
    this->gsmModel = {gsmModel};
    this->devModel = {devModel};

#ifdef DUMP_AT_COMMANDS
    this->debugger = new StreamDebugger(SerialAT, Serial);
    this->modem = new TinyGsm(*this->debugger);
#else
    this->modem = new TinyGsm(SerialAT);
#endif

    this->client = new TinyGsmClient(*this->modem);
    this->mqtt = new PubSubClient(*this->client);

#ifdef USING_TINYGPS_LIBRARY
    this->gps = new TinyGPSPlus();
#endif
}

/* Methods */
void GSMHandler::initialization()
{
    SerialAT.begin(this->gsmModel.getBaudRate(),
                   SERIAL_8N1,
                   this->gsmModel.getPinRX(),
                   this->gsmModel.getPinTX());
    delay(100);

    pinMode(this->gsmModel.getPinPWRKEY(), OUTPUT);
    digitalWrite(this->gsmModel.getPinPWRKEY(), HIGH);
    delay(300);
    digitalWrite(this->gsmModel.getPinPWRKEY(), LOW);

    pinMode(this->gsmModel.getPinFlight(), OUTPUT);
    digitalWrite(this->gsmModel.getPinFlight(), HIGH);

    // ***** GPS ***** //

    for (int i = 0; i < 3; ++i)
    {
        while (!(*this->modem).testAT(5000))
        {
            SerialMon.println("Try to start modem...");
            pinMode(this->gsmModel.getPinPWRKEY(), OUTPUT);
            digitalWrite(this->gsmModel.getPinPWRKEY(), HIGH);
            delay(300); // Need delay
            digitalWrite(this->gsmModel.getPinPWRKEY(), LOW);
        }
    }
    SerialMon.println("Modem Response Started.");

    // Stop GPS Server
    (*this->modem).sendAT("+CGPS=0");
    (*this->modem).waitResponse(30000);

    // Configure GNSS support mode
    (*this->modem).sendAT("+CGNSSMODE=15,1");
    (*this->modem).waitResponse(30000);

    // Configure NMEA sentence type
    (*this->modem).sendAT("+CGPSNMEA=200191");
    (*this->modem).waitResponse(30000);

    // Set NMEA output rate to 1HZ
    (*this->modem).sendAT("+CGPSNMEARATE=1");
    (*this->modem).waitResponse(30000);

    // Enable GPS
    (*this->modem).sendAT("+CGPS=1");
    (*this->modem).waitResponse(30000);

    // Download Report GPS NMEA-0183 sentence , NMEA TO AT PORT
    (*this->modem).sendAT("+CGPSINFOCFG=0,31");
    (*this->modem).waitResponse(30000);

    // Disable NMEA OUTPUT
    //  modem.sendAT("+CGPSINFOCFG=0,31");
    //  modem.waitResponse(30000);

    // ***** GPS ***** //

    SerialMon.println("Initializing modem...");
    if (!(*this->modem).init())
    {
        SerialMon.println("Failed to restart modem, delaying 10s and retrying");
    }

    bool ret = (*this->modem).setNetworkMode(2);
    SerialMon.printf("setNetworkMode: %d\n", ret);
    String name = (*this->modem).getModemName();
    SerialMon.printf("Modem Name    : %s\n", name.c_str());
    String modemInfo = (*this->modem).getModemInfo();
    SerialMon.printf("Modem Info    : %s\n", modemInfo.c_str());

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (this->gsmModel.getPin() && (*this->modem).getSimStatus() != 3)
    {
        (*this->modem).simUnlock(this->gsmModel.getPin().c_str());
    }
#endif

#if TINY_GSM_USE_WIFI
    // Wifi connection parameters must be set before waiting for the network
    SerialMon.print(F("Setting SSID/password..."));
    if (!this->modem.networkConnect(wifiSSID, wifiPass))
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
    // The XBee must run the gprsConnect function BEFORE waiting for network!
    (*this->modem).gprsConnect(this->gsmModel.getAPN().c_str(), this->gsmModel.getUser().c_str(), this->gsmModel.getPass().c_str());
#endif

    SerialMon.print("Waiting for network...");
    if (!(*this->modem).waitForNetwork())
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if ((*this->modem).isNetworkConnected())
    {
        SerialMon.println("Network connected");
    }

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(this->gsmModel.getAPN());

    String apn = this->gsmModel.getAPN();
    String user = this->gsmModel.getUser();
    String pass = this->gsmModel.getPass();

    if (!(*this->modem).gprsConnect(apn.c_str(), user.c_str(), pass.c_str()))
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if ((*this->modem).isGprsConnected())
    {
        SerialMon.println("GPRS connected");
    }
#endif

    // if (this->startup())
    // {
    //     SerialMon.println(F("***********************************************************"));
    //     SerialMon.println(F(" You can now send AT commands"));
    //     SerialMon.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
    //     SerialMon.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
    //     SerialMon.println(F(" DISCLAIMER: Entering AT commands without knowing what they do"));
    //     SerialMon.println(F(" can have undesired consiquinces..."));
    //     SerialMon.println(F("***********************************************************\n"));
    //
    //     // Uncomment to read received SMS
    //     // SerialAT.println("AT+CMGL=\"ALL\"");
    // }
    // else
    // {
    //     SerialMon.println(F("***********************************************************"));
    //     SerialMon.println(F(" Failed to connect to the modem! Check the baud and try again."));
    //     SerialMon.println(F("***********************************************************\n"));
    // }

    // ***** MQTT ***** //

    String temp = this->gsmModel.getBroker();
    (*this->mqtt).setServer(temp.c_str(), 2883);
    (*this->mqtt).setCallback([this](char *topic, byte *message, unsigned int length)
                              { this->mqttCallback(topic, message, length); });

    // ***** MQTT ***** //

    this->lastReconnectAttempt = 0;
}

bool GSMHandler::startup()
{
    bool reply = false;
    int i = 40;
    SerialMon.print(F("\r\n# Startup #\r\n"));
    SerialMon.print(F("# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
    while (i)
    {
        SerialAT.println(F("AT"));

        // Show the User: we are doing something.
        SerialMon.print(F("."));
        delay(500);

        // Did the Modem send something?
        if (SerialAT.available())
        {
            String r = SerialAT.readString();
            SerialMon.print("\r\n# Response:\r\n" + r);
            if (r.indexOf("OK") >= 0)
            {
                reply = true;
                break;
                ;
            }
            else
            {
                SerialMon.print(F("\r\n# "));
            }
        }

        // Did the User try to send something? Maybe he did not receive the first messages yet. Inform the User what is happening
        if (SerialMon.available() && !reply)
        {
            SerialMon.read();
            SerialMon.print(F("\r\n# Modem is not yet online."));
            SerialMon.print(F("\r\n# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
        }

        // On the 5th try: Inform the User what is happening
        if (i == 35)
        {
            SerialMon.print(F("\r\n# Modem did not yet answer. Probably Power loss?\r\n"));
            SerialMon.print(F("# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
        }
        delay(500);
        i--;
    }
    SerialMon.println(F("#\r\n"));

    return reply;
}

void GSMHandler::netCheck()
{
    if (!(*this->modem).isNetworkConnected())
    {
        SerialMon.println("Network disconnected");
        if (!(*this->modem).waitForNetwork(180000L, true))
        {
            SerialMon.println(" fail");
            delay(10000);
            return;
        }

        if ((*this->modem).isNetworkConnected())
        {
            SerialMon.println("Network reconnected");
        }

#if TINY_GSM_USE_GPRS
        if (!(*this->modem).isGprsConnected())
        {
            SerialMon.println("GPRS disconnected!");
            SerialMon.print(F("Connecting to "));
            SerialMon.print(this->gsmModel.getAPN());

            String apn = this->gsmModel.getAPN();
            String user = this->gsmModel.getUser();
            String pass = this->gsmModel.getPass();

            if (!(*this->modem).gprsConnect(apn.c_str(), user.c_str(), pass.c_str()))
            {
                SerialMon.println(" fail");
                delay(10000);
                return;
            }

            if ((*this->modem).isGprsConnected())
            {
                SerialMon.println("GPRS reconnected");
            }
        }
#endif
    }

    if (!(this->mqtt)->connected())
    {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - this->lastReconnectAttempt > 10000L)
        {
            this->lastReconnectAttempt = t;
            if (this->mqttConnect())
            {
                this->lastReconnectAttempt = 0;
            }
        }
        delay(100);
        return;
    }

    (this->mqtt)->loop();
}

bool GSMHandler::mqttConnect()
{
    SerialMon.print("Connecting to ");
    String brokerStr = this->gsmModel.getBroker();
    SerialMon.print(brokerStr);

    (*this->mqtt).setServer(brokerStr.c_str(), 2883);
    boolean status = (*this->mqtt).connect("client", "iot", "iot");

    if (status == false)
    {
        SerialMon.println(" fail");
        return false;
    }

    SerialMon.println(" success");
    // (*this->mqtt).publish(this->gsmModel.getTopicInit().c_str(), "sewacopy started");
    // (*this->mqtt).subscribe(this->gsmModel.getTopicLed().c_str());
    return (*this->mqtt).connected();
}

void GSMHandler::mqttCallback(char *topic, byte *payload, unsigned int len)
{
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, len);
    SerialMon.println();

    // Only proceed if incoming message's topic matches
    if (String(topic) == this->gsmModel.getTopicLed())
    {
        ledStatus = !ledStatus;
        digitalWrite(this->devModel.getPinLED(), ledStatus);
        SerialMon.print("ledStatus:");
        SerialMon.println(ledStatus);
        (*this->mqtt).publish(this->gsmModel.getTopicLedStatus().c_str(), ledStatus ? "1" : "0");
    }
}

void GSMHandler::getData()
{
    if (SerialMon.available())
    {
        String temp = SerialMon.readStringUntil('\n');
        String arrOID = this->printer.arrOID;

        // parsing process
        int index = temp.indexOf('|');
        if (index > 0)
        {
            if (strcmp(temp.substring(0, index).c_str(), arrOID[0].c_str()) == 32)
            {
                this->printer.sysDescr = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[1].c_str()) == 32)
            {
                this->printer.sysUpTime = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[2].c_str()) == 32)
            {
                this->printer.sysName = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[3].c_str()) == 32)
            {
                this->printer.prtInputCurrentLevel11 = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[4].c_str()) == 32)
            {
                this->printer.prtInputCurrentLevel12 = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[5].c_str()) == 32)
            {
                this->printer.prtInputCurrentLevel13 = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[6].c_str()) == 32)
            {
                this->printer.prtMarkerStatus = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[7].c_str()) == 32)
            {
                this->printer.prtMarkerLifeCount = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[8].c_str()) == 32)
            {
                this->printer.tonerLevelBlack = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[9].c_str()) == 32)
            {
                this->printer.tonerLevelCyan = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[10].c_str()) == 32)
            {
                this->printer.tonerLevelMagenta = temp.substring(index + 2);
            }
            else if (strcmp(temp.substring(0, index).c_str(), arrOID[11].c_str()) == 32)
            {
                this->printer.tonerLevelYellow = temp.substring(index + 2);
            }
        }
    }
}

void GSMHandler::showData()
{
    SerialMon.printf("//*** *** *** *** *** PRINTER DATA *** *** *** *** ***//\n");
    SerialMon.printf("sysDescr                : %s\n", this->printer.sysDescr.c_str());
    SerialMon.printf("sysUpTime               : %s\n", this->printer.sysUpTime);
    SerialMon.printf("sysName                 : %s\n", this->printer.sysName);
    SerialMon.printf("prtInputCurrentLevel1.1 : %s\n", this->printer.prtInputCurrentLevel11);
    SerialMon.printf("prtInputCurrentLevel1.2 : %s\n", this->printer.prtInputCurrentLevel12);
    SerialMon.printf("prtInputCurrentLevel1.3 : %s\n", this->printer.prtInputCurrentLevel13);
    SerialMon.printf("prtMarkerStatus         : %s\n", this->printer.prtMarkerStatus);
    SerialMon.printf("prtMarkerLifeCount      : %s\n", this->printer.prtMarkerLifeCount);
    SerialMon.printf("tonerLevelBlack         : %s\n", this->printer.tonerLevelBlack);
    SerialMon.printf("tonerLevelCyan          : %s\n", this->printer.tonerLevelCyan);
    SerialMon.printf("tonerLevelMagenta       : %s\n", this->printer.tonerLevelMagenta);
    SerialMon.printf("tonerLevelYellow        : %s\n", this->printer.tonerLevelYellow);
    SerialMon.printf("//*** *** *** *** *** PRINTER DATA *** *** *** *** ***//\n");
}

void GSMHandler::insertData(String param, String value)
{
    param = value;
}

void GSMHandler::publish(String topic, String payload, int test = 0)
{
    (*this->mqtt).publish(topic.c_str(), payload.c_str());
}

void GSMHandler::publish()
{
    String output;
    // JsonDocument doc0;
    // doc0["sysDescr"]                = this->printer.sysDescr;
    // doc0["sysUpTime"]               = this->printer.sysUpTime;
    // doc0["sysName"]                 = this->printer.sysName;
    // doc0["prtInputCurrentLevel1.1"] = this->printer.prtInputCurrentLevel11;
    // doc0["prtInputCurrentLevel1.2"] = this->printer.prtInputCurrentLevel12;
    // doc0["prtInputCurrentLevel1.3"] = this->printer.prtInputCurrentLevel13;
    // serializeJson(doc0,output);

    output = "{" + this->printer.sysDescr + "," +
             this->printer.sysUpTime + "," +
             this->printer.sysName + "," +
             this->printer.prtInputCurrentLevel11 + "," +
             this->printer.prtInputCurrentLevel12 + "," +
             this->printer.prtInputCurrentLevel13 + "," +
             this->printer.prtMarkerStatus + "," +
             this->printer.prtMarkerLifeCount + "," +
             "[" + this->printer.tonerLevelBlack + "," +
             this->printer.tonerLevelCyan + "," +
             this->printer.tonerLevelMagenta + "," +
             this->printer.tonerLevelYellow +
             "]," +
             this->latitude + "," +
             this->longitude + "," +
             this->systemMode + "," +
             this->mcc_mnc + "," +
             this->tac + "," +
             this->s_cell_id + "," +
             this->p_cell_id + "," +
             this->freq_band + "," +
             this->earfcn + "," +
             this->rsrq + "," +
             this->rsrp + "," +
             this->rssi + "," +
             this->rssnr + "}";

    (*this->mqtt).publish("sewacopy/id0/1", output.c_str());

    // JsonDocument doc1;
    // doc1["sysDescr"]                = this->printer.sysDescr;
    // doc1["sysUpTime"]               = this->printer.sysUpTime;
    // doc1["sysName"]                 = this->printer.sysName;
    // doc1["prtMarkerStatus"]         = this->printer.prtMarkerStatus;
    // doc1["prtMarkerLifeCount"]      = this->printer.prtMarkerLifeCount;
    // doc1["tonerLevelBlack"]         = this->printer.tonerLevelBlack;
    // doc1["tonerLevelCyan"]          = this->printer.tonerLevelCyan;
    // doc1["tonerLevelMagenta"]       = this->printer.tonerLevelMagenta;
    // doc1["tonerLevelYellow"]        = this->printer.tonerLevelYellow;
    // serializeJson(doc1,output);
    // (*this->mqtt).publish("sewacopy/id0/2",output.c_str());

    // JsonDocument doc2;
    // doc2["sysDescr"]                = this->printer.sysDescr;
    // doc2["sysUpTime"]               = this->printer.sysUpTime;
    // doc2["sysName"]                 = this->printer.sysName;
    // doc2["latitude"]                = this->latitude;
    // doc2["longitude"]               = this->longitude;
    // doc2["cellID"]                  = this->cellID;
    // serializeJson(doc2,output);
    // (*this->mqtt).publish("sewacopy/id0/3",output.c_str());
}

void GSMHandler::getInfo()
{
#ifdef USING_TINYGPS_LIBRARY
    while (SerialAT.available())
    {
        if ((*this->gps).encode(SerialAT.read()))
        {
            if ((*this->gps).location.isValid())
            {
                this->latitude = (*this->gps).location.lat();
                this->longitude = (*this->gps).location.lng();
            }
        }
    }
#else
    if (SerialAT.available())
    {
        if (SerialAT.available())
        {
            SerialMon.write(SerialAT.read());
        }

        if (SerialMon.available())
        {
            SerialAT.write(SerialMon.read());
        }
    }
#endif

    (*this->modem).sendAT(GF("+CPSI?"));
    (*this->modem).waitResponse(GF("\r\n"));
    String msg = (*this->modem).stream.readStringUntil('\n');
    (*this->modem).waitResponse();

    int index = msg.indexOf(":");
    msg = msg.substring(index + 2);

    char msgArray[msg.length() + 1];
    msg.toCharArray(msgArray, msg.length() + 1);

    char *parts[15];
    index = 0;
    char *token = strtok(msgArray, ",");
    while (token != NULL && index < 15)
    {
        parts[index++] = token;
        token = strtok(NULL, ",");
    }
    this->systemMode = parts[0];
    this->mcc_mnc = parts[1];
    this->tac = parts[2];
    this->s_cell_id = parts[3];
    this->p_cell_id = parts[4];
    this->freq_band = parts[5];
    this->earfcn = parts[6];
    this->rsrq = parts[7];
    this->rsrp = parts[8];
    this->rssi = parts[9];
    this->rssnr = parts[10];
}

void GSMHandler::test(const char *obj, String param)
{
    if (obj == "GPRS")
    {
        /*
            Preferred mode selection : AT+CNMP
            2 – Automatic
            13 – GSM Only
            14 – WCDMA Only
            38 – LTE Only
            59 – TDS-CDMA Only
            9 – CDMA Only
            10 – EVDO Only
            19 – GSM+WCDMA Only
            22 – CDMA+EVDO Only
            48 – Any but LTE
            60 – GSM+TDSCDMA Only
            63 – GSM+WCDMA+TDSCDMA Only
            67 – CDMA+EVDO+GSM+WCDMA+TDSCDMA Only
            39 – GSM+WCDMA+LTE Only
            51 – GSM+LTE Only
            54 – WCDMA+LTE Only
        */
        String ret;
        //    do {
        //        ret = (*this->modem).setNetworkMode(2);
        //        delay(500);
        //    } while (ret != "OK");
        ret = (*this->modem).setNetworkMode(2);
        DBG("setNetworkMode:", ret);
        uint8_t mode = (*this->modem).getGNSSMode();
        DBG("GNSS Mode:", mode);

        /*
            CGNSSMODE: <gnss_mode>,<dpo_mode>
            This command is used to configure GPS, GLONASS, BEIDOU and QZSS support mode.
            gnss_mode:
                0 : GLONASS
                1 : BEIDOU
                2 : GALILEO
                3 : QZSS
            dpo_mode :
                0 disable
                1 enable
        */
        (*this->modem).setGNSSMode(1, 1);
        this->light_sleep(1);

        String name = (*this->modem).getModemName();
        DBG("Modem Name:", name);
        String modemInfo = (*this->modem).getModemInfo();
        DBG("Modem Info:", modemInfo);

        // Unlock your SIM card with a PIN if needed
        if (this->gsmModel.getPin() && (*this->modem).getSimStatus() != 3)
        {
            String pin = this->gsmModel.getPin();
            (*this->modem).simUnlock(pin.c_str());
        }

        DBG("Waiting for network...");
        if (!(*this->modem).waitForNetwork(600000L))
        {
            light_sleep(10);
            return;
        }

        if ((*this->modem).isNetworkConnected())
        {
            DBG("Network connected");
        }

        DBG("Connecting to", this->gsmModel.getAPN());
        if (!(*this->modem).gprsConnect(this->gsmModel.getAPN().c_str(), this->gsmModel.getUser().c_str(), this->gsmModel.getPass().c_str()))
        {
            light_sleep(10);
            return;
        }

        bool res = (*this->modem).isGprsConnected();
        DBG("GPRS status:", res ? "connected" : "not connected");
        String ccid = (*this->modem).getSimCCID();
        DBG("CCID:", ccid);
        String imei = (*this->modem).getIMEI();
        DBG("IMEI:", imei);
        String imsi = (*this->modem).getIMSI();
        DBG("IMSI:", imsi);
        String cop = (*this->modem).getOperator();
        DBG("Operator:", cop);
        IPAddress local = (*this->modem).localIP();
        DBG("Local IP:", local);
        int csq = (*this->modem).getSignalQuality();
        DBG("Signal quality:", csq);
    }

    else if (obj == "TCP")
    {
#if TINY_GSM_TEST_TCP && defined TINY_GSM_MODEM_HAS_TCP
        this->client = new TinyGsmClient(*this->modem, 0);
        DBG("Connecting to ", this->gsmModel.getServer());

        if (!(this->client)->connect(this->gsmModel.getServer().c_str(), this->gsmModel.getPort()))
        {
            DBG("... failed");
        }
        else
        {
            // Make a HTTP GET request:
            (this->client)->print(String("GET ") + this->gsmModel.getResource() + " HTTP/1.0\r\n");
            (this->client)->print(String("Host: ") + this->gsmModel.getServer() + "\r\n");
            (this->client)->print("Connection: close\r\n\r\n");

            // Wait for data to arrive
            uint32_t start = millis();
            while ((this->client)->connected() && !(this->client)->available() && millis() - start < 30000L)
            {
                delay(100);
            };

            // Read data
            start = millis();
            while ((this->client)->connected() && millis() - start < 5000L)
            {
                while ((this->client)->available())
                {
                    SerialMon.write((this->client)->read());
                    start = millis();
                }
            }
            (this->client)->stop();
        }
#endif
    }

    else if (obj == "CALL")
    {
        DBG("Calling:", param);
        SerialAT.println("ATD" + param + ";");
        (*this->modem).waitResponse();
        light_sleep(20);
    }

    else if (obj == "USSD")
    {
        (*this->modem).sendAT("+CUSD=2");
        (*this->modem).waitResponse(30000L);
        // String answer = (*this->modem).sendUSSD(code);
        String answer = this->sendUSSD(param);
        // SerialMon.print("answer: ");
        SerialMon.println(answer);
        (*this->modem).sendAT("+CUSD=2");
        (*this->modem).waitResponse(30000L);

        // String ussd_phone_num = (*this->modem).sendUSSD("*161#");
        // DBG("Phone number (USSD):", ussd_phone_num);
    }

    else if (obj == "TEMP")
    {
#if defined TINY_GSM_MODEM_HAS_TEMPERATURE
        float temp = (*this->modem).getTemperature();
        DBG("Chip temperature:", temp);
#endif
    }

    else if (obj == "TIME")
    {
#if defined TINY_GSM_MODEM_HAS_TIME
        int year3 = 0;
        int month3 = 0;
        int day3 = 0;
        int hour3 = 0;
        int min3 = 0;
        int sec3 = 0;
        float timezone = 0;
        for (int8_t i = 5; i; i--)
        {
            DBG("Requesting current network time");
            if ((*this->modem).getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone))
            {
                DBG("Year:", year3, "\tMonth:", month3, "\tDay:", day3);
                DBG("Hour:", hour3, "\tMinute:", min3, "\tSecond:", sec3);
                DBG("Timezone:", timezone);
                break;
            }
            else
            {
                DBG("Couldn't get network time, retrying in 15s.");
                light_sleep(15);
            }
        }
        DBG("Retrieving time again as a string");
        String time = (*this->modem).getGSMDateTime(DATE_FULL);
        DBG("Current Network Time:", time);
#endif
    }

    else if (obj == "GPS")
    {
#ifdef USING_TINYGPS_LIBRARY
        SerialMon.print(F("Location: "));
        if ((*this->gps).location.isValid())
        {
            SerialMon.print((*this->gps).location.lat(), 6);
            SerialMon.print(F(","));
            SerialMon.print((*this->gps).location.lng(), 6);
        }
        else
        {
            SerialMon.print(F("INVALID"));
        }

        SerialMon.print(F("  Date/Time: "));
        if ((*this->gps).date.isValid())
        {
            SerialMon.print((*this->gps).date.month());
            SerialMon.print(F("/"));
            SerialMon.print((*this->gps).date.day());
            SerialMon.print(F("/"));
            SerialMon.print((*this->gps).date.year());
        }
        else
        {
            SerialMon.print(F("INVALID"));
        }

        SerialMon.print(F(" "));
        if ((*this->gps).time.isValid())
        {
            if ((*this->gps).time.hour() < 10)
                SerialMon.print(F("0"));
            SerialMon.print((*this->gps).time.hour());
            SerialMon.print(F(":"));
            if ((*this->gps).time.minute() < 10)
                SerialMon.print(F("0"));
            SerialMon.print((*this->gps).time.minute());
            SerialMon.print(F(":"));
            if ((*this->gps).time.second() < 10)
                SerialMon.print(F("0"));
            SerialMon.print((*this->gps).time.second());
            SerialMon.print(F("."));
            if ((*this->gps).time.centisecond() < 10)
                SerialMon.print(F("0"));
            SerialMon.print((*this->gps).time.centisecond());
        }
        else
        {
            SerialMon.print(F("INVALID"));
        }

        SerialMon.println();
#endif
    }
}

String GSMHandler::sendUSSD(String ussd)
{
    this->resetRingCount();
    (*this->modem).sendAT(GF("+CMGF=1"));
    (*this->modem).waitResponse();

    // Set preferred message format to text mode
    (*this->modem).sendAT(GF("+CSCS=\"IRA\""));
    (*this->modem).waitResponse();

    // Send the message
    (*this->modem).sendAT(GF("+CUSD=1,\""), ussd, GF("\",15"));
    if ((*this->modem).waitResponse() != 1)
    {
        return "";
    }
    if ((*this->modem).waitResponse(30000L, GF("+CUSD:")) != 1)
    {
        return "";
    }

    // Read response
    (*this->modem).stream.readStringUntil('"');
    String str = (*this->modem).stream.readStringUntil('"');

    return str;
}

void GSMHandler::powerdown()
{
    (*this->modem).poweroff();
    DBG("Poweroff.");
}

void GSMHandler::light_sleep(uint32_t sec)
{
    esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
    esp_light_sleep_start();
}

void GSMHandler::resetRingCount()
{
    ringCount = 0;
}

PubSubClient *GSMHandler::getMQTT()
{
    return this->mqtt;
}
