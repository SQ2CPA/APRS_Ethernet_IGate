#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <Arduino.h>
#include <vector>
#include <FS.h>

class APRSIS {
public:
    bool active;
    String passcode;
    String server;
    int port;
};

class TNC {
public:
    bool enableServer;
};

class Port {
public:
    int baudrate;
    String callsign;
};

class Configuration {
public:
    String callsign;  
    APRSIS aprsis;
    TNC tnc;
    Port portA;
    Port portB;
  
    void init();
    void writeFile();
    Configuration();

private:
    bool readFile();
    String _filePath;
};

#endif