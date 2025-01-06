#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "config.h"

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Wait for Wi-Fi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi!");

    // Initialize TFT Display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 10);
    tft.println("Spotify Remote Ready!");
}

// Fetch playback data
void fetchPlaybackData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(SERVER_URL) + "/playback-data");
        int httpCode = http.GET();

        if (httpCode == 200) {
            String payload = http.getString();
            Serial.println(payload);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            String track = doc["track"];
            String artist = doc["artist"];
            bool isPlaying = doc["is_playing"];

            // Display on TFT
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(10, 30);
            tft.print("Track: " + track);
            tft.setCursor(10, 60);
            tft.print("Artist: " + artist);
            tft.setCursor(10, 90);
            tft.print(isPlaying ? "Playing" : "Paused");
        } else {
            Serial.println("Failed to fetch playback data");
        }
        http.end();
    }
}

// Send control commands
void sendControlCommand(String action) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(SERVER_URL) + "/control");
        http.addHeader("Content-Type", "application/json");

        String json = "{\"action\": \"" + action + "\"}";
        int httpResponseCode = http.POST(json);

        Serial.println(httpResponseCode);
        http.end();
    }
}

void loop() {
    fetchPlaybackData();
    delay(5000); // Refresh every 5 seconds

    // Example Button Controls
    if (digitalRead(12) == HIGH) {
        sendControlCommand("play");
    }
    if (digitalRead(13) == HIGH) {
        sendControlCommand("pause");
    }
}
