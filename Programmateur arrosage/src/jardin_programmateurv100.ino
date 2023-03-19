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
0 : s7
1 : s6
2 : s5
3 : s4
4 : s3
5 : s2
6 : s1
*/
BUTTON arrayofButton[3] = {BUTTON(pin_button_line1), BUTTON(pin_button_line2), BUTTON(pin_button_line3)};
MENU menu = MENU();
boolean aar = true; // mode wire endtransmission
int module_state[2];
char buf[10];

U8GLIB_SSD1309_128X64 u8g(13, 11, 10, 9, 8);
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
}

void Init()
{

  delay(1000);
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
    u8g.setFont(u8g_font_tpss);           // Utilise la police de caractÃ¨re standard
    u8g.drawStr(5, 11, "Initialisation"); // 12 line
    if (module_state[1] == 0)
    {
      menu.getClock(module_state);
      print_actual_time();
    }
    else
    {
      u8g.drawStr(10, 22, "erreur horloge");
    }
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
    Serial.println(F("Forward")); // to remove after test
    menu.forward();
    break;
  case 5: // up
    arrayofButton[0].type = 1;
    Serial.println(F("upward")); // to remove after test
    menu.up();
    break;
  case 7: // left
    arrayofButton[0].type = 2;
    Serial.println(F("backard")); // to remove after test
    menu.backward();

    break;
  case 8: // down
    arrayofButton[0].type = 3;
    Serial.println(F("Downward"));
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
    menu.selectEV(6);
    break;
  case 2: // s5
    arrayofButton[2].type = 4;
    menu.selectEV(5);
  case 3: // s4
    arrayofButton[2].type = 3;
    menu.selectEV(4);
    break;
  case 4: // s3
    arrayofButton[2].type = 2;
    menu.selectEV(3);
    break;
  case 5: // s2
    arrayofButton[2].type = 1;
    menu.selectEV(2);
    break;
  case 6: // s1
    arrayofButton[2].type = 0;
    menu.selectEV(1);
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
    Serial.println(F("Forward")); // to remove after test
    menu.forward();
    button_state = 1;
    break;
  case 56: // up
    arrayofButton[0].type = 1;
    Serial.println(F("up")); // to remove after test
    menu.up();
    button_state = 1;
    break;
  case 52: // left
    arrayofButton[0].type = 2;
    Serial.println(F("backard")); // to remove after test
    menu.backward();
    button_state = 1;
    break;
  case 50: // down
    arrayofButton[0].type = 3;
    Serial.println(F("Downward"));
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
  case 117: // s7
    arrayofButton[2].type = 6;
    menu.selectEV(7);
    button_state = 1;
    break;
  case 121: // s6
    arrayofButton[2].type = 5;
    menu.selectEV(6);
    button_state = 1;
  case 116: // s5
    arrayofButton[2].type = 4;
    menu.selectEV(5);
    button_state = 1;
    break;
  case 114: // s4
    arrayofButton[2].type = 3;
    menu.selectEV(4);
    button_state = 1;
    break;
  case 101: // s3
    arrayofButton[2].type = 2;
    menu.selectEV(3);
    button_state = 1;
    break;
  case 122: // s2
    arrayofButton[2].type = 1;
    menu.selectEV(2);
    button_state = 1;
    break;
  case 97: // s1
    arrayofButton[2].type = 0;
    menu.selectEV(1);
    button_state = 1;
    break;
  case 18:
    break;
  }
}

void loop()
{
  wdt_reset(); // reset watchdog
  menu.getClock(module_state);
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
    u8g.setFont(u8g_font_tpss); // Use standard character
    print_screen();
  } while (u8g.nextPage()); // Select the next page

  delay(100);
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
  if (menu.inactive < 5)
  {
    if (menu.redraw == 0)
    {
      draw_cursor();
    }
    else if (menu.redraw == 1)
    {
      switch (menu.actualScreen)
      {
      case 0:
        main_screen();
        menu.redraw = -1;
        break;
      case 1:
        menu_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 2:
        parameter_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 4:
        auto_mode_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 6:
        clock_parameter_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 7:
        other_parameter_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 8:
        manual_mode_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 9:
        state_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 10:
        delay_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      case 11:
        stop_screen();
        menu.redraw = -1;
        draw_cursor();
        break;
      }
    }
  }
}

void main_screen()
{
  u8g.disableCursor();
  if (module_state[0] == 1)
  {
    u8g.drawStr(20, 11, "erreur");
    Serial.println(F("erreur"));
  }
  else
  {
    print_actual_time();
    print_ev_state();
  }
}

void print_actual_time()
{
  sprintf(buf, "%02d/%02d/%04d", menu.day, menu.month, menu.year);
  u8g.drawStr(20, 11, buf);
  Serial.println(buf);
  sprintf(buf, "%02d:%02d:%02d", menu.hour, menu.min, menu.sec);
  u8g.drawStr(40, 11, buf);
  Serial.println(buf);
}

void print_ev_state()
{
  u8g.drawStr(5, 33, "1 : ");
  print_on_screen(10, 33, arrayOfEV[0].remainingTimeOn);
  u8g.drawStr(20, 33, ",");
  print_on_screen(25, 22, arrayOfEV[0].nextDayOn);
  Serial.print(F("1 : "));
  Serial.print(arrayOfEV[0].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[0].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(5, 44, "2 : ");
  print_on_screen(10, 44, arrayOfEV[1].remainingTimeOn);
  u8g.drawStr(20, 44, ",");
  print_on_screen(25, 44, arrayOfEV[1].nextDayOn);
  Serial.print(F("2 : "));
  Serial.print(arrayOfEV[1].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[1].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(5, 55, "3 : ");
  print_on_screen(10, 55, arrayOfEV[2].remainingTimeOn);
  u8g.drawStr(20, 55, ",");
  print_on_screen(25, 55, arrayOfEV[2].nextDayOn);
  Serial.print(F("3 : "));
  Serial.print(arrayOfEV[2].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[2].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(40, 33, "sortie 4:");
  print_on_screen(45, 33, arrayOfEV[3].remainingTimeOn);
  u8g.drawStr(55, 33, ",");
  print_on_screen(70, 33, arrayOfEV[3].nextDayOn);
  Serial.print(F("4 : "));
  Serial.print(arrayOfEV[3].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[3].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(40, 44, "5 : ");
  print_on_screen(45, 44, arrayOfEV[4].remainingTimeOn);
  u8g.drawStr(55, 44, ",");
  print_on_screen(70, 44, arrayOfEV[4].nextDayOn);
  Serial.print(F("5 : "));
  Serial.print(arrayOfEV[4].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[4].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(40, 55, "6 : ");
  print_on_screen(45, 55, arrayOfEV[5].remainingTimeOn);
  u8g.drawStr(55, 55, ",");
  print_on_screen(70, 55, arrayOfEV[5].nextDayOn);
  Serial.print(F("6 : "));
  Serial.print(arrayOfEV[5].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[5].nextDayOn);
  Serial.println(F(""));
}

void menu_screen()
{
  u8g.drawStr(5, 11, "Parametre");
  u8g.drawStr(5, 22, "Mode Manuel");
  u8g.drawStr(5, 33, "Gerer sortie");
  u8g.drawStr(5, 44, "Delais");
  u8g.drawStr(5, 55, "Stop");
  Serial.println(F("Parametre "));
  Serial.println(F("Mode Manuel "));
  Serial.println(F("Gerer sortie"));
  Serial.println(F("Delais"));
  Serial.println(F("Stop"));
}

void parameter_screen()
{
  u8g.drawStr(10, 11, "parametre");
  u8g.drawStr(5, 22, "Mode Auto");
  u8g.drawStr(5, 33, "Heure");
  u8g.drawStr(5, 44, "Divers");
  Serial.println(F(" parametre "));
  Serial.println(F(" Mode Auto "));
  Serial.println(F(" Heure "));
  Serial.println(F(" Divers "));
}

void auto_mode_screen()
{
  u8g.drawStr(5, 11, "Sortie :");
  print_on_screen(70, 11, menu.selectedEV);
  Serial.print(F(" Sortie : "));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  // print auto mode state
  u8g.drawStr(5, 22, "Etat :");
  Serial.print(F(" Etat : "));
  int mem_value = eeprom.Read(mem_autostate + (menu.selectedEV * 10));
  activate_screen(mem_value, 70, 22);
  Serial.println(F(""));
  // print auto time on
  u8g.drawStr(5, 33, "Duree :");
  mem_value = eeprom.Read(mem_autoTimeOn + (menu.selectedEV * 10));
  print_on_screen(70, 33, mem_value);
  Serial.print(F(" Duree : "));
  Serial.print(mem_value);
  Serial.println(F(""));
  // print auto frequency
  u8g.drawStr(5, 44, "Tous les:");
  print_mem_value(70, 44, mem_autoFreq + (menu.selectedEV * 10));
  u8g.drawStr(110, 44, "j");
  Serial.print(F(" Tout les : "));
  Serial.print(eeprom.Read(mem_autoFreq + (menu.selectedEV * 10)));
  Serial.print(F(" j"));
  Serial.println(F(""));
  // print auto start hour
  u8g.drawStr(5, 56, "Heure :");
  print_mem_value(70, 55, mem_autoStartHour + (menu.selectedEV * 10));
  Serial.print(F(" Heure : "));
  Serial.print(eeprom.Read(mem_autoStartHour + (menu.selectedEV * 10)));
  Serial.println(F(""));
}

void clock_parameter_screen()
{
  menu.getClock(module_state);
  u8g.drawStr(10, 11, " Horloge ");
  Serial.println(F(" Horloge "));
  u8g.drawStr(5, 22, " Date :");
  print_on_screen(7, 22, menu.day);
  u8g.drawStr(20, 22, "/");
  print_on_screen(28, 22, menu.month);
  u8g.drawStr(40, 22, "/");
  print_on_screen(48, 22, menu.year);
  Serial.print(F(" Date : "));
  Serial.print(menu.day);
  Serial.print(F("/"));
  Serial.print(menu.month);
  Serial.print(F("/"));
  Serial.print(menu.year);
  Serial.println(F(""));
  u8g.drawStr(5, 44, " Heure :");
  print_on_screen(7, 44, menu.hour);
  u8g.drawStr(20, 44, ":");
  print_on_screen(25, 44, menu.min);
  u8g.drawStr(40, 44, ":");
  print_on_screen(45, 44, menu.sec);
  Serial.print(F(" Heure : "));
  Serial.print(menu.hour);
  Serial.print(F(":"));
  Serial.print(menu.min);
  Serial.print(F(":"));
  Serial.print(menu.sec);
  Serial.println(F(""));
}

void other_parameter_screen()
{
  Serial.println(F(" Divers "));
  u8g.drawStr(10, 11, "Divers");
  switch (menu.actualLine)
  {
  case 1:
  case 2:
  case 3:
  case 4:
  {
    u8g.drawStr(5, 22, "Auto ete/hiver :");
    Serial.print(F("Auto ete/hiver : "));
    int mem_value = eeprom.Read(mem_autoSeason);
    activate_screen(mem_value, 90, 22);
    Serial.println(F(""));
    u8g.drawStr(5, 33, "Temp ete/hiver :");
    print_mem_value(70, 33, mem_tempSeason);
    Serial.print(F("Temp ete/hiver : "));
    Serial.print(eeprom.Read(mem_tempSeason));
    Serial.println(F(""));
    u8g.drawStr(5, 44, "Duree ete :");
    print_mem_value(70, 44, mem_sumerTimeon);
    Serial.print(F("Duree ete : "));
    Serial.print(eeprom.Read(mem_sumerTimeon));
    Serial.println(F(""));
    u8g.drawStr(5, 55, "Freq ete :");
    print_mem_value(70, 55, mem_sumerFreq);
    Serial.print(F("Freq ete : "));
    Serial.print(eeprom.Read(mem_sumerFreq));
    Serial.println(F(""));
  }
  break;
  case 5:
  case 6:
    u8g.drawStr(5, 22, "Duree hiver :");
    print_mem_value(70, 22, mem_winterTimeon);
    Serial.print(F("Duree hiver : "));
    Serial.print(eeprom.Read(mem_winterTimeon));
    Serial.println(F(""));
    u8g.drawStr(5, 33, "Freq hivers :");
    print_mem_value(70, 33, mem_winterFreq);
    Serial.print(F("Freq hivers : "));
    Serial.print(eeprom.Read(mem_winterFreq));
    Serial.println(F(""));
    break;
  }
}

void manual_mode_screen()
{
  u8g.drawStr(10, 11, "Mode manuel");
  Serial.println(F(" Mode manuel "));
  u8g.drawStr(5, 22, "Sortie :");
  print_on_screen(70, 22, menu.selectedEV);
  Serial.print(F(" Sortie : "));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  u8g.drawStr(5, 33, "Duree :");
  print_on_screen(70, 33, menu.screenValue);
  Serial.print(F(" Duree : "));
  Serial.print(menu.screenValue);
  Serial.println(F(""));
}

void state_screen()
{
  u8g.drawStr(10, 11, " Activation ");
  Serial.println(F("Activation"));
  u8g.drawStr(5, 22, "Sortie : ");
  Serial.print(F("Sortie : "));
  print_on_screen(70, 22, menu.selectedEV);
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  u8g.drawStr(5, 33, "Etat : ");
  Serial.print(F("Etat : "));
  int mem_value = eeprom.Read(mem_state + (menu.selectedEV * 10));
  activate_screen(mem_value, 70, 33);
  Serial.println(F(""));
}

void delay_screen()
{
  u8g.drawStr(10, 11, " Delais ");
  u8g.drawStr(5, 22, "Sortie : ");
  print_on_screen(70, 22, menu.selectedEV);
  Serial.println(F("Delais :"));
  Serial.print(F("sortie  :"));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  u8g.drawStr(5, 33, "Duree :");
  print_on_screen(70, 33, menu.screenValue);
  Serial.print(F(" Duree : "));
  Serial.print(menu.screenValue);
  Serial.println(F(""));
}

void stop_screen()
{
  u8g.drawStr(10, 11, " Stop ");
  u8g.drawStr(5, 22, "Sortie : ");
  print_on_screen(70, 22, menu.selectedEV);
  Serial.println(F("Stop :"));
  Serial.print(F("Sortie  :"));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
}

/*int line 11 22 33 44 55*/
void draw_cursor()
{
  u8g.enableCursor();
  u8g.setCursorPos(2, menu.actualLine * 11);
  // int x = 0;
  // int y = menu.actualLine * 11;
  // int length = 4;
  // int height = 11;
  // u8g.drawBox(x, y, length, height);
  Serial.print(F("C"));
  Serial.print(menu.actualLine);
  Serial.println(F(""));
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

void loop_actualization()
{
  if (menu.delay == 1)
  {
    Serial.println("delay");
    arrayOfEV[menu.selectedEV - 1].nextDayOn += menu.screenValue;
    menu.delay = 0;
  }
  if (menu.manual == 1)
  {
    Serial.println("manual");
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = menu.screenValue;
    menu.manual = 0;
  }
  if (menu.stop == 1)
  {
    Serial.println("stop");
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
    menu.stop = 0;
  }

  // Check Temperature and humidity at 12 o'clock if auto mode on with aht21

  // Update menu last minute
  if (menu.rtc_min != menu.min)
  {
    int mem_value;
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
    Serial.print(F("desactiver"));
    break;
  case 1:
    u8g.drawStr(x, y, "activer");
    Serial.print(F("activer"));
    break;
  }
}