#include <WiFi.h>
#include "configuration.h"
#include "aprs_is_utils.h"
#include "utils.h"

extern Configuration  Config;
extern WiFiClient     espClient;

namespace APRS_IS_Utils {

    void upload(String line) {
        espClient.print(line + "\r\n");
    }

    void connect() {
        uint8_t count = 0;
        String aprsauth;
        Serial.print("Connecting to APRS-IS ...     ");
        while (!espClient.connect(Config.aprsis.server.c_str(), Config.aprsis.port) && count < 20) {
            Serial.println("Didn't connect with server...");
            delay(1000);
            espClient.stop();
            espClient.flush();
            Serial.println("Run client.stop");
            Serial.println("Trying to connect with Server: " + String(Config.aprsis.server) + " AprsServerPort: " + String(Config.aprsis.port));
            count++;
            Serial.println("Try: " + String(count));
        }
        if (count == 20) {
            Serial.println("Tried: " + String(count) + " FAILED!");
        }
        else {
            Serial.println("Connected!\n(Server: " + String(Config.aprsis.server) + " / Port: " + String(Config.aprsis.port) + ")");

            aprsauth = "user " + Config.callsign + " pass " + Config.aprsis.passcode + " vers SQ2CPA_Igate_soft 1.0";
            upload(aprsauth);
            delay(200);
        }
    }

    String createPacket(String packet, String callsign) {
        return packet.substring(0, packet.indexOf(":")) + ",qAR," + callsign + packet.substring(packet.indexOf(":"));
    }

    void processLoRaPacket(String packet, String callsign) {
        if (espClient.connected()) {
            if (packet != "") {
                if (packet.indexOf("TCPIP") == -1 && packet.indexOf("NOGATE") == -1 && packet.indexOf("RFONLY") == -1) {
                    // String sender = packet.substring(3, packet.indexOf(">"));
                    String aprsPacket = createPacket(packet, callsign);

                    upload(aprsPacket);
                    Utils::println("---> Uploaded to APRS-IS");
                }
            }
        }
    }
}