#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "configuration.h"

void Configuration::writeFile() {
    Serial.println("Saving config..");

    StaticJsonDocument<2048> data;
    File configFile = SPIFFS.open("/config.json", "w");

    data["callsign"] = callsign;

    data["aprs_is"]["active"] = aprsis.active;
    data["aprs_is"]["passcode"] = aprsis.passcode;
    data["aprs_is"]["server"] = aprsis.server;
    data["aprs_is"]["port"] = aprsis.port;

    data["tnc"]["enableServer"] = tnc.enableServer;

    data["ports"]["A"]["baudrate"] = portA.baudrate;
    data["ports"]["A"]["callsign"] = portA.callsign;

    data["ports"]["B"]["baudrate"] = portB.baudrate;
    data["ports"]["B"]["callsign"] = portB.callsign;

    serializeJson(data, configFile);

    configFile.close();

    Serial.println("Config saved");
}

bool Configuration::readFile() {
    Serial.println("Reading config..");

    File configFile = SPIFFS.open("/config.json", "r");

    if (configFile) {
        StaticJsonDocument<2048> data;

        DeserializationError error = deserializeJson(data, configFile);
        if (error) {
            Serial.println("Failed to read file, using default configuration");
        }

        callsign = data["callsign"].as<String>();

        aprsis.passcode = data["aprs_is"]["passcode"].as<String>();
        aprsis.server = data["aprs_is"]["server"].as<String>();
        aprsis.port = data["aprs_is"]["port"].as<int>();
        aprsis.active = data["aprs_is"]["active"].as<bool>();

        tnc.enableServer = data["tnc"]["enableServer"].as<bool>();

        portA.baudrate = data["ports"]["A"]["baudrate"].as<int>();
        portA.callsign = data["ports"]["A"]["callsign"].as<String>();

        portB.baudrate = data["ports"]["B"]["baudrate"].as<int>();
        portB.callsign = data["ports"]["B"]["callsign"].as<String>();
        
        configFile.close();
        Serial.println("Config read successfuly");
        return true;
    } else {
        Serial.println("Config file not found");
        return false;
    }
}
    
void Configuration::init() {
    callsign = "N0CALL";

    aprsis.active = false;
    aprsis.passcode = "XYZVW";
    aprsis.server = "rotate.aprs2.net";
    aprsis.port = 14580;

    tnc.enableServer = false;

    portA.baudrate = 115200;
    portA.callsign = "N0CALL";

    portB.baudrate = 115200;
    portB.callsign = "N0CALL";
}

Configuration::Configuration() {
    if (!SPIFFS.begin(false)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    } else {
        Serial.println("SPIFFS Mounted");
    }

    bool exists = SPIFFS.exists("/config.json");
    if (!exists) {
        init();
        writeFile();
        ESP.restart();
    }

    readFile();
}