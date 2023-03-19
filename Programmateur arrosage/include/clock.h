#ifndef CLOCK_h
#define CLOCK_h
#include <Arduino.h>

class CLOCK
{
private:
    int milliDivsecs;
    unsigned long currmillis;
    bool minlock;
    void updateMin();
    void updateHrs();
    void updateDays();
    void updateMonths();

public:
    unsigned long prevMillis;
    uint8_t secs;
    uint8_t mins;
    uint8_t hrs;
    uint8_t days;
    uint8_t months;
    uint16_t years;
    CLOCK();
    void updateTime(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint16_t *year);
};
#endif
