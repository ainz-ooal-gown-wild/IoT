#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>  // Für Zeitstempel

// Datenstrukturen
struct DataPoint {
    // Timestamp komprimiert
    uint8_t year;    // Jahre seit 2000
    uint8_t month;   // 1-12
    uint8_t day;     // 1-31
    uint8_t hour;    // 0-23
    uint8_t minute;  // 0-59
    
    // Messwerte komprimiert
    int16_t temp;    // Temperatur * 10 (-3276.8°C bis +3276.7°C)
    uint16_t hum;    // Feuchtigkeit * 10 (0-100%)
    uint16_t press;  // Luftdruck in hPa (0-65535)
    
    uint8_t status;  // Status-Flags
} __attribute__((packed));

struct HourlyData {
    DataPoint measurements[4];  // Alle 4 15-Minuten Messungen
    uint8_t validCount;        // Anzahl gültiger Messungen (0-4)
} __attribute__((packed));

// Konstanten
const int SRAM_BUFFER_SIZE = 100;    // ~25h bei 15min Intervall
const int EEPROM_HOURS = 24;         // 24 Stunden Backup
const int EEPROM_START = 0;