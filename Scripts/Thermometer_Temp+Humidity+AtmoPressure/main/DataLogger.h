#pragma once
#include "data_structures.h"

class DataLogger {
private:
    DataPoint ramBuffer[SRAM_BUFFER_SIZE];
    int ramIndex;
    int eepromHour;
    RTC_DS3231 rtc;  // RTC für genaue Zeit
    
    void backupLastHourToEEPROM(uint8_t currentHour);
    bool sendToServer(DataPoint &data);

public:
    DataLogger();
    void begin();
    void addMeasurement(float temp, float humidity, uint16_t pressure);
    void restoreFromEEPROM();
    void syncWithServer();
};