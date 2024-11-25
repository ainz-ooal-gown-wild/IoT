#include "DataLogger.h"

DataLogger::DataLogger() {
    ramIndex = 0;
    eepromHour = -1;
}

void DataLogger::begin() {
    if (!rtc.begin()) {
        Serial.println("RTC nicht gefunden!");
        while (1);
    }
    
    // Falls RTC die Zeit verloren hat
    if (rtc.lostPower()) {
        Serial.println("RTC hat die Zeit verloren! Setze auf Kompilierzeit...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    // Gespeicherte Daten wiederherstellen
    restoreFromEEPROM();
}

void DataLogger::addMeasurement(float temp, float humidity, uint16_t pressure) {
    DateTime now = rtc.now();
    
    DataPoint newData = {
        .year = (uint8_t)(now.year() - 2000),
        .month = (uint8_t)now.month(),
        .day = (uint8_t)now.day(),
        .hour = (uint8_t)now.hour(),
        .minute = (uint8_t)now.minute(),
        .temp = (int16_t)(temp * 10),
        .hum = (uint16_t)(humidity * 10),
        .press = pressure,
        .status = 0x01  // Noch nicht synchronisiert
    };
    
    ramBuffer[ramIndex] = newData;
    ramIndex = (ramIndex + 1) % SRAM_BUFFER_SIZE;
    
    // Prüfen ob neue Stunde für EEPROM Backup
    if (now.hour() != eepromHour) {
        backupLastHourToEEPROM(now.hour());
        eepromHour = now.hour();
    }
}

void DataLogger::backupLastHourToEEPROM(uint8_t currentHour) {
    HourlyData hourData;
    hourData.validCount = 0;
    
    uint8_t lastHour = (currentHour + 23) % 24;
    
    // Rückwärts durch den RAM-Buffer suchen
    for (int i = ramIndex - 1; i >= 0 && hourData.validCount < 4; i--) {
        if (ramBuffer[i].hour == lastHour) {
            hourData.measurements[hourData.validCount++] = ramBuffer[i];
        }
    }
    
    if (hourData.validCount > 0) {
        // Ein Schreibvorgang für die ganze Stunde
        EEPROM.put(EEPROM_START + (lastHour * sizeof(HourlyData)), hourData);
    }
}

void DataLogger::restoreFromEEPROM() {
    DateTime now = rtc.now();
    
    // Letzte 24h aus EEPROM laden
    for (int h = 0; h < EEPROM_HOURS; h++) {
        HourlyData hourData;
        EEPROM.get(EEPROM_START + (h * sizeof(HourlyData)), hourData);
        
        // Alle gültigen Messungen dieser Stunde verarbeiten
        for (int m = 0; m < hourData.validCount; m++) {
            ramBuffer[ramIndex++] = hourData.measurements[m];
        }
    }
}

void DataLogger::syncWithServer() {
    for (int i = 0; i < ramIndex; i++) {
        if (ramBuffer[i].status & 0x01) {  // Noch nicht synchronisiert
            if (sendToServer(ramBuffer[i])) {
                ramBuffer[i].status &= ~0x01;  // Als synchronisiert markieren
            }
        }
    }
}

bool DataLogger::sendToServer(DataPoint &data) {
    // Werte für Übertragung aufbereiten
    float temp = data.temp / 10.0;
    float hum = data.hum / 10.0;
    
    // HTTP Request zusammenbauen
    String url = "http://your-server.com/data";
    String payload = String("timestamp=") + 
                    String(data.year + 2000) + "-" +
                    String(data.month) + "-" +
                    String(data.day) + " " +
                    String(data.hour) + ":" +
                    String(data.minute) + ":00" +
                    "&temp=" + String(temp) +
                    "&humidity=" + String(hum) +
                    "&pressure=" + String(data.press);
    
    // Hier den tatsächlichen HTTP POST Request implementieren
    // Beispiel mit WiFiClient oder ähnlichem
    // ...
    
    return true;  // Erfolgreich gesendet
}