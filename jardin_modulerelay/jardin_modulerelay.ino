#include <Wire.h> //i2c

const int pin_ev1 = 2;
const int pin_ev2 = 3;
const int pin_ev3 = 4;
const int pin_ev4 = 5;
const int pin_ev5 = 6;
const int pin_ev6 = 7;
const int pin_ev7 = 8;
const int pin_ev8 = 9;
int selected_output = 0;

void setup()
{
  // put your setup code here, to run once:
  Wire.begin(9);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register read event
  Serial.begin(9600);

  // define the pin as selected_output
  pinMode(pin_ev1, OUTPUT);
  pinMode(pin_ev2, OUTPUT);
  pinMode(pin_ev3, OUTPUT);
  pinMode(pin_ev4, OUTPUT);
  pinMode(pin_ev5, OUTPUT);
  pinMode(pin_ev6, OUTPUT);
  pinMode(pin_ev7, OUTPUT);
  pinMode(pin_ev8, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

void receiveEvent(int howMany)
{
  int electrovanne_select = Wire.read(); // receive byte as an integer
  switch (electrovanne_select)
  {
  case 1:
    selected_output = pin_ev1;
    break;
  case 2:
    selected_output = pin_ev2;
    break;
  case 3:
    selected_output = pin_ev3;
    break;
  case 4:
    selected_output = pin_ev4;
    break;
  case 5:
    selected_output = pin_ev5;
    break;
  case 6:
    selected_output = pin_ev6;
    break;
  case 7:
    selected_output = pin_ev7;
    break;
  case 8:
    selected_output = pin_ev8;
    break;
  case 100: // Set the selected output to LOW
    if (selected_output != 0)
    {
      write_output(selected_output, 0);
      selected_output = 0;
    }
    break;
  case 101: // Set the selected output to HIGH
    if (selected_output != 0)
    {
      write_output(selected_output, 1);
      selected_output = 0;
    }
    break;
  case 102: // Set all output to low
    write_output(pin_ev1, 0);
    write_output(pin_ev2, 0);
    write_output(pin_ev3, 0);
    write_output(pin_ev4, 0);
    write_output(pin_ev5, 0);
    write_output(pin_ev6, 0);
    write_output(pin_ev7, 0);
    write_output(pin_ev8, 0);
    break;
  } // print the integer
}

void write_output(int pin_num, int val)
{
  // To check
  if (val == 0)
  {
    digitalWrite(pin_num, LOW);
  }
  else
  {
    digitalWrite(pin_num, HIGH);
  }
}