#include "button.h"
#include "Arduino.h"

BUTTON::BUTTON(int pin)
{
    buttonPin = pin;
    buttonState = NOT_PRESSED;
    buttonSelection = -1;
    buttonEvent = NO_EVENT;
    type = -1;
}

int BUTTON::readButton()
{
    int selectedButton = (analogRead(buttonPin) + 190) / 128;
    int newButtonSelection = buttonSelection; /* Ã  priori rien ne change */
    switch (buttonState)
    {
    case NOT_PRESSED:
        if (selectedButton < 9)
            buttonState = PUSHED;
        break;
    case PUSHED:
        if (selectedButton < 9)
        {
            buttonState = PRESSED;
            newButtonSelection = selectedButton;
        }
        else
        {
            buttonState = NOT_PRESSED;
        }
        break;
    case PRESSED:
        if (selectedButton == 9)
        {
            buttonState = NOT_PRESSED;
            newButtonSelection = -1;
        }
        break;
    }

    return newButtonSelection;
}

void BUTTON::readEvent()
{
    int selection = readButton();

    if (selection == buttonState)
        buttonEvent = NO_EVENT;
    if (selection >= 0 && buttonSelection == -1)
        buttonEvent = EVENT_PRESSED;
    if (selection == -1 && buttonSelection >= 0)
        buttonEvent = EVENT_RELEASED;
    buttonSelection = selection;
}

int BUTTON::getSelection()
{
    return buttonSelection;
}

byte BUTTON::getEvent()
{
    return buttonEvent;
}