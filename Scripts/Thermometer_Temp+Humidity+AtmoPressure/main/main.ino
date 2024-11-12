#include "DataLogger.h"

DataLogger logger;
unsigned long lastMeasurement = 0;
const unsigned long MEASURE_INTERVAL = 900000;  // 15 Minuten in Millisekunden

void setup() {
    Serial.begin(9600);
    logger.begin();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Alle 15 Minuten messen
    if (currentMillis - lastMeasurement >= MEASURE_INTERVAL) {
        // Hier Sensoren auslesen
        float temperature = readTemperature();    // Implementierung je nach Sensor
        float humidity = readHumidity();          // Implementierung je nach Sensor
        uint16_t pressure = readPressure();       // Implementierung je nach Sensor
        
        logger.addMeasurement(temperature, humidity, pressure);
        lastMeasurement = currentMillis;
        
        // Versuche zu synchronisieren
        logger.syncWithServer();
    }
}