#include <ArduinoJson.h>
#include "configuration.h"
#include "web_utils.h"
#include "utils.h"

extern Configuration               Config;
extern std::vector<ReceivedPacket> receivedPackets;

extern const char web_index_html[] asm("_binary_data_embed_index_html_gz_start");
extern const char web_index_html_end[] asm("_binary_data_embed_index_html_gz_end");
extern const size_t web_index_html_len = web_index_html_end - web_index_html;

extern const char web_style_css[] asm("_binary_data_embed_style_css_gz_start");
extern const char web_style_css_end[] asm("_binary_data_embed_style_css_gz_end");
extern const size_t web_style_css_len = web_style_css_end - web_style_css;

extern const char web_script_js[] asm("_binary_data_embed_script_js_gz_start");
extern const char web_script_js_end[] asm("_binary_data_embed_script_js_gz_end");
extern const size_t web_script_js_len = web_script_js_end - web_script_js;

extern const char web_bootstrap_css[] asm("_binary_data_embed_bootstrap_css_gz_start");
extern const char web_bootstrap_css_end[] asm("_binary_data_embed_bootstrap_css_gz_end");
extern const size_t web_bootstrap_css_len = web_bootstrap_css_end - web_bootstrap_css;

extern const char web_bootstrap_js[] asm("_binary_data_embed_bootstrap_js_gz_start");
extern const char web_bootstrap_js_end[] asm("_binary_data_embed_bootstrap_js_gz_end");
extern const size_t web_bootstrap_js_len = web_bootstrap_js_end - web_bootstrap_js;


namespace WEB_Utils {

    AsyncWebServer server(80);

    void handleNotFound(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(404, "text/plain", "Not found");
        response->addHeader("Cache-Control", "max-age=3600");
        request->send(response);
    }

    void handleStatus(AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
    }

    void handleHome(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", (const uint8_t*)web_index_html, web_index_html_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    }

    void handleReadConfiguration(AsyncWebServerRequest *request) {
        File file = SPIFFS.open("/config.json");
        
        String fileContent;
        while(file.available()){
            fileContent += String((char)file.read());
        }

        request->send(200, "application/json", fileContent);
    }

    void handleReceivedPackets(AsyncWebServerRequest *request) {
        StaticJsonDocument<1536> data;

        for (int i = 0; i < receivedPackets.size(); i++) {
            data[i]["millis"] = receivedPackets[i].millis;
            data[i]["packet"] = receivedPackets[i].packet;
            data[i]["port"] = receivedPackets[i].port;
        }

        String buffer;

        serializeJson(data, buffer);

        request->send(200, "application/json", buffer);
    }

    void handleWriteConfiguration(AsyncWebServerRequest *request) {
        Serial.println("Got new config from www");

        Config.callsign = request->getParam("callsign", true)->value();

        Config.aprsis.active = request->hasParam("aprs_is.active", true);
        Config.aprsis.passcode = request->getParam("aprs_is.passcode", true)->value();
        Config.aprsis.server = request->getParam("aprs_is.server", true)->value();
        Config.aprsis.port = request->getParam("aprs_is.port", true)->value().toInt();

        Config.tnc.enableServer = request->hasParam("tnc.enableServer", true);

        Config.portA.baudrate = request->getParam("ports.A.baudrate", true)->value().toInt();
        Config.portA.callsign = request->getParam("ports.A.callsign", true)->value();

        Config.portB.baudrate = request->getParam("ports.B.baudrate", true)->value().toInt();
        Config.portB.callsign = request->getParam("ports.B.callsign", true)->value();

        Config.writeFile();

        AsyncWebServerResponse *response = request->beginResponse(302, "text/html", "");
        response->addHeader("Location", "/");
        request->send(response);

        ESP.restart();
    }

    void handleAction(AsyncWebServerRequest *request) {
        // String type = request->getParam("type", false)->value();
        String type = "reboot";

        if (type == "reboot") {
            ESP.restart();
        } else {
            request->send(404, "text/plain", "Not Found");
        }
    }

    void handleStyle(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", (const uint8_t*)web_style_css, web_style_css_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    }

    void handleScript(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", (const uint8_t*)web_script_js, web_script_js_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    }

    void handleBootstrapStyle(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", (const uint8_t*)web_bootstrap_css, web_bootstrap_css_len);
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", "max-age=3600");
        request->send(response);
    }

    void handleBootstrapScript(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", (const uint8_t*)web_bootstrap_js, web_bootstrap_js_len);
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", "max-age=3600");
        request->send(response);
    }

    void setup() {
        server.on("/", HTTP_GET, handleHome);
        server.on("/status", HTTP_GET, handleStatus);
        server.on("/received-packets.json", HTTP_GET, handleReceivedPackets);
        server.on("/configuration.json", HTTP_GET, handleReadConfiguration);
        server.on("/configuration.json", HTTP_POST, handleWriteConfiguration);
        server.on("/action", HTTP_POST, handleAction);
        server.on("/style.css", HTTP_GET, handleStyle);
        server.on("/script.js", HTTP_GET, handleScript);
        server.on("/bootstrap.css", HTTP_GET, handleBootstrapStyle);
        server.on("/bootstrap.js", HTTP_GET, handleBootstrapScript);

        server.onNotFound(handleNotFound);

        server.begin();
    }

}