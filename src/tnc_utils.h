#ifndef TNC_UTILS_H_
#define TNC_UTILS_H_

#include <Arduino.h>

namespace TNC_Utils {

    void setup();
    void loop();
    
    void sendToClient(String packet);
    void sendToClientA(String packet);
    void sendToClientB(String packet);
    String handleInputData(char character, int port);

}

#endif