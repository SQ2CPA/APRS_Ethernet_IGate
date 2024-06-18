#ifndef APRS_IS_UTILS_H_
#define APRS_IS_UTILS_H_

#include <Arduino.h>


namespace APRS_IS_Utils {

    void upload(String line);
    void connect();
    String createPacket(String unprocessedPacket, String callsign);
    void processLoRaPacket(String packet, String callsign);

}

#endif