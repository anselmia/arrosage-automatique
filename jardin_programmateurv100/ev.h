#ifndef EV_h
#define EV_h

class EV
{
private:
    int evPin;
    int evState;

public:
    EV(int pin);
    void turnON();
    void turnOFF();
    int nextDayOn;
    int getState();
    int remainingTimeOn;
};
#endif