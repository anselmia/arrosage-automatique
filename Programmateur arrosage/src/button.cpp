#include "button.h"
#include "Arduino.h"

BUTTON::BUTTON(int pin)
{
    buttonPin = pin;
    buttonState = NOT_PRESSED;
    buttonSelection = -1;
    buttonEvent = NO_EVENT;
}

int BUTTON::readButton()
{
    int buttonNumber = (analogRead(buttonPin) + 64) / 128;
    //Serial.println(analogRead(buttonPin));
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
               //Serial.println(buttonNumber);
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

    if (newButtonSelection == buttonSelection)
        buttonEvent = NO_EVENT;
    if (newButtonSelection >= 0 && buttonSelection == -1)
        buttonEvent = EVENT_PRESSED;
    if (newButtonSelection == -1 && buttonSelection >= 0)
        buttonEvent = EVENT_RELEASED;

    buttonSelection = newButtonSelection;
}

int BUTTON::getSelection()
{
    return buttonSelection;
}

byte BUTTON::getEvent()
{
    return buttonEvent;
}