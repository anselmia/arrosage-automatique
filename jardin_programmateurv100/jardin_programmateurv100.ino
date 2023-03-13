/* Code to relay module
  1,2,3,4,5,6,7,8 : select the output pin for desired ev
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
#include "m_item.h"

int versio = 1.0;
/* memory address
0 : 20:90 auto time on for ev
1 : 21:91 auto frequency for ev
2 : 22:92 auto mode state for ev
3 : 23:93 auto start time for ev
4 : 27:97 active state for ev
5 : 150 day temp at 12
    151 day humidity at 12
6 : 160 auto mode with ath21 sensor
    161 winter/ete temp
    162 duration summer
    163 frequency summer
    164 duration winter
    165 frequency winter
7 : 170:173 manual time value from button 1 to 4
*/
MYEEPROM eeprom = MYEEPROM();
int mem_address[8][8] = {{20, 30, 40, 50, 60, 70, 80, 90}, {21, 31, 41, 51, 61, 71, 81, 91}, {22, 32, 42, 52, 62, 72, 82, 92}, {23, 33, 43, 53, 63, 73, 83, 93}, {27, 37, 47, 57, 67, 77, 87, 97}, {150, 151}, {160, 161, 162, 163, 164, 165}, {170, 171, 172, 173, 174}};
EV arrayOfEV[8] = {EV(2), EV(3), EV(4), EV(5), EV(6), EV(7), EV(8), EV(9)};
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
2 : B4
3 : B3
4 : B2
5 : B1

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
int module_state[3];
int manual_mode_state = 0;
int manual_time_on = 0;
char buf[10];

const int addr_mod_relay = 9;
const int max_time_on_ev = 20;
U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;
int error = 0;

void setup()
{
  wdt_enable(WDTO_4S); // watchdog 4 seconde reinitialisation
  Wire.begin();
  Wire.setClock(31000L); // reglage de horloge de i2c
  u8g.setColorIndex(1);
  DDRC |= _BV(2) | _BV(3); // POWER:Vcc Gnd
  PORTC |= _BV(3);         // VCC PINC3
  // initialisation de ds1307
  menu.initClock();

  Serial.begin(9600);
  Init();
}

void send_to_module(int adr, int val)
{
  Wire.beginTransmission(adr);
  Wire.write(val);
  error = Wire.endTransmission(aar);
}

void init_ev_state()
{
  // Set all EV off
  send_to_module(addr_mod_relay, 102);
  for (int i = 0; i < 8; i++)
    arrayOfEV[i].remainingTimeOn = 0;
}

void select_button(int selected_button)
{
  int action;
  switch (arrayofButton[0].getSelection())
  {
  case 1: // right
    menu.prinheu();
    arrayofButton[0].type = 0;
    Serial.println(F("Forward")); // to remove after test
    action = menu.forward();
    break;
  case 5: // up
    menu.prinheu();
    arrayofButton[0].type = 1;
    Serial.println(F("up")); // to remove after test
    action = menu.up();
    break;
  case 7: // left
    menu.prinheu();
    arrayofButton[0].type = 2;
    Serial.println(F("backard")); // to remove after test
    action = menu.backward();
    break;
  case 8: // down
    menu.prinheu();
    arrayofButton[0].type = 3;
    Serial.println(F("down")); // to remove after test
    action = menu.down();
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: //+
    menu.prinheu();
    arrayofButton[1].type = 0;
    menu.updateValue(1);
    // switch (selected_screen)
    //{
    // case 3:
    // case 4:
    // case 9:
    //   break;
    // case 10:
    //   init_ev_state();
    //   break;
    // case 8:
    //   init_ev_state();
    //   selected_screen = 0;
    //  break;
    // default:
    //  selected_screen = 2;
    //  break;
    //}
    break;
  case 2: //-
    menu.prinheu();
    arrayofButton[1].type = 1;
    menu.updateValue(1);
    // if (selected_screen == 0)
    //{
    //   subScreen_selected = 0;
    //   selected_line_on_screen = 0;
    //   selected_parameter = 0;
    // }
    break;
  case 3: // b4
    menu.prinheu();
    arrayofButton[1].type = 2;
    break;
  case 4: // b3
    menu.prinheu();
    arrayofButton[1].type = 3;
    break;
  case 5: // b2
    menu.prinheu();
    arrayofButton[1].type = 4;
    break;
  case 6: // b1
    menu.prinheu();
    arrayofButton[1].type = 5;
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: // s7
    arrayofButton[2].type = 6;
    menu.selectedEV = 7;
    break;
  case 2: // s6
    arrayofButton[2].type = 5;
    menu.selectedEV = 6;
  case 3: // s5
    arrayofButton[2].type = 4;
    menu.selectedEV = 5;
    break;
  case 4: // s4
    arrayofButton[2].type = 3;
    menu.selectedEV = 4;
    break;
  case 5: // s3
    arrayofButton[2].type = 2;
    menu.selectedEV = 3;
    break;
  case 6: // s2
    arrayofButton[2].type = 1;
    menu.selectedEV = 2;
    break;
  case 7: // s1
    arrayofButton[2].type = 0;
    menu.selectedEV = 1;
    break;
  case 8:
    break;
  }
}

void loop()
{
  wdt_reset(); // reset watchdog

  for (int i = 0; i < 3; i++)
  {
    arrayofButton[i].readEvent();
    switch (arrayofButton[i].getEvent())
    {
    case arrayofButton[i].EVENT_PRESSED:
      if (button_state == 0)
      {
        select_button(arrayofButton[i].getSelection());
        button_state = 1;
      }
      break;
    case arrayofButton[i].EVENT_RELEASED:
      button_state = 0;
      break;
    }
  }

  u8g.firstPage(); // Select the first memory page of the scrren
  do
  {
    u8g.setFont(u8g_font_tpss); // Use standard character
    menu.getClock();
    print_screen();
  } while (u8g.nextPage()); // Select the next page

  delay(100);
  loop_actualization();
  update_all_ev_state();

  // if ev not in active state reset remining time to 0
  for (int i = 0; i < 8; i++)
  {
    if (eeprom.Read(mem_address[4][i]) == 0)
    {
      if (arrayOfEV[i].remainingTimeOn != 0)
      {
        arrayOfEV[i].remainingTimeOn = 0;
      }
    }
  }

  reset_button();
}

void reset_button()
{
  arrayofButton[0].type = -1;
  arrayofButton[1].type = -1;
  arrayofButton[2].type = -1;
}

void print_screen()
{
  switch (menu.actualScreen)
  {
  case 0:
    main_screen();
    break;
  case 1:
    parameter_screen();
    break;
  case 2:
    output_screen();
    break;
  case 3:
    auto_mode_screen();
    break;
  case 4:
    buttons_parameter_screen();
    break;
  case 5:
    clock_parameter_screen();
    break;
  case 6:
    other_parameter_screen();
    break;
  case 7:
    manual_mode_screen();
    break;
  case 8:
    active_mode_screen();
    break;
  case 9:
    delay_screen();
    break;
  case 10:
    stop_screen();
    break;
  default:
    main_screen();
    break;
  }
}

void inii2c(int adr)
{
  Wire.beginTransmission(adr);
  error = Wire.endTransmission(aar);
}

void print_actual_time()
{
  Serial.println(F(" Heure "));
  // Serial.print(menu.rtc[4]);
  // Serial.print(F("/"));
  // Serial.print(menu.rtc[5]);
  // Serial.print(F("/"));
  // Serial.print(menu.rtc[6]);
  // Serial.println(F(""));
  // Serial.print(menu.rtc[2]);
  // Serial.print(F(":"));
  // Serial.print(menu.rtc[1]);
  // Serial.print(F(":"));
  // Serial.print(menu.rtc[0]);
  // Serial.println(F(""));
  u8g.drawStr(10, 11, " Heure ");
  print_on_screen(7, 22, menu.rtc[4]);
  u8g.drawStr(20, 22, "/");
  print_on_screen(28, 22, menu.rtc[5]);
  u8g.drawStr(40, 22, "/");
  print_on_screen(48, 22, menu.rtc[6]);
  print_on_screen(7, 33, menu.rtc[2]);
  u8g.drawStr(20, 33, ":");
  print_on_screen(25, 33, menu.rtc[1]);
  u8g.drawStr(40, 33, ":");
  print_on_screen(45, 33, menu.rtc[0]);
}

void Init()
{
  delay(1000);
  // Init com with arduino relay module
  inii2c(addr_mod_relay);
  if (error != 0)
  {
    module_state[0] = 1;
  }
  // Init com with ds1307
  inii2c(0x68);
  if (error != 0)
  {
    module_state[1] = 1;
  }
  // Init com with aht21 sensor
  inii2c(0x38);
  if (error != 0)
  {
    module_state[2] = 1;
  }

  init_ev_state();

  u8g.firstPage(); // SÃ©lectionne la 1er page mÃ©moire de l'Ã©cran
  do
  {
    u8g.setFont(u8g_font_tpss);           // Utilise la police de caractÃ¨re standard
    u8g.drawStr(5, 11, "Initialisation"); // 12 line
    Serial.println(F("Initialisation"));
    if (module_state[1] == 0)
    {
      menu.getClock();
      print_actual_time();
    }
    else
    {
      u8g.drawStr(10, 22, "erreur horloge");
      Serial.println(F("Initialisation"));
    }
    u8g.drawStr(5, 44, "version");
    Serial.print(F("Version : "));
    print_on_screen(50, 44, versio);
    Serial.print(versio);
    Serial.println("");
  } while (u8g.nextPage());

  delay(2000);
}

void main_screen()
{
  manual_mode_state = 0;
  if (error == 1)
  {
    u8g.drawStr(20, 11, "erreur");
    Serial.println(F("erreur"));
  }
  else
  {
    print_actual_time();
  }
  print_active_ev();
}

void parameter_screen()
{
  u8g.drawStr(10, 11, "parametre");
  u8g.drawStr(5, 22, "Sorties");
  u8g.drawStr(5, 33, "Boutons");
  u8g.drawStr(5, 44, "Heure");
  u8g.drawStr(5, 55, "Autre");
  Serial.println(F(" parametre "));
  Serial.println(F(" Sorties "));
  Serial.println(F(" Boutons "));
  Serial.println(F(" Heure "));
  Serial.println(F(" Autre "));
}

void output_screen()
{
  u8g.drawStr(10, 11, "les sorties");
  switch (menu.actualLine)
  {
  case 1:
  case 2:
  case 3:
  case 4:
    u8g.drawStr(0, 22, "Sortie 1:");
    print_on_screen(57, 22, arrayOfEV[0].remainingTimeOn);
    u8g.drawStr(85, 22, ",");
    print_on_screen(90, 22, arrayOfEV[0].nextDayOn);
    Serial.print(F(" Sortie 1"));
    Serial.print(arrayOfEV[0].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[0].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 33, "sortie 2:");
    print_on_screen(57, 33, arrayOfEV[1].remainingTimeOn);
    u8g.drawStr(85, 33, ",");
    print_on_screen(90, 22, arrayOfEV[1].nextDayOn);
    Serial.print(F(" Sortie 2"));
    Serial.print(arrayOfEV[1].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[1].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 44, "sortie 3:");
    print_on_screen(57, 44, arrayOfEV[2].remainingTimeOn);
    u8g.drawStr(85, 44, ",");
    print_on_screen(90, 44, arrayOfEV[2].nextDayOn);
    Serial.print(F(" Sortie 3"));
    Serial.print(arrayOfEV[2].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[2].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 55, "sortie 4:");
    print_on_screen(57, 55, arrayOfEV[3].remainingTimeOn);
    u8g.drawStr(85, 55, ",");
    print_on_screen(90, 55, arrayOfEV[3].nextDayOn);
    Serial.print(F(" Sortie 4"));
    Serial.print(arrayOfEV[3].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[3].nextDayOn);
    Serial.println(F(""));
    break;
  case 5:
  case 6:
  case 7:
  case 8:
    u8g.drawStr(0, 22, "sortie 5:");
    print_on_screen(57, 22, arrayOfEV[4].remainingTimeOn);
    u8g.drawStr(85, 22, ",");
    print_on_screen(90, 22, arrayOfEV[4].nextDayOn);
    Serial.print(F(" Sortie 5"));
    Serial.print(arrayOfEV[4].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[4].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 33, "sortie 6:");
    print_on_screen(57, 33, arrayOfEV[5].remainingTimeOn);
    u8g.drawStr(85, 33, ",");
    print_on_screen(90, 33, arrayOfEV[5].nextDayOn);
    Serial.print(F(" Sortie 6"));
    Serial.print(arrayOfEV[5].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[5].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 44, "sortie 7:");
    print_on_screen(57, 44, arrayOfEV[6].remainingTimeOn);
    u8g.drawStr(85, 44, ",");
    print_on_screen(90, 44, arrayOfEV[6].nextDayOn);
    Serial.print(F(" Sortie 7"));
    Serial.print(arrayOfEV[6].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[6].nextDayOn);
    Serial.println(F(""));
    u8g.drawStr(0, 55, "sortie 8:");
    print_on_screen(57, 55, arrayOfEV[7].remainingTimeOn);
    u8g.drawStr(85, 55, ",");
    print_on_screen(90, 55, arrayOfEV[7].nextDayOn);
    Serial.print(F(" Sortie 8"));
    Serial.print(arrayOfEV[7].remainingTimeOn);
    Serial.print(F(", "));
    Serial.print(arrayOfEV[7].nextDayOn);
    Serial.println(F(""));
    break;
  }

  if (menu.actualLine < 5)
  {
    draw_cursor((menu.actualLine + 1) * 11);
  }
  else
  {
    draw_cursor((menu.actualLine - 4) * 11);
  }
}

void auto_mode_screen()
{
  u8g.drawStr(10, 11, "Sortie ");
  print_on_screen(90, 11, menu.selectedEV);
  Serial.print(F(" Sortie "));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  // print auto mode state
  u8g.drawStr(5, 22, "Etat:");
  int mem_value = eeprom.Read(mem_address[2][menu.selectedEV - 1]);
  activate_auto_mode_screen(mem_value, 90, 22);
  Serial.print(F(" Etat : "));
  Serial.print(mem_value);
  Serial.println(F(""));
  // print auto time on
  u8g.drawStr(5, 33, "Temps on :");
  mem_value = eeprom.Read(mem_address[0][menu.selectedEV - 1]);
  print_on_screen(70, 33, mem_value);
  Serial.print(F(" Temps on : "));
  Serial.print(mem_value);
  Serial.println(F(""));
  // print auto frequency
  u8g.drawStr(5, 44, "Tous les:");
  print_mem_value(70, 44, mem_address[1][menu.selectedEV - 1]);
  u8g.drawStr(110, 44, "j");
  Serial.print(F(" Tout les : "));
  Serial.print(eeprom.Read(mem_address[1][menu.selectedEV - 1]));
  Serial.print(F(" j"));
  Serial.println(F(""));
  // print auto start hour
  u8g.drawStr(5, 56, "Heure :");
  print_mem_value(70, 55, mem_address[3][menu.selectedEV - 1]);
  Serial.print(F(" Heure : "));
  Serial.print(eeprom.Read(mem_address[3][menu.selectedEV - 1]));
  Serial.println(F(""));
}

void buttons_parameter_screen()
{
  u8g.drawStr(10, 11, "Les boutons");
  Serial.println(F(" Les boutons "));
  u8g.drawStr(5, 22, "bouton 1:");
  print_mem_value(75, 22, 170);
  Serial.print(F(" bouton 1 :"));
  Serial.print(eeprom.Read(170));
  Serial.println(F(""));
  u8g.drawStr(5, 33, "bouton 2:");
  print_mem_value(75, 33, 171);
  Serial.print(F(" bouton 2 :"));
  Serial.print(eeprom.Read(171));
  Serial.println(F(""));
  u8g.drawStr(5, 44, "bouton 3:");
  print_mem_value(75, 44, 172);
  Serial.print(F(" bouton 3 :"));
  Serial.print(eeprom.Read(172));
  Serial.println(F(""));
  u8g.drawStr(5, 55, "bouton 4:");
  print_mem_value(75, 56, 173);
  Serial.print(F(" bouton 4 :"));
  Serial.print(eeprom.Read(173));
  Serial.println(F(""));
}

void clock_parameter_screen()
{
  u8g.drawStr(10, 11, " Heure ");
  Serial.println(F(" Heure "));
  u8g.drawStr(2, 22, " Date :");
  print_on_screen(7, 22, menu.rtc[4]);
  u8g.drawStr(20, 22, "/");
  print_on_screen(28, 22, menu.rtc[5]);
  u8g.drawStr(40, 22, "/");
  print_on_screen(48, 22, menu.rtc[6]);
  Serial.print(F(" Date : "));
  Serial.print(menu.rtc[4]);
  Serial.print(F("/"));
  Serial.print(menu.rtc[5]);
  Serial.print(F("/"));
  Serial.print(menu.rtc[6]);
  Serial.println(F(""));
  u8g.drawStr(2, 44, " Heure :");
  print_on_screen(7, 44, menu.rtc[2]);
  u8g.drawStr(20, 44, ":");
  print_on_screen(25, 44, menu.rtc[1]);
  u8g.drawStr(40, 44, ":");
  print_on_screen(45, 44, menu.rtc[0]);
  Serial.print(F(" Heure : "));
  Serial.print(menu.rtc[2]);
  Serial.print(F(":"));
  Serial.print(menu.rtc[1]);
  Serial.print(F(":"));
  Serial.print(menu.rtc[0]);
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
    u8g.drawStr(5, 22, "Auto ete/hiver :");
    activate_auto_mode_screen(160, 90, 22);
    Serial.print(F("Auto ete/hiver : "));
    Serial.print(eeprom.Read(160));
    Serial.println(F(""));
    u8g.drawStr(5, 33, "Temp ete/hiver :");
    print_mem_value(70, 33, 161);
    Serial.print(F("Temp ete/hiver : "));
    Serial.print(eeprom.Read(161));
    Serial.println(F(""));
    u8g.drawStr(5, 44, "Duree ete :");
    print_mem_value(70, 44, 162);
    Serial.print(F("Duree ete : "));
    Serial.print(eeprom.Read(162));
    Serial.println(F(""));
    u8g.drawStr(5, 55, "Freq ete :");
    print_mem_value(70, 55, 163);
    Serial.print(F("Freq ete : "));
    Serial.print(eeprom.Read(163));
    Serial.println(F(""));
    break;
  case 5:
  case 6:
    u8g.drawStr(5, 22, "Duree hiver :");
    print_mem_value(70, 22, 164);
    Serial.print(F("Duree hiver : "));
    Serial.print(eeprom.Read(164));
    Serial.println(F(""));
    u8g.drawStr(5, 33, "Freq hivers :");
    print_mem_value(70, 33, 165);
    Serial.print(F("Freq hivers : "));
    Serial.print(eeprom.Read(165));
    Serial.println(F(""));
    break;
  }
}

void manual_mode_screen()
{
  u8g.drawStr(20, 11, "Mode manuel");
  Serial.println(F(" Mode manuel "));
  menu.ecran_princ_min = 60;
  manual_mode_state = 1;
  if (arrayofButton[2].type != -1)
  {
    if (arrayOfEV[menu.selectedEV - 1].remainingTimeOn == 0)
    {
      arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 10;
    }
    else
    {
      arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
    }
  }
  print_active_ev();
}

void print_active_ev()
{
  for (int i = 0; i < 8; i++)
  {
    if (arrayOfEV[i].remainingTimeOn != 0)
    {
      print_on_screen(2 + (16 * (i + 1)), 55, i + 1);
      Serial.print(i + 1);
      Serial.print(F("  "));
    }
  }
  Serial.println(F(""));
}

void active_mode_screen()
{
  if (arrayofButton[0].type == 2)
  {
    u8g.drawStr(2, 11, "activation des sorties :");
    u8g.drawStr(6, 22, "mode automatique :");
    Serial.println(F("activation des sorties :"));
    Serial.println(F("mode automatique :"));
    for (int i = 0; i < 8; i++)
    {
      if (eeprom.Read(mem_address[2][i] == 1))
      {
        print_on_screen(2 + (16 * i), 33, i + 1);
        Serial.print(i + 1);
        Serial.print(F("  "));
      }
    }
    Serial.println(F(""));
    u8g.drawStr(6, 44, "sorties actives :");
    Serial.println(F("mode automatique :"));
    for (int i = 0; i < 8; i++)
    {
      if (eeprom.Read(mem_address[4][i] == 1))
      {
        print_on_screen(2 + (16 * i), 56, i + 1);
        Serial.print(i + 1);
        Serial.print(F("  "));
      }
    }
    Serial.println(F(""));
  }
}

void delay_screen()
{
  sprintf(buf, "%d", menu.selectedEV);
  u8g.drawStr(70, 22, buf);
  u8g.drawStr(10, 11, " Delais ");
  u8g.drawStr(5, 22, " sortie N) :");
  Serial.println(F("Delais :"));
  Serial.print(F("sortie N° :"));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
  }
}

void stop_screen()
{
  sprintf(buf, "%d", menu.selectedEV);
  u8g.drawStr(70, 22, buf);
  u8g.drawStr(10, 11, " Delais ");
  u8g.drawStr(5, 22, " sortie N) :");
  Serial.println(F("Delais :"));
  Serial.print(F("sortie N° :"));
  Serial.print(menu.selectedEV);
  Serial.println(F(""));
  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
  }
}

/*int line 11 22 33 44 55*/
void draw_cursor(int line)
{
  int x = 0;
  int y = line;
  int length = 4;
  int height = 11;
  u8g.drawBox(x, y, length, height);
}

void minus_button(int mem_adress, int max)
{
  int time_on = 0;
  if (arrayofButton[2].type != -1)
  {
    time_on = 7 * arrayofButton[2].type + 1;
    arrayofButton[2].type = -1;
  }

  switch (arrayofButton[1].type)
  {
  case 5: // 170
    time_on = eeprom.Read(mem_address[7][0]);
    break;
  case 4:
    time_on = eeprom.Read(mem_address[7][1]);
    break;
  case 3:
    time_on = eeprom.Read(mem_address[7][2]);
    break;
  case 2:
    time_on = eeprom.Read(mem_address[7][3]);
    break;
  }
  if (arrayofButton[1].type != -1)
  {
    arrayofButton[1].type = -1;
  }
  if (time_on >= 1)
  {
    int mem_value;
    mem_value = eeprom.Read(mem_adress);
    mem_value = mem_value + time_on;
    if (mem_value >= max)
    {
      mem_value = mem_value - max;
    }
    eeprom.write(mem_adress, mem_value);
  }
}

void update_all_ev_state()
{
  for (int i = 0; i < 8; i++)
  {
    set_output_state(addr_mod_relay, i + 1, arrayOfEV[i].remainingTimeOn);
  }
}

void set_output_state(int adr, int ev, int val)
{
  if (eeprom.Read(mem_address[4][ev - 1]) == 0)
  {
    val = 0;
  }

  if (val == 0)
  {
    send_to_module(adr, ev);
    send_to_module(adr, 100);
  }
  else
  {
    send_to_module(adr, ev);
    send_to_module(adr, 101);
  }
}

void update_auto_mode_with_ath21()
{
  int mem_value;
  sensors_event_t humidity;
  sensors_event_t temp;
  aht_humidity->getEvent(&humidity);
  aht_temp->getEvent(&temp);

  int temp_value;
  temp_value = (int)temp.temperature;
  eeprom.write(150, temp_value);
  int humidity_value;
  humidity_value = (int)humidity.relative_humidity;
  eeprom.write(151, humidity_value);

  int temp_change_season;
  temp_change_season = eeprom.Read(161);

  for (int i = 0; i < 8; i++)
  {
    if (temp_value <= temp_change_season)
    {
      mem_value = eeprom.Read(mem_address[6][2]);
      eeprom.write(mem_address[0][i], mem_value);
      mem_value = eeprom.Read(mem_address[6][3]);
      eeprom.write(mem_address[1][i], mem_value);
    }
    else
    {
      mem_value = eeprom.Read(164);
      eeprom.write(mem_address[0][i], mem_value);
      mem_value = eeprom.Read(165);
      eeprom.write(mem_address[1][i], mem_value);
    }
  }
}

void loop_actualization()
{
  if (menu.rtc_min != menu.rtc[1])
  {
    // check remainining time for ev on
    for (int i = 0; i < 8; i++)
    {
      if (arrayOfEV[i].remainingTimeOn != 0)
      {
        arrayOfEV[i].remainingTimeOn--;

        if (arrayOfEV[i].remainingTimeOn <= 0)
        {
          arrayOfEV[i].remainingTimeOn = 0;
        }
      }
    }
    menu.rtc_min = menu.rtc[1];
  }

  if (manual_mode_state == 0)
  {
    horlact();
  }

  // Check Temperature and humidity at 12 o'clock if auto mode on with aht21
  int mem_value;
  mem_value = eeprom.Read(160);
  if (mem_value == 1)
  {
    if (menu.rtc_day != menu.rtc[4])
    {
      if (menu.rtc[2] == 12)
      {
        update_auto_mode_with_ath21();
      }
    }
  }
}

void horlact()
{
  for (int i = 0; i < 8; i++)
  {
    int time_on;

    // if mode auto on
    if (eeprom.Read(mem_address[2][i]) == 1)
    {
      // if ev state is on
      if (eeprom.Read(mem_address[4][i]) == 1)
      {
        // if clock h == auto start time
        if (menu.rtc[2] == eeprom.Read(mem_address[3][i]))
        {
          if (arrayOfEV[i].remainingTimeOn == 0)
          {
            // if not started once set next day as today
            if (arrayOfEV[i].nextDayOn == 0)
            {
              arrayOfEV[i].nextDayOn = menu.rtc[4];
            }
            // if clock day == next start day
            if (arrayOfEV[i].nextDayOn == menu.rtc[4])
            {
              // mise en route
              time_on = eeprom.Read(mem_address[0][i]);
              if (time_on <= 0)
              {
                time_on = 0;
              }
              if (time_on >= max_time_on_ev)
              {
                time_on = max_time_on_ev;
              }
              arrayOfEV[i].remainingTimeOn = time_on;
              calculate_next_day(i);
            }
          }
        }
      }
      else
      {
        arrayOfEV[i].remainingTimeOn = 0;
        if (arrayOfEV[i].nextDayOn == menu.rtc[4])
        {
          arrayOfEV[i].nextDayOn = 0;
        }
      }
    }
  }
}

void calculate_next_day(int i)
{
  int day_to_add;
  int day_in_month;
  int day;
  if (arrayofButton[1].type != 0)
  {
    day_to_add = eeprom.Read(mem_address[1][i]);
  }
  else
  {
    day_to_add = 1;
  }
  day = menu.rtc[4] + day_to_add;
  switch (menu.rtc[5])
  {
  case 1: // 31
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    day_in_month = 31;
    break;
  case 2: // 28
    day_in_month = 28;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    day_in_month = 30;
    break;
  }
  if (day > day_in_month)
  {
    day = day - day_in_month;
  }
  arrayOfEV[i].nextDayOn = day;
}

void manual_activation_screen()
{
  int mem_value;
  if (arrayofButton[2].type != -1)
  {
    menu.selectedEV = arrayofButton[2].type + 1;
    arrayofButton[2].type = -1;
  }
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
  }
  switch (arrayofButton[1].type)
  {
  case 5:
    arrayofButton[1].type = -1;
    mem_value = eeprom.Read(170);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 4:
    arrayofButton[1].type = -1;
    mem_value = eeprom.Read(171);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 3:
    arrayofButton[1].type = -1;
    mem_value = eeprom.Read(172);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 2:
    arrayofButton[1].type = -1;
    mem_value = eeprom.Read(173);
    manual_time_on = manual_time_on + mem_value;
    break;
  }
  if (manual_time_on >= max_time_on_ev)
  {
    manual_time_on = manual_time_on - max_time_on_ev;
  }
  sprintf(buf, "%d", menu.selectedEV);
  u8g.drawStr(70, 22, buf);
  sprintf(buf, "%d", manual_time_on);
  u8g.drawStr(60, 33, buf);
  u8g.drawStr(10, 11, " manuel ");
  u8g.drawStr(5, 22, " sortie n :");
  u8g.drawStr(5, 33, " temp :");

  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = manual_time_on;

    manual_time_on = 0;
    menu.selectedEV = 1;
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

void activate_auto_mode_screen(int v, int col, int li)
{
  switch (v)
  {
  case 0:
    u8g.drawStr(col, li, "desactiver");
    break;
  case 1:
    u8g.drawStr(col, li, "activer");
    break;
  }
}