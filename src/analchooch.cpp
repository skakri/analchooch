#include <Arduino.h>
#include <vector>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "../.piolibdeps/ArduinoJson_ID64/ArduinoJson.h"  // Ugly, fixme in cmake
#include "./analchooch.h"

ESP8266WiFiMulti WiFiMulti;

char analchooch_hostname[64] = {0};
std::vector<String> steam_friend_ids;

uint8_t pwmPin = 12;
bool set_up = false;

void setup_wifi() {
    delay(10);

    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(9600);

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    sprintf(analchooch_hostname, "choocher%06X", ESP.getChipId());
    WiFi.hostname(analchooch_hostname);

    setup_wifi();
    analogWriteFreq(100);
    pinMode(pwmPin, OUTPUT);
}

// Hack: Couldn't find matching resistors
// TODO(skakri): settings
// Lower 115 - larger spread
#define OUR_PWMRANGE (PWMRANGE - 115)

void reboot() {
    delay(500);
    Serial.println("reboot");
    ESP.restart();
}

void setup_steam() {
    HTTPClient http;
    bool setup_success = false;
    bool have_data = false;

    http.begin("http://api.steampowered.com/ISteamUser"
                   "/GetFriendList/v0001/?key=" + (String) STEAM_KEY +
                   "&steamid=" + (String) STEAM_USER_ID +
                   "&relationship=friend");
    int httpCode = http.GET();
    if (httpCode == 0) {
        Serial.println("http call failed");
        reboot();
        return;
    }

    String payload = http.getString();
    http.end();

    DynamicJsonBuffer friend_list;
    JsonObject& root = friend_list.parseObject(payload);

    if (!root.success()) {
        Serial.println("setup: parseObject() failed");
        reboot();
    } else {
        have_data = true;
    }

    if (have_data) {
        for (int i = 0; i < root["friendslist"]["friends"].size(); i++) {
            String steamid = root["friendslist"]["friends"][i]["steamid"];
            steam_friend_ids.push_back(steamid);
        }
        if (steam_friend_ids.size()) {
            setup_success = true;
        }
    }

    if (!setup_success) {
        Serial.println("No friends");
        reboot();
    }

    set_up = true;
}

void loop() {
    if (!set_up) {
        setup_steam();
    }

    HTTPClient http;
    http.setTimeout(10000);

    int online = 0;

    std::vector<std::vector<String>> chunked_steam_friend_ids = SplitVector(
        steam_friend_ids, 7);

    for (int j = 0; j < chunked_steam_friend_ids.size(); j++) {
        String steam_fried_id_chunk = "";
        for (int k = 0; k < chunked_steam_friend_ids[j].size(); ++k) {
            steam_fried_id_chunk += chunked_steam_friend_ids[j][k] + ",";
        }

        http.begin(
            ("http://api.steampowered.com/ISteamUser"
                "/GetPlayerSummaries/v0002/?key=" + (String) STEAM_KEY +
                "&steamids=" + steam_fried_id_chunk));
        int httpCode = http.GET();

        if (httpCode == 0) {
            Serial.println("http call failed");
            delay(10000);
            return;
        }

        String payload = http.getString();
        http.end();

        DynamicJsonBuffer summaries;
        JsonObject& root = summaries.parseObject(payload);

        if (!root.success()) {
            Serial.println("parseObject() failed");
            delay(500);
            return;
        }

        for (int i = 0; i < root["response"]["players"].size(); i++) {
            auto player = root["response"]["players"][i];
            unsigned int personastate = player["personastate"];
            if (personastate > 0) {
                online++;
            }
        }
    }
    Serial.println(online);

    analogWrite(pwmPin, (OUR_PWMRANGE / 50) * online);
    delay(5000);
}
