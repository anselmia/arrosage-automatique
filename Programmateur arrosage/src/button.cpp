#include "button.h"
#include "Arduino.h"

BUTTON::BUTTON(byte pin)
{
    buttonPin = pin;
    buttonState = NOT_PRESSED;
    buttonSelection = -1;
    buttonEvent = NO_EVENT;
}

int BUTTON::readButton()
{
    byte buttonNumber = (analogRead(buttonPin) + 64) / 128;
    int newButtonSelection = buttonSelection;

    switch (buttonState)
    {
    case NOT_PRESSED:
        if (buttonNumber < 8)
        {
            buttonState = PUSHED;
            break;
        case PUSHED:
            if (buttonNumber < 7)
            {
                buttonState = PRESSED;
                newButtonSelection = buttonNumber;
            }
            else
                buttonState = NOT_PRESSED;
            break;
        case PRESSED:
            if (buttonNumber == 8)
            {
                buttonState = NOT_PRESSED;
                newButtonSelection = -1;
            }
            break;
        }
    }
    return newButtonSelection;
}

void BUTTON::readEvent()
{
    int newButtonSelection = readButton();
    Serial.println(buttonSelection);
    Serial.println(newButtonSelection);
    if (newButtonSelection == buttonSelection)
        buttonEvent = NO_EVENT;
    if (newButtonSelection >= 0 && buttonSelection == -1)
        buttonEvent = EVENT_PRESSED;
    if (newButtonSelection == -1 && buttonSelection >= 0)
        buttonEvent = EVENT_RELEASED;

    buttonSelection = newButtonSelection;
}

byte BUTTON::getSelection()
{
    return buttonSelection;
}

byte BUTTON::getEvent()
{
    return buttonEvent;
}