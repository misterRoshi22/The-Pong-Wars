#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "PongWars.h"
#include "wifi_config.h"

const char *host = "thepongwars";

WebServer server(80);

uint8_t *framebuffer = NULL;
const int MAX_BLOCK_DIM = 60; 

PongWars* pongWars = nullptr;

void handleReset();
void handleChangeSize();

void setup() {
    Serial.begin(115200);
    delay(1000);
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    pongWars = new PongWars(20);

    WiFi.mode(WIFI_STA);

    const int num_networks = sizeof(wifi_networks) / sizeof(wifi_networks[0]);

    
    bool connected = false;
    for (int i = 0; i < num_networks && !connected; i++) {
        Serial.print("Trying to connect to: ");
        Serial.println(wifi_networks[i][0]);
        
        WiFi.begin(wifi_networks[i][0], wifi_networks[i][1]);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            Serial.print("Connected to: ");
            Serial.println(wifi_networks[i][0]);
        }
    }
    
    if (connected) {
        Serial.println(WiFi.localIP());
        
        if (MDNS.begin(host)) MDNS.addService("http", "tcp", 80);

        server.on("/reset", handleReset);
        server.on("/change_size", HTTP_POST, handleChangeSize);
        server.begin();
    }

    for (int i = 0; i < 1e6; i++) pongWars->step();

    epd_init();
    epd_poweron();
    epd_clear();
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) server.handleClient();
    pongWars->step();
    pongWars->draw();
    epd_push_pixels(epd_full_screen(), 1, 1);
}

void handleReset() { if (pongWars) pongWars->reset(); }

void handleChangeSize() {
    if (!server.hasArg("block_dim")) return;
    
    int block_dim = server.arg("block_dim").toInt();    
    if (60 % block_dim != 0) return;    
    
    if (pongWars) pongWars->reset(block_dim);
}