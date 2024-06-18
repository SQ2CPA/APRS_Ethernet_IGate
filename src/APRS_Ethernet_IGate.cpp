#include <Arduino.h>
#include <SPI.h>
#include <ETH.h>
#include "configuration.h"
#include "aprs_is_utils.h"
#include "web_utils.h"
#include "tnc_utils.h"
#include "utils.h"
#include <SoftwareSerial.h>

Configuration Config;
WiFiClient espClient;

std::vector<ReceivedPacket> receivedPackets;

SoftwareSerial portA(4, 0);
SoftwareSerial portB(35, 0);

void WiFiEvent(WiFiEvent_t event)
{
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            Serial.println("ETH Started");
            ETH.setHostname("LoRa-IGate");
        break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("ETH Connected");
        break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.print("ETH MAC: ");
            Serial.print(ETH.macAddress());
            Serial.print(", IPv4: ");
            Serial.print(ETH.localIP());
            if (ETH.fullDuplex()) {
                Serial.print(", FULL_DUPLEX");
            }
            Serial.print(", ");
            Serial.print(ETH.linkSpeed());
            Serial.println("Mbps");
        break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("ETH Disconnected");
        break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("ETH Stopped");
        break;
        default:
        break;
    }
}

void setup() {
    Serial.begin(115200);

    portA.begin(Config.portA.baudrate);
    portB.begin(Config.portB.baudrate);

    WiFi.onEvent(WiFiEvent);
    ETH.begin();

    while (!ETH.linkUp()) ;

    WEB_Utils::setup();
    TNC_Utils::setup();
}

void loop() {
   
    if (Config.aprsis.active && !espClient.connected()) {
        APRS_IS_Utils::connect();
    }

    while (portA.available()) {
        String packet = TNC_Utils::handleInputData(portA.read(), 1);

        if (packet != "") {
            Utils::print("<--- Got packet PORT A : ");
            Utils::println(packet);

            if (Config.aprsis.active && espClient.connected()) {
                APRS_IS_Utils::processLoRaPacket(packet, Config.portA.callsign);
            }

            TNC_Utils::sendToClient(packet);
            TNC_Utils::sendToClientA(packet);

            ReceivedPacket receivedPacket;
            receivedPacket.millis = millis();
            receivedPacket.packet = packet;
            receivedPacket.port = 1;

            if (receivedPackets.size() >= 20) {
                receivedPackets.erase(receivedPackets.begin());
            }

            receivedPackets.push_back(receivedPacket);
        }
    }

    while (portB.available()) {
        String packet = TNC_Utils::handleInputData(portB.read(), 2);

        if (packet != "") {
            Utils::print("<--- Got packet PORT B : ");
            Utils::println(packet);

            if (Config.aprsis.active && espClient.connected()) {
                APRS_IS_Utils::processLoRaPacket(packet, Config.portB.callsign);
            }

            TNC_Utils::sendToClient(packet);
            TNC_Utils::sendToClientB(packet);

            ReceivedPacket receivedPacket;
            receivedPacket.millis = millis();
            receivedPacket.packet = packet;
            receivedPacket.port = 1;

            if (receivedPackets.size() >= 20) {
                receivedPackets.erase(receivedPackets.begin());
            }

            receivedPackets.push_back(receivedPacket);
        }
    }

}