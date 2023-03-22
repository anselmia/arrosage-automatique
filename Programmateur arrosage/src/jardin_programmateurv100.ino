/* Code to relay module
  1,2,3,4,5,6,7 : select the output pin for desired ev
  100: Set the selected output to LOW
  101: Set the selected output to HIGH
  102: Set all output to low
*/
#include <Wire.h> //i2c
#include "Arduino.h"
#include "myeeprom.h"
#include <U8glib.h> // LCD screen
#include <avr/wdt.h>
#include <Adafruit_AHTX0.h> // AHT21
#include "button.h"
#include "ev.h"
// #include "screen.h"
#include "menu.h"

int versio = 1.0;

MYEEPROM eeprom = MYEEPROM();
/* EV pins*/
const int pin_ev1 = 0;
const int pin_ev2 = 1;
const int pin_ev3 = 2;
const int pin_ev4 = 3;
const int pin_ev5 = 4;
const int pin_ev6 = 5;

/* Error led pins*/
const int pin_led = 6;

/* Pin alim scrren */
const int pin_screen = 7;

EV arrayOfEV[6] = {EV(pin_ev1, 1), EV(pin_ev2, 2), EV(pin_ev3, 3), EV(pin_ev4, 4), EV(pin_ev5, 5), EV(pin_ev6, 6)};
// SCREEN screen;
// bouton
int button_state = 0;
const int pin_button_line1 = 0;
const int pin_button_line2 = 1;
const int pin_button_line3 = 3;
/* type of button
Line 1 :
0 : right
1 : up
2 : left
3 : down

Line 2 :
0 : +
1 : -

Line 3 :
0 : s6
1 : s5
2 : s4
3 : s3
4 : s2
5 : s1
*/
BUTTON arrayofButton[3] = {BUTTON(pin_button_line1), BUTTON(pin_button_line2), BUTTON(pin_button_line3)};
MENU menu = MENU();
boolean aar = true; // mode wire endtransmission
int module_state[2];
int manual_couter = 0;
char buf[20];

U8GLIB_ST7920_128X64_4X u8g(13, 11, 10);
Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;

void setup()
{
  wdt_enable(WDTO_4S); // watchdog 4 seconde reinitialisation
  Wire.begin();
  Wire.setClock(31000L); // reglage de horloge de i2c
  u8g.setColorIndex(1);
  DDRC |= _BV(2) | _BV(3); // POWER:Vcc Gnd
  PORTC |= _BV(3);         // VCC PINC3
  // initialisation de ds1307
  menu.initClock(module_state);

  Serial.begin(9600);
  Init();
  delay(3000);
}

void Init()
{
  // Init com with ds1307
  inii2c(0x68, 0);

  // Init com with aht21 sensor
  inii2c(0x38, 1);

  // Init EV
  for (int i = 0; i < 6; i++)
    arrayOfEV[i].init();

  // set output for error led an screen
  pinMode(pin_led, OUTPUT);
  pinMode(pin_screen, OUTPUT);

  // SWitch On the screen
  digitalWrite(pin_screen, HIGH);
  delay(200);

  // Init memory
  // eeprom.init_memory();
  u8g.setCursorFont(u8g_font_cursor);
  u8g.setCursorStyle(144);

  u8g.firstPage(); // SÃ©lectionne la 1er page mÃ©moire de l'Ã©cran
  do
  {
    u8g.setFont(u8g_font_tpss);            // Utilise la police de caractÃ¨re standard
    u8g.drawStr(30, 11, "Initialisation"); // 12 line
    if (module_state[1] == 1)
      u8g.drawStr(10, 22, "erreur horloge");

    u8g.drawStr(5, 44, "version");
    print_on_screen(50, 44, versio);
  } while (u8g.nextPage());
}

void inii2c(int adr, int i)
{
  Wire.beginTransmission(adr);
  module_state[i] = Wire.endTransmission(aar);
}

void select_button(int selected_button)
{
  switch (arrayofButton[0].getSelection())
  {
  case 1: // right
    arrayofButton[0].type = 0;
    menu.forward();
    break;
  case 5: // up
    arrayofButton[0].type = 1;
    menu.up();
    break;
  case 7: // left
    arrayofButton[0].type = 2;
    menu.backward();

    break;
  case 8: // down
    arrayofButton[0].type = 3;
    menu.down();
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: // +
    arrayofButton[1].type = 0;
    menu.updateValue(1);

    break;
  case 2: // -
    arrayofButton[1].type = 1;
    menu.updateValue(0);
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: // s6
    arrayofButton[2].type = 5;
    menu.selectedEV = 6;
    break;
  case 2: // s5
    arrayofButton[2].type = 4;
    menu.selectedEV = 5;
  case 3: // s4
    arrayofButton[2].type = 3;
    menu.selectedEV = 4;
    break;
  case 4: // s3
    arrayofButton[2].type = 2;
    menu.selectedEV = 3;
    break;
  case 5: // s2
    arrayofButton[2].type = 1;
    menu.selectedEV = 2;
    break;
  case 6: // s1
    arrayofButton[2].type = 0;
    menu.selectedEV = 1;
    break;
  }
}

// to remove
void select()
{
  byte selectedButton;
  selectedButton = Serial.read();
  switch (selectedButton)
  {
  case 54: // right
    arrayofButton[0].type = 0;
    menu.forward();
    button_state = 1;
    break;
  case 56: // up
    arrayofButton[0].type = 1;
    menu.up();
    button_state = 1;
    break;
  case 52: // left
    arrayofButton[0].type = 2;
    menu.backward();
    button_state = 1;
    break;
  case 50: // down
    arrayofButton[0].type = 3;
    menu.down();
    button_state = 1;
    break;
  }

  switch (selectedButton)
  {
  case 43: // +
    arrayofButton[1].type = 0;
    menu.updateValue(1);
    button_state = 1;
    break;
  case 45: // -
    arrayofButton[1].type = 1;
    menu.updateValue(0);
    button_state = 1;
    break;
  }

  switch (selectedButton)
  {
  case 121: // s6
    arrayofButton[2].type = 5;
    menu.selectedEV = 6;
    button_state = 1;
  case 116: // s5
    arrayofButton[2].type = 4;
    menu.selectedEV = 5;
    button_state = 1;
    break;
  case 114: // s4
    arrayofButton[2].type = 3;
    menu.selectedEV = 4;
    button_state = 1;
    break;
  case 101: // s3
    arrayofButton[2].type = 2;
    menu.selectedEV = 3;
    button_state = 1;
    break;
  case 122: // s2
    arrayofButton[2].type = 1;
    menu.selectedEV = 2;
    button_state = 1;
    break;
  case 97: // s1
    arrayofButton[2].type = 0;
    menu.selectedEV = 1;
    button_state = 1;
    break;
  case 18:
    break;
  }
}

void loop()
{
  wdt_reset(); // reset watchdog

  // to uncomment with real button
  // for (int i = 0; i < 3; i++)
  //{
  //   arrayofButton[i].readEvent();
  //   switch (arrayofButton[i].getEvent())
  //   {
  //   case arrayofButton[i].EVENT_PRESSED:
  //     if (button_state == 0)
  //     {
  //       Serial.println(F("Button Pressed"));
  //       select_button(arrayofButton[i].getSelection());
  //       button_state = 1;
  //     }
  //     break;
  //   case arrayofButton[i].EVENT_RELEASED:
  //     button_state = 0;
  //     break;
  //   }
  // }

  select(); // to remove()

  u8g.firstPage(); // Select the first memory page of the scrren
  do
  {
    print_screen();
  } while (u8g.nextPage()); // Select the next page

  check_inactiveScreen();
  loop_actualization();
  reset_button();
  check_error();
  button_state = 0; // to remove
}

void check_inactiveScreen()
{
  if (button_state == 1 && menu.inactive > 5)
  {
    main_screen();
    menu.inactive = 0;
    digitalWrite(pin_screen, HIGH);
  }

  if (button_state == 0 && menu.rtc_min != menu.min)
    menu.inactive++;

  if (menu.inactive > 5)
    digitalWrite(pin_screen, LOW);
}

void reset_button()
{
  arrayofButton[0].type = -1;
  arrayofButton[1].type = -1;
  arrayofButton[2].type = -1;
}

void check_error()
{
  if (module_state[0] > 0 || module_state[1] > 0)
  {
    digitalWrite(pin_led, HIGH);
  }
  else
    digitalWrite(pin_led, LOW);
}

void print_screen()
{
  // u8g.setFont(u8g_font_unifont);
  menu.getClock(module_state);

  if (menu.inactive < 5)
  {
    switch (menu.actualScreen)
    {
    case 0:
      main_screen();
      break;
    case 1:
      menu_screen();
      draw_cursor();
      break;
    case 2:
      parameter_screen();
      draw_cursor();
      break;
    case 4:
      auto_mode_screen();
      draw_cursor();
      break;
    case 6:
      clock_parameter_screen();
      draw_cursor();
      break;
    case 7:
      other_parameter_screen();
      draw_cursor();
      break;
    case 8:
      manual_mode_screen();
      draw_cursor();
      break;
    case 9:
      state_screen();
      draw_cursor();
      break;
    case 10:
      delay_screen();
      draw_cursor();
      break;
    case 11:
      stop_screen();
      draw_cursor();
      break;
    }
  }
  else
    u8g.disableCursor();
}

void main_screen()
{
  u8g.disableCursor();
  sprintf(buf, "%02d/%02d/%04d", menu.day, menu.month, menu.year);
  u8g.drawStr(34, 11, buf);

  sprintf(buf, "%02d:%02d:%02d", menu.hour, menu.min, menu.sec);
  u8g.drawStr(44, 21, buf);

  u8g.drawStr(15, 33, "1 : ");
  print_on_screen(30, 33, arrayOfEV[0].remainingTimeOn);
  u8g.drawStr(44, 33, ",");
  print_on_screen(50, 33, arrayOfEV[0].nextDayOn);
  u8g.drawStr(15, 44, "2 : ");
  print_on_screen(30, 44, arrayOfEV[1].remainingTimeOn);
  u8g.drawStr(44, 44, ",");
  print_on_screen(50, 44, arrayOfEV[1].nextDayOn);
  u8g.drawStr(15, 55, "3 : ");
  print_on_screen(30, 55, arrayOfEV[2].remainingTimeOn);
  u8g.drawStr(44, 55, ",");
  print_on_screen(50, 55, arrayOfEV[2].nextDayOn);
  u8g.drawStr(70, 33, "4 : ");
  print_on_screen(85, 33, arrayOfEV[3].remainingTimeOn);
  u8g.drawStr(100, 33, ",");
  print_on_screen(106, 33, arrayOfEV[3].nextDayOn);
  u8g.drawStr(70, 44, "5 : ");
  print_on_screen(85, 44, arrayOfEV[4].remainingTimeOn);
  u8g.drawStr(100, 44, ",");
  print_on_screen(106, 44, arrayOfEV[4].nextDayOn);
  u8g.drawStr(70, 55, "6 : ");
  print_on_screen(85, 55, arrayOfEV[5].remainingTimeOn);
  u8g.drawStr(100, 55, ",");
  print_on_screen(106, 55, arrayOfEV[5].nextDayOn);
}

void menu_screen()
{
  u8g.drawStr(15, 11, "Parametre");
  u8g.drawStr(15, 22, "Mode Manuel");
  u8g.drawStr(15, 33, "Gerer sortie");
  u8g.drawStr(15, 44, "Delais");
  u8g.drawStr(15, 55, "Stop");
}

void parameter_screen()
{
  u8g.drawStr(25, 11, "parametre");
  u8g.drawStr(15, 22, "Mode Auto");
  u8g.drawStr(15, 33, "Heure");
  u8g.drawStr(15, 44, "Divers");
}

void auto_mode_screen()
{
  u8g.drawStr(15, 11, "Sortie :");
  print_on_screen(70, 11, menu.selectedEV);
  // print auto mode state
  u8g.drawStr(15, 22, "Etat :");
  int mem_value = eeprom.Read(mem_autostate + (menu.selectedEV * 10));
  activate_screen(mem_value, 70, 22);
  // print auto time on
  u8g.drawStr(15, 33, "Duree :");
  mem_value = eeprom.Read(mem_autoTimeOn + (menu.selectedEV * 10));
  print_on_screen(70, 33, mem_value);
  u8g.drawStr(90, 33, "min");
  // print auto frequency
  u8g.drawStr(15, 44, "Tous les:");
  print_mem_value(70, 44, mem_autoFreq + (menu.selectedEV * 10));
  u8g.drawStr(90, 44, "j");
  // print auto start hour
  u8g.drawStr(15, 55, "Heure :");
  print_mem_value(70, 55, mem_autoStartHour + (menu.selectedEV * 10));
  u8g.drawStr(90, 55, "h");
}

void clock_parameter_screen()
{
  sprintf(buf, "Date : %02d/%02d/%04d", menu.day, menu.month, menu.year);
  u8g.drawStr(15, 22, buf);

  sprintf(buf, "Heure : %02d:%02d:%02d", menu.hour, menu.min, menu.sec);
  u8g.drawStr(15, 44, buf);
}

void other_parameter_screen()
{
  u8g.drawStr(50, 11, "Divers");
  switch (menu.actualLine)
  {
  case 1:
  case 2:
  case 3:
  case 4:
  {
    u8g.drawStr(15, 22, "Saison :");
    int mem_value = eeprom.Read(mem_autoSeason);
    activate_screen(mem_value, 70, 22);
    u8g.drawStr(15, 33, "Temp. :");
    print_mem_value(80, 33, mem_tempSeason);
    u8g.drawStr(95, 33, "deg");
    u8g.drawStr(15, 44, "Duree ete :");
    print_mem_value(80, 44, mem_sumerTimeon);
    u8g.drawStr(95, 44, "min");
    u8g.drawStr(15, 55, "Freq ete :");
    print_mem_value(80, 55, mem_sumerFreq);
    u8g.drawStr(95, 55, "j");
  }
  break;
  case 6:
  case 7:
    u8g.drawStr(15, 22, "Duree hiver :");
    print_mem_value(85, 22, mem_winterTimeon);
    u8g.drawStr(100, 22, "min");
    u8g.drawStr(15, 33, "Freq hivers :");
    print_mem_value(85, 33, mem_winterFreq);
    u8g.drawStr(100, 33, "j");
    break;
  }
}

void manual_mode_screen()
{
  u8g.drawStr(33, 11, "Mode manuel");
  u8g.drawStr(15, 22, "Sortie :");
  print_on_screen(70, 22, menu.selectedEV);
  u8g.drawStr(15, 33, "Duree :");
  print_on_screen(70, 33, menu.screenValue);
  u8g.drawStr(90, 33, "min");
}

void state_screen()
{
  u8g.drawStr(20, 11, " Activation ");
  u8g.drawStr(15, 22, "Sortie : ");
  print_on_screen(70, 22, menu.selectedEV);
  u8g.drawStr(15, 33, "Etat : ");
  int mem_value = eeprom.Read(mem_state + (menu.selectedEV * 10));
  activate_screen(mem_value, 70, 33);
}

void delay_screen()
{
  u8g.drawStr(20, 11, " Delais ");
  u8g.drawStr(15, 22, "Sortie : ");
  print_on_screen(70, 22, menu.selectedEV);
  u8g.drawStr(15, 33, "Duree :");
  print_on_screen(70, 33, menu.screenValue);
}

void stop_screen()
{
  u8g.drawStr(20, 11, " Stop ");
  u8g.drawStr(15, 22, "Sortie : ");
  print_on_screen(70, 22, menu.selectedEV);
}

/*int line 11 22 33 44 55*/
void draw_cursor()
{
  u8g.enableCursor();
  if (menu.actualLine > 5)
    u8g.setCursorPos(12, ((menu.actualLine - 5) * 11) + 5);
  else
    u8g.setCursorPos(12, (menu.actualLine * 11) + 5);
}

void draw_menu_edge()
{
  int x = 0;
  int y = 0;
  int length = 168;
  int height = 64;
  int r = 1;
  u8g.drawRFrame(x, y, length, height, r);
}

void update_auto_mode_with_ath21()
{
  int temp_value = -1;
  int humidity_value = -1;

  sensors_event_t humidity;
  sensors_event_t temp;
  aht_humidity->getEvent(&humidity);
  aht_temp->getEvent(&temp);

  temp_value = (int)temp.temperature;
  humidity_value = (int)humidity.relative_humidity;

  if (temp_value != 0)
  {
    int timeon;
    int freq;

    eeprom.write(mem_dayTemp, temp_value);
    eeprom.write(mem_dayHumidity, humidity_value);

    int temp_change_season;
    temp_change_season = eeprom.Read(mem_tempSeason);

    if (temp_value <= temp_change_season)
    {
      timeon = eeprom.Read(mem_winterTimeon);
      freq = eeprom.Read(mem_winterFreq);
    }
    else
    {
      timeon = eeprom.Read(mem_sumerTimeon);
      freq = eeprom.Read(mem_sumerFreq);
    }

    for (int i = 0; i < 6; i++)
      arrayOfEV[i].updateSeason(timeon, freq);

    module_state[1] = 0;
  }
  else
    module_state[1] = 1;
}

// Update all EV state based on clock
void loop_actualization()
{
  // EV delayed
  if (menu.delay == true)
  {
    arrayOfEV[menu.selectedEV - 1].nextDayOn += menu.screenValue;
    menu.delay = false;
  }

  // EV started manually
  if (menu.manual == true)
  {
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = menu.screenValue;
    menu.manual = false;
  }

  // EV stopped manually
  if (menu.stop == true)
  {
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
    menu.stop = false;
  }

  // Start every 5 min one EV mode
  if (menu.manual_all > 0)
  {
    if (manual_couter == 0)
      arrayOfEV[menu.manual_all - 1].remainingTimeOn = 5;

    if (menu.rtc_min != menu.min)
    {
      manual_couter++;
    }
    if (manual_couter > 5)
    {
      manual_couter = 0;
      menu.manual_all++;
    }

    if (menu.manual_all > 6)
      menu.manual_all = 0;
  }

  // Stop all EV
  if (menu.stop_all == true)
  {
    for (int i = 0; i < 6; i++)
      arrayOfEV[i].remainingTimeOn = 0;

    menu.stop_all = false;
  }

  // If EV has been desactivated, stop it
  for (int i = 0; i < 6; i++)
  {
    if (eeprom.Read(mem_state + (10 * (i + 1))) == 0)
    {
      arrayOfEV[i].remainingTimeOn = 0;
      arrayOfEV[i].nextDayOn = 0;
    }
  }

  // Update EV state every minute
  if (menu.rtc_min != menu.min)
  {
    int mem_value;
    // Check Temperature and humidity at 12 o'clock if auto mode on with aht21
    mem_value = eeprom.Read(mem_autoSeason);
    if (mem_value == 1)
    {
      if (menu.rtc_day != menu.day)
      {
        if (menu.hour == 12)
        {
          update_auto_mode_with_ath21();
        }
        menu.rtc_day = menu.day;
      }
    }
    menu.rtc_min = menu.min;

    // Calculate next day on and remaining time
    for (int i = 0; i < 6; i++)
    {
      arrayOfEV[i].updateRemainingTime(menu.hour, menu.day, menu.month, menu.year);
      arrayOfEV[i].update_state();
    }
  }
}

void print_mem_value(int col, int line, int mem_address)
{
  int mem_value;
  mem_value = eeprom.Read(mem_address);
  sprintf(buf, "%d", mem_value);
  u8g.drawStr(col, line, buf);
}

void print_on_screen(int col, int line, int num)
{
  sprintf(buf, "%d", num);
  u8g.drawStr(col, line, buf);
}

void activate_screen(int value, int x, int y)
{
  switch (value)
  {
  case 0:
    u8g.drawStr(x, y, "desactiver");
    break;
  case 1:
    u8g.drawStr(x, y, "activer");
    break;
  }
}