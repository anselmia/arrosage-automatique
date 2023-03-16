#ifndef CLOCK_h
#define CLOCK_h

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
    int secs;
    int mins;
    int hrs;
    int days;
    int months;
    int years;
    CLOCK();
    void updateTime(int (&rtc)[7]);
};
#endif
