#include <WiFi.h>
#include "kiss_utils.h"
#include "kiss_protocol.h"
#include "configuration.h"
#include "utils.h"

extern Configuration Config;

#define MAX_CLIENTS 4

#define TNC_PORT 8000
#define TNC_PORT_A 8001
#define TNC_PORT_B 8002

WiFiClient* clients[MAX_CLIENTS];
WiFiClient* clientsA[MAX_CLIENTS];
WiFiClient* clientsB[MAX_CLIENTS];

WiFiServer tncServer(TNC_PORT);
WiFiServer tncServerA(TNC_PORT_A);
WiFiServer tncServerB(TNC_PORT_B);

String inputSerialBufferA = "";
String inputSerialBufferB = "";

namespace TNC_Utils {
    
    void setup() {
        if (Config.tnc.enableServer) {
            tncServer.stop();
            tncServer.begin();

            tncServerA.stop();
            tncServerA.begin();

            tncServerB.stop();
            tncServerB.begin();
        }
    }

    void checkNewClients() {
        WiFiClient new_client;

        new_client = tncServer.available();
        if (new_client.connected()) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                WiFiClient* client = clients[i];

                if (client == nullptr) {
                    clients[i] = new WiFiClient(new_client);

                    Utils::println("New TNC client connected (for ALL)");

                    break;
                }
            }
        }

        new_client = tncServerA.available();
        if (new_client.connected()) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                WiFiClient* client = clientsA[i];

                if (client == nullptr) {
                    clientsA[i] = new WiFiClient(new_client);

                    Utils::println("New TNC client connected (for PORT A)");

                    break;
                }
            }
        }

        new_client = tncServerB.available();
        if (new_client.connected()) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                WiFiClient* client = clientsB[i];

                if (client == nullptr) {
                    clientsB[i] = new WiFiClient(new_client);

                    Utils::println("New TNC client connected (for PORT B)");

                    break;
                }
            }
        }
    }

    void sendToClient(String packet) {
        const String kissEncoded = encodeKISS(packet);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            auto client = clients[i];
            if (client != nullptr) {
                if (client->connected()) {
                    client->print(kissEncoded);
                    client->flush();
                } else {
                    delete client;
                    clients[i] = nullptr;
                }
            }
        }

        Utils::print("---> Sent to TNC (ALL) : ");
        Utils::println(packet);
    }

    void sendToClientA(String packet) {
        const String kissEncoded = encodeKISS(packet);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            auto client = clientsA[i];
            if (client != nullptr) {
                if (client->connected()) {
                    client->print(kissEncoded);
                    client->flush();
                } else {
                    delete client;
                    clientsA[i] = nullptr;
                }
            }
        }

        Utils::print("---> Sent to TNC (A)   : ");
        Utils::println(packet);
    }

    void sendToClientB(String packet) {
        const String kissEncoded = encodeKISS(packet);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            auto client = clientsB[i];
            if (client != nullptr) {
                if (client->connected()) {
                    client->print(kissEncoded);
                    client->flush();
                } else {
                    delete client;
                    clientsB[i] = nullptr;
                }
            }
        }

        Utils::print("---> Sent to TNC (B)   : ");
        Utils::println(packet);
    }

    String handleInputData(char character, int port) {
        String* data;
        
        switch (port) {
            case 1:
            data = &inputSerialBufferA;
            break;
            case 2:
            data = &inputSerialBufferB;
            break;
            default:
            return "";
            break;
        }

        if (data->length() == 0 && character != (char)FEND) {
            return "";
        }

        String packet;

        data->concat(character);

        if (character == (char)FEND && data->length() > 3) {
            bool isDataFrame = false;
            const String& frame = decodeKISS(*data, isDataFrame);

            if (isDataFrame) {
                packet = frame;
            }

            data->clear();
        }

        if (data->length() > 255) {
            data->clear();
        }

        return packet;
    }

    void loop() {
        if (Config.tnc.enableServer) {
            checkNewClients();
        }
    }
}