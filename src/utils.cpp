#include <WiFi.h>
#include "configuration.h"
#include "utils.h"

extern Configuration        Config;


namespace Utils {
    void print(String text) {
        Serial.print(text);
    }

    void println(String text) {
        Serial.println(text);
    }

}