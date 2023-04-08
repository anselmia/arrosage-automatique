#ifndef BUTTON_h
#define BUTTON_h
#include "Arduino.h"

static const byte NOT_PRESSED = 0;
static const byte PUSHED = 1;
static const byte PRESSED = 2;
static const byte NO_EVENT = 0;
static const byte EVENT_PRESSED = 1;
static const byte EVENT_RELEASED = 2;

class BUTTON
{
private:
    byte buttonPin;
    byte buttonState;
    int buttonSelection;
    byte buttonEvent;
    int readButton();

public:
    BUTTON(byte pin);
    int type;
    byte getSelection();
    byte getEvent();
    void readEvent();
};

#endif