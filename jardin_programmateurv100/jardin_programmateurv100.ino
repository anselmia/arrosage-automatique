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
    165 frequency winter*/
MYEEPROM eeprom = MYEEPROM();
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
  {
    arrayOfEV[i].remainingTimeOn = 0;
    arrayOfEV[i].nextDayOn = 0;
  }
}

// void init_memory()
//{
//   eeprom.write(20, 0);
//   eeprom.write(30, 0);
//   eeprom.write(40, 0);
//   eeprom.write(50, 0);
//   eeprom.write(60, 0);
//   eeprom.write(70, 0);
//   eeprom.write(80, 0);
//   eeprom.write(90, 0);
//   eeprom.write(21, 0);
//   eeprom.write(31, 0);
//   eeprom.write(41, 0);
//   eeprom.write(51, 0);
//   eeprom.write(61, 0);
//   eeprom.write(71, 0);
//   eeprom.write(81, 0);
//   eeprom.write(91, 0);
//   eeprom.write(22, 0);
//   eeprom.write(32, 0);
//   eeprom.write(42, 0);
//   eeprom.write(52, 0);
//   eeprom.write(62, 0);
//   eeprom.write(72, 0);
//   eeprom.write(82, 0);
//   eeprom.write(92, 0);
//   eeprom.write(23, 0);
//   eeprom.write(33, 0);
//   eeprom.write(43, 0);
//   eeprom.write(53, 0);
//   eeprom.write(63, 0);
//   eeprom.write(73, 0);
//   eeprom.write(83, 0);
//   eeprom.write(93, 0);
//   eeprom.write(27, 0);
//   eeprom.write(37, 0);
//   eeprom.write(47, 0);
//   eeprom.write(57, 0);
//   eeprom.write(67, 0);
//   eeprom.write(77, 0);
//   eeprom.write(87, 0);
//   eeprom.write(97, 0);
//   eeprom.write(150, 0);
//   eeprom.write(151, 0);
//   eeprom.write(160, 0);
//   eeprom.write(161, 0);
//   eeprom.write(162, 0);
//   eeprom.write(163, 0);
//   eeprom.write(164, 0);
//   eeprom.write(165, 0);
// }

void select_button(int selected_button)
{
  switch (arrayofButton[0].getSelection())
  {
  case 1: // right
    menu.prinheu();
    arrayofButton[0].type = 0;
    Serial.println(F("Forward")); // to remove after test
    menu.forward();
    break;
  case 5: // up
    menu.prinheu();
    arrayofButton[0].type = 1;
    Serial.println(F("upward")); // to remove after test
    menu.up();
    break;
  case 7: // left
    menu.prinheu();
    arrayofButton[0].type = 2;
    Serial.println(F("backard")); // to remove after test
    menu.backward();

    break;
  case 8: // down
    menu.prinheu();
    arrayofButton[0].type = 3;
    Serial.println(F("Downward"));
    menu.down();
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: // +
    menu.prinheu();
    arrayofButton[1].type = 0;
    menu.updateValue(1);

    break;
  case 2: // -
    menu.prinheu();
    arrayofButton[1].type = 1;
    menu.updateValue(0);
    break;
  }

  switch (arrayofButton[0].getSelection())
  {
  case 1: // s7
    arrayofButton[2].type = 6;
    menu.selectEV(7);
    break;
  case 2: // s6
    arrayofButton[2].type = 5;
    menu.selectEV(6);
  case 3: // s5
    arrayofButton[2].type = 4;
    menu.selectEV(5);
    break;
  case 4: // s4
    arrayofButton[2].type = 3;
    menu.selectEV(4);
    break;
  case 5: // s3
    arrayofButton[2].type = 2;
    menu.selectEV(3);
    break;
  case 6: // s2
    arrayofButton[2].type = 1;
    menu.selectEV(2);
    break;
  case 7: // s1
    arrayofButton[2].type = 0;
    menu.selectEV(1);
    break;
  case 8:
    break;
  }
}

// to remove
void select()
{
  int selectedButton = 0;
  selectedButton = Serial.parseInt();
  switch (selectedButton)
  {
  case 6: // right
    menu.prinheu();
    arrayofButton[0].type = 0;
    Serial.println(F("Forward")); // to remove after test
    menu.forward();
    break;
  case 8: // up
    menu.prinheu();
    arrayofButton[0].type = 1;
    Serial.println(F("up")); // to remove after test
    menu.up();
    break;
  case 4: // left
    menu.prinheu();
    arrayofButton[0].type = 2;
    Serial.println(F("backard")); // to remove after test
    menu.backward();
    break;
  case 2: // down
    menu.prinheu();
    arrayofButton[0].type = 3;
    Serial.println(F("Downward"));
    menu.down();
    break;
  }

  switch (selectedButton)
  {
  case 9: // +
    menu.prinheu();
    arrayofButton[1].type = 0;
    menu.updateValue(1);
    break;
  case 7: // -
    menu.prinheu();
    arrayofButton[1].type = 1;
    menu.updateValue(1);
    break;
  }

  switch (selectedButton)
  {
  case 11: // s7
    arrayofButton[2].type = 6;
    menu.selectEV(7);
    break;
  case 12: // s6
    arrayofButton[2].type = 5;
    menu.selectEV(6);
  case 13: // s5
    arrayofButton[2].type = 4;
    menu.selectEV(5);
    break;
  case 14: // s4
    arrayofButton[2].type = 3;
    menu.selectEV(4);
    break;
  case 15: // s3
    arrayofButton[2].type = 2;
    menu.selectEV(3);
    break;
  case 16: // s2
    arrayofButton[2].type = 1;
    menu.selectEV(2);
    break;
  case 17: // s1
    arrayofButton[2].type = 0;
    menu.selectEV(1);
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
    u8g.setFont(u8g_font_tpss); // Use standard character
    menu.getClock();
    print_screen();
  } while (u8g.nextPage()); // Select the next page

  delay(100);
  loop_actualization();
  update_all_ev_state();

  // if ev not in active state reset remining time to 0
  // Serial.println(F("end of loop"));
  // for (int i = 0; i < 8; i++)
  //{
  //  if (eeprom.Read(17 + (10 * (i + 1))) == 0)
  //  {
  //    if (arrayOfEV[i].remainingTimeOn != 0)
  //    {
  //      arrayOfEV[i].remainingTimeOn = 0;
  //    }
  //  }
  //}
  delay(2000); // to remove
  reset_button();
}

void reset_button()
{
  if (button_state == 0)
  {
    if (menu.rtc_min != menu.rtc[1])
      menu.inactive++;
    if (menu.inactive == 2)
    {
      main_screen();
    }
  }
  else
    menu.inactive = 0;

  arrayofButton[0].type = -1;
  arrayofButton[1].type = -1;
  arrayofButton[2].type = -1;
}

void print_screen()
{
  if (menu.action == 0)
  {
    draw_cursor();
  }
  else if (menu.action == 1)
  {
    switch (menu.actualScreen)
    {
    case 0:
      main_screen();
      break;
    case 1:
      menu_screen();
      break;
    case 2:
      parameter_screen();
      break;
    case 4:
      auto_mode_screen();
      break;
    case 6:
      clock_parameter_screen();
      break;
    case 7:
      other_parameter_screen();
      break;
    case 8:
      manual_mode_screen();
      break;
    case 9:
      state_screen();
      break;
    case 10:
      delay_screen();
      break;
    case 11:
      stop_screen();
      break;
    default:
      main_screen();
      break;
    }
    draw_cursor();
  }
  menu.action = -1;
}

void inii2c(int adr)
{
  Wire.beginTransmission(adr);
  error = Wire.endTransmission(aar);
}

void print_actual_time()
{
  Serial.print(menu.rtc[4]);
  Serial.print(F("/"));
  Serial.print(menu.rtc[5]);
  Serial.print(F("/"));
  Serial.print(menu.rtc[6]);
  Serial.println(F(""));
  Serial.print(menu.rtc[2]);
  Serial.print(F(":"));
  Serial.print(menu.rtc[1]);
  Serial.print(F(":"));
  Serial.print(menu.rtc[0]);
  Serial.println(F(""));
  print_on_screen(7, 11, menu.rtc[4]);
  u8g.drawStr(20, 11, "/");
  print_on_screen(28, 11, menu.rtc[5]);
  u8g.drawStr(40, 11, "/");
  print_on_screen(48, 11, menu.rtc[6]);
  print_on_screen(7, 22, menu.rtc[2]);
  u8g.drawStr(20, 22, ":");
  print_on_screen(25, 22, menu.rtc[1]);
  u8g.drawStr(40, 33, ":");
  print_on_screen(45, 22, menu.rtc[0]);
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
  // init_memory();

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
      Serial.println(F("erreur horloge"));
    }
    u8g.drawStr(5, 44, "version");
    Serial.print(F("Version : "));
    print_on_screen(50, 44, versio);
    Serial.print(versio);
    Serial.println("");
  } while (u8g.nextPage());
}

void main_screen()
{
  if (error == 1)
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
  int mem_value = eeprom.Read(12 + (menu.selectedEV * 10));
  activate_screen(mem_value, 70, 22);
  Serial.println(F(""));
  // print auto time on
  u8g.drawStr(5, 33, "Duree :");
  mem_value = eeprom.Read(10 + (menu.selectedEV * 10));
  print_on_screen(70, 33, mem_value);
  Serial.print(F(" Duree : "));
  Serial.print(mem_value);
  Serial.println(F(""));
  // print auto frequency
  u8g.drawStr(5, 44, "Tous les:");
  print_mem_value(70, 44, 11 + (menu.selectedEV * 10));
  u8g.drawStr(110, 44, "j");
  Serial.print(F(" Tout les : "));
  Serial.print(eeprom.Read(11 + (menu.selectedEV * 10)));
  Serial.print(F(" j"));
  Serial.println(F(""));
  // print auto start hour
  u8g.drawStr(5, 56, "Heure :");
  print_mem_value(70, 55, 13 + (menu.selectedEV * 10));
  Serial.print(F(" Heure : "));
  Serial.print(eeprom.Read(13 + (menu.selectedEV * 10)));
  Serial.println(F(""));
}

void clock_parameter_screen()
{
  menu.getClock();
  u8g.drawStr(10, 11, " Horloge ");
  Serial.println(F(" Horloge "));
  u8g.drawStr(5, 22, " Date :");
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
  u8g.drawStr(5, 44, " Heure :");
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
    Serial.print(F("Auto ete/hiver : "));
    activate_screen(160, 90, 22);
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

  menu.ecran_princ_min = 60;
  if (menu.selectedEV != 0 && menu.screenValue != 0)
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = menu.screenValue;
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
  Serial.print(F(" 6 : "));
  Serial.print(arrayOfEV[5].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[5].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(85, 33, "7 : ");
  print_on_screen(90, 33, arrayOfEV[6].remainingTimeOn);
  u8g.drawStr(100, 33, ",");
  print_on_screen(115, 33, arrayOfEV[6].nextDayOn);
  Serial.print(F("7 : "));
  Serial.print(arrayOfEV[6].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[6].nextDayOn);
  Serial.println(F(""));
  u8g.drawStr(85, 44, "8 : ");
  print_on_screen(90, 44, arrayOfEV[7].remainingTimeOn);
  u8g.drawStr(100, 44, ",");
  print_on_screen(115, 44, arrayOfEV[7].nextDayOn);
  Serial.print(F("8 : "));
  Serial.print(arrayOfEV[7].remainingTimeOn);
  Serial.print(F(", "));
  Serial.print(arrayOfEV[7].nextDayOn);
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
  activate_screen(17 + (10 * (menu.selectedEV + 1)), 70, 33);
  if (eeprom.Read(17 + (10 * (menu.selectedEV + 1))) == 0)
  {
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
    arrayOfEV[menu.selectedEV - 1].nextDayOn = 0;
  }
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

  if (menu.stop == 1)
  {
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = 0;
    menu.stop = 0;
  }
}

/*int line 11 22 33 44 55*/
void draw_cursor()
{
  int x = 0;
  int y = menu.actualLine * 11;
  int length = 4;
  int height = 11;
  u8g.drawBox(x, y, length, height);
  Serial.print(F("C"));
  Serial.print(menu.actualLine);
  Serial.println(F(""));
}

void update_all_ev_state()
{
  for (int i = 0; i < 8; i++)
  {
    set_output_state(i + 1);
  }
}

void set_output_state(int ev)
{
  if (eeprom.Read(17 + (10 * ev)) == 1)
  {
    if (arrayOfEV[ev - 1].remainingTimeOn == 0)
    {
      send_to_module(addr_mod_relay, ev);
      send_to_module(addr_mod_relay, 100);
    }
    else
    {
      send_to_module(addr_mod_relay, ev);
      send_to_module(addr_mod_relay, 101);
    }
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
      mem_value = eeprom.Read(162);
      eeprom.write(10 + (10 * (i + 1)), mem_value);
      mem_value = eeprom.Read(163);
      eeprom.write(11 + (10 * (i + 1)), mem_value);
    }
    else
    {
      mem_value = eeprom.Read(164);
      eeprom.write(10 + (10 * (i + 1)), mem_value);
      mem_value = eeprom.Read(165);
      eeprom.write(11 + (10 * (i + 1)), mem_value);
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

        if (arrayOfEV[i].remainingTimeOn < 0)
        {
          // Serial.println(F("set to 0 remaning time"));
          arrayOfEV[i].remainingTimeOn = 0;
        }
      }
    }
    menu.rtc_min = menu.rtc[1];
  }

  horlact();

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
  if (menu.delay == 1)
  {
    arrayOfEV[menu.selectedEV - 1].nextDayOn += 1;
    menu.delay = 0;
  }
  for (int i = 0; i < 8; i++)
  {
    int time_on = 0;
    // Serial.println(eeprom.Read(12 + (10 * (i + 1))));
    //  if mode auto on
    if (eeprom.Read(12 + (10 * (i + 1))) == 1)
    {
      // if ev state is on
      // Serial.println(eeprom.Read(17 + (10 * (i + 1))));
      if (eeprom.Read(17 + (10 * (i + 1))) == 1)
      {
        // Serial.println(eeprom.Read(13 + (10 * (i + 1))));
        //  if clock h == auto start time
        if (menu.rtc[2] == eeprom.Read(13 + (10 * (i + 1))))
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
              time_on = eeprom.Read(10 + (10 * (i + 1)));
              if (time_on <= 0)
              {
                time_on = 0;
              }
              if (time_on >= max_time_on_ev)
              {
                time_on = max_time_on_ev;
              }
              arrayOfEV[i].remainingTimeOn = time_on;
              // Serial.println(F("Next day calc i should not"));
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

  day_to_add = eeprom.Read(11 + (10 * (i + 1)));
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