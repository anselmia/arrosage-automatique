/* Code to relay module
  1,2,3,4,5,6,7,8 : select the output pin for desired ev
  100: Set the selected output to LOW
  subScreen_selected: Set the selected output to HIGH
  102: Set all output to low
*/

#include <Wire.h> //i2c
#include "Arduino.h"
#include <EEPROM.h>
#include <U8glib.h> // LCD screen
#include <avr/wdt.h>
#include <DS1307.h>         // DS1307 clock module
#include <Adafruit_AHTX0.h> // AHT21
#include "button.h"
#include "ev.h"
// #include "screen.h"

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
boolean aar = true; // mode wire endtransmission
int module_state[3];
int manual_mode_state = 0;
int manual_selected_ev = 0;
int manual_time_on = 0;
char buf[10];
int ecran_princ_min = 0;
int ecran_princ_sec = 0;
int selected_line_on_screen = 0;
int subScreen_selected = 0;
int rtc[7];
int selected_parameter = 0;
int selected_ev_on_screen = 0;
int rtc_min = 0;
int rtc_day = 0;
const int addr_mod_relay = 9;
const int max_time_on_ev = 20;
U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;

int selected_screen = 0;
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
  RTC.get(rtc, true);
  RTC.SetOutput(DS1307_SQW32KHZ);

  Serial.begin(9600);
  Init();
}

void prinheu()
{
  ecran_princ_min = rtc[1] + 2;
  if (ecran_princ_min >= 60)
  {
    ecran_princ_min = 0;
  }
  ecran_princ_sec = rtc[0];
  subScreen_selected = 1;
}

void prinheu2()
{
  if (subScreen_selected == 0)
  {
    prinheu();
  }
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
  switch (selected_button)
  {
  case 1: // right
    prinheu();
    arrayofButton[0].type = 0;
    break;
  case 5: // up
    prinheu();
    arrayofButton[0].type = 1;
    if (selected_screen == 0)
    {
      selected_screen = 7;
    }
    break;
  case 7: // left
    prinheu();
    arrayofButton[0].type = 2;
    break;
  case 8: // down
    prinheu();
    arrayofButton[0].type = 3;
    break;
  }

  switch (selected_button)
  {
  case 1: //+
    prinheu();
    arrayofButton[1].type = 0;
    switch (selected_screen)
    {
    case 3:
    case 4:
    case 9:
      break;
    case 10:
      init_ev_state();
      break;
    case 8:
      init_ev_state();
      selected_screen = 0;
      break;
    default:
      selected_screen = 2;
      break;
    }
    break;
  case 2: //-
    prinheu();
    arrayofButton[1].type = 1;
    if (selected_screen == 0)
    {
      subScreen_selected = 0;
      selected_line_on_screen = 0;
      selected_parameter = 0;
    }
    break;
  case 3: // b4
    prinheu();
    arrayofButton[1].type = 2;
    if (selected_screen == 0)
    {
      selected_screen = 8;
    }
    break;
  case 4: // b3
    prinheu();
    arrayofButton[1].type = 3;
    if (selected_screen == 0)
    {
      selected_screen = 10;
    }
    break;
  case 5: // b2
    prinheu();
    arrayofButton[1].type = 4;
    if (selected_screen == 0)
    {
      selected_screen = 9;
    }
    break;
  case 6: // b1
    prinheu();
    arrayofButton[1].type = 5;
    if (selected_screen == 0)
    {
      selected_screen = 3;
    }
    break;
  }

  switch (selected_button)
  {
  case 1: // s7
    arrayofButton[2].type = 6;
    break;
  case 2: // s6
    arrayofButton[2].type = 5;
  case 3: // s5
    arrayofButton[2].type = 4;
    break;
  case 4: // s4
    arrayofButton[2].type = 3;
    break;
  case 5: // s3
    arrayofButton[2].type = 2;
    break;
  case 6: // s2
    arrayofButton[2].type = 1;
    break;
  case 7:
    arrayofButton[2].type = 0;
    break;
  case 8:
    break;

    if (arrayofButton[2].type != -1)
    {
      prinheu2();
      if (selected_screen <= 1)
      {
        selected_screen = 4;
      }
    }
  }
}

void screen_selection()
{
  RTC.get(rtc, true);
  if (ecran_princ_min <= rtc[1])
  {
    if (ecran_princ_sec <= rtc[0])
    {
      selected_line_on_screen = 0;
      subScreen_selected = 0;
      selected_screen = 0;
      selected_ev_on_screen = 0;
      selected_parameter = 0;
      manual_mode_state = 0;
    }
  }

  if (subScreen_selected == 0)
  {
    manual_mode_state = 0;
    main_screen();
  }
  else
  {
    select_screen();
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
    screen_selection();
  } while (u8g.nextPage()); // Select the next page

  delay(100);
  loop_actualization();
  update_all_ev_state();

  // if ev not in active state reset remining time to 0
  for (int i = 0; i < 8; i++)
  {
    if (EEPROM.read(mem_address[4][i]) == 0)
    {
      if (arrayOfEV[i].remainingTimeOn != 0)
      {
        arrayOfEV[i].remainingTimeOn = 0;
      }
    }
  }
}

void inii2c(int adr)
{
  Wire.beginTransmission(adr);
  error = Wire.endTransmission(aar);
}

void print_actual_time()
{
  u8g.drawStr(10, 11, " Heure ");
  print_on_screen(7, 22, rtc[4]);
  print_on_screen(28, 22, rtc[5]);
  print_on_screen(48, 22, rtc[6]);
  u8g.drawStr(20, 22, "/");
  u8g.drawStr(40, 22, "/");
  u8g.drawStr(20, 33, ":");
  u8g.drawStr(40, 33, ":");
  print_on_screen(7, 33, rtc[2]);
  print_on_screen(25, 33, rtc[1]);
  print_on_screen(45, 33, rtc[0]);
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
    if (module_state[1] == 0)
    {
      RTC.get(rtc, true);
      print_actual_time();
    }
    else
    {
      u8g.drawStr(10, 22, "erreur horloge");
    }
    u8g.drawStr(5, 44, "version");
    print_on_screen(50, 44, versio);
  } while (u8g.nextPage());

  delay(2000);
}

void main_screen()
{
  if (error == 1)
  {
    u8g.drawStr(20, 11, "erreur");
  }
  else
  {
    print_actual_time();
  }
  print_active_ev();
}

void select_screen()
{
  switch (selected_screen)
  {
  case 1:
    output_screen();
    break;
  case 2:
    parameter_screen();
    break;
  case 3:
    manual_activation_screen();
    break;
  case 4:
    delay_screen();
    break;
  case 9:
    automatic_mode_status_screen();
    break;
  case 8:
    manual_mode_screen();
    break;
  default:
    selected_screen = 1;
    break;
  }
}

void automatic_mode_status_screen()
{
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_screen = 0;
    subScreen_selected = 0;
  }
  change_selected_line(1);
  int selected_mem_address = 0;
  int mem_value = 0;
  switch (selected_line_on_screen)
  {
  case 0:
    draw_rectangle(11);
    if (arrayofButton[2].type != -1)
    {
      selected_mem_address = mem_address[2][arrayofButton[2].type];

      arrayofButton[2].type = -1;
      if (EEPROM.read(selected_mem_address) == 1)
      {
        mem_value = 0;
      }
      else
      {
        mem_value = 1;
      }
      write_eeprom(selected_mem_address, mem_value);
    }
    break;
  case 1:
    draw_rectangle(33);
    if (arrayofButton[2].type != -1)
    {
      selected_mem_address = mem_address[4][arrayofButton[2].type];

      arrayofButton[2].type = -1;
      if (EEPROM.read(selected_mem_address) == 1)
      {
        mem_value = 0;
      }
      else
      {
        mem_value = 1;
      }
      write_eeprom(selected_mem_address, mem_value);
    }
    break;
  }
  u8g.drawStr(2, 11, "activation des sorties :");
  u8g.drawStr(6, 22, "mode automatique :");
  for (int i = 0; i < 8; i++)
  {
    if (EEPROM.read(mem_address[2][i] == 1))
    {
      print_on_screen(2 + (16 * i), 33, i + 1);
    }
  }
  u8g.drawStr(6, 44, "sorties actives :");
  for (int i = 0; i < 8; i++)
  {
    if (EEPROM.read(mem_address[4][i] == 1))
    {
      print_on_screen(2 + (16 * i), 56, i + 1);
    }
  }
}

void manual_mode_screen()
{
  if (arrayofButton[1].type == 0)
  {
    manual_mode_state = 0;
    arrayofButton[0].type = 2;
    subScreen_selected = 0;
  }
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_screen = 0;
    subScreen_selected = 0;
    manual_mode_state = 0;
  }
  u8g.drawStr(20, 11, " mode manuel");
  ecran_princ_min = 60;
  manual_mode_state = 1;
  if (arrayofButton[2].type != -1)
  {
    if (arrayOfEV[arrayofButton[2].type].remainingTimeOn == 0)
    {
      arrayOfEV[arrayofButton[2].type].remainingTimeOn = 10;
    }
    else
    {
      arrayOfEV[arrayofButton[2].type].remainingTimeOn = 0;
    }
    arrayofButton[2].type = -1;
  }
  print_active_ev();
}

void print_active_ev()
{
  for (int i = 0; i < 8; i++)
  {
    if (arrayOfEV[i].remainingTimeOn != 0)
    {
      print_on_screen(2 + (16 * (i + 1)), 56, i + 1);
    }
  }
}

void output_screen()
{
  if (arrayofButton[0].type == 2)
  {
    selected_screen = 0;
    arrayofButton[0].type = -1;
    subScreen_selected = 0;
  }
  change_selected_line(1);
  switch (selected_line_on_screen)
  {
  case 0:
    u8g.drawStr(0, 11, "sortie");
    u8g.drawStr(0, 22, "sortie");
    u8g.drawStr(0, 33, "sortie");
    u8g.drawStr(0, 44, "sortie");
    u8g.drawStr(0, 56, "sortie");
    u8g.drawStr(35, 11, "1 :");
    u8g.drawStr(35, 22, "2 :");
    u8g.drawStr(35, 33, "3 :");
    u8g.drawStr(35, 44, "4 :");
    u8g.drawStr(35, 56, "5 :");
    print_on_screen(57, 11, arrayOfEV[0].remainingTimeOn);
    print_on_screen(57, 22, arrayOfEV[1].remainingTimeOn);
    print_on_screen(57, 33, arrayOfEV[2].remainingTimeOn);
    print_on_screen(57, 44, arrayOfEV[3].remainingTimeOn);
    print_on_screen(57, 56, arrayOfEV[4].remainingTimeOn);
    u8g.drawStr(85, 33, ",");
    u8g.drawStr(85, 44, ",");
    u8g.drawStr(85, 56, ",");
    print_on_screen(90, 11, arrayOfEV[0].nextDayOn);
    print_on_screen(90, 22, arrayOfEV[1].nextDayOn);
    print_on_screen(90, 33, arrayOfEV[2].nextDayOn);
    print_on_screen(90, 44, arrayOfEV[3].nextDayOn);
    print_on_screen(90, 56, arrayOfEV[4].nextDayOn);
    break;
  case 1:
    u8g.drawStr(0, 11, "sortie");
    u8g.drawStr(0, 22, "sortie");
    u8g.drawStr(0, 33, "sortie");
    u8g.drawStr(35, 11, "6 :");
    u8g.drawStr(35, 22, "7 :");
    u8g.drawStr(35, 33, "8 :");
    print_on_screen(57, 11, arrayOfEV[5].remainingTimeOn);
    print_on_screen(57, 22, arrayOfEV[6].remainingTimeOn);
    print_on_screen(57, 33, arrayOfEV[7].remainingTimeOn);
    u8g.drawStr(85, 11, ",");
    u8g.drawStr(85, 22, ",");
    u8g.drawStr(85, 33, ",");
    print_on_screen(90, 11, arrayOfEV[5].nextDayOn);
    print_on_screen(90, 22, arrayOfEV[6].nextDayOn);
    print_on_screen(90, 33, arrayOfEV[7].nextDayOn);
    break;
  }
}

void parameter_screen()
{
  switch (selected_parameter)
  {
  case 1: // output screen
    select_output_parameter_screen();
    break;
  case 2: // buttons
    buttons_parameter_screen();
    break;
  case 3: // clock
    clock_parameter_screen();
    break;
  case 4: // other
    other_parameter_screen();
    break;

  default:
    // return to main screen
    if (arrayofButton[0].type == 2)
    {
      arrayofButton[0].type = -1;
      selected_screen = 0;
      subScreen_selected = 0;
      selected_parameter = 0;
      selected_line_on_screen = 0;
    }
    arrayofButton[1].type = -1;
    arrayofButton[1].type = -1;
    change_selected_line(4);
    switch (selected_line_on_screen)
    {
    case 0:
      if (arrayofButton[0].type == 0)
      {
        selected_parameter = 1;
        selected_ev_on_screen = 0;
        selected_line_on_screen = 0;
      }
      draw_rectangle(11);
      break;
    case 1:
      if (arrayofButton[0].type == 0)
      {
        selected_parameter = 2;
      }
      draw_rectangle(22);
      break;
    case 2:
      if (arrayofButton[0].type == 0)
      {
        selected_parameter = 3;
      }
      draw_rectangle(33);
      break;
    case 3:
      if (arrayofButton[0].type == 0)
      {
        selected_parameter = 4;
      }
      draw_rectangle(44);
      break;
    }
    if (arrayofButton[0].type == 0)
    {
      arrayofButton[0].type = -1;
      selected_line_on_screen = 0;
    }
    u8g.drawStr(10, 11, "parametre");
    switch (selected_line_on_screen)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g.drawStr(5, 22, "les sorties");
      u8g.drawStr(5, 33, "boutons");
      u8g.drawStr(5, 44, "heure");
      u8g.drawStr(5, 55, "Autre");
      break;
    }
    break;
  }
}

void draw_rectangle(int line)
{
  int x = 0;
  int y = line;
  int length = 4;
  int height = 11;
  u8g.drawBox(x, y, length, height);
}

void select_output_parameter_screen()
{
  switch (selected_ev_on_screen)
  {
  case 0:
    if (arrayofButton[0].type == 2)
    {
      arrayofButton[0].type = -1;
      selected_parameter = 0;
      selected_line_on_screen = 0;
    }
    change_selected_line(7);
    u8g.drawStr(5, 22, "sortie");
    u8g.drawStr(5, 33, "sortie");
    u8g.drawStr(5, 44, "sortie");
    u8g.drawStr(5, 56, "sortie");
    u8g.drawStr(10, 11, "les sorties");
    switch (selected_line_on_screen)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g.drawStr(50, 22, "1");
      u8g.drawStr(50, 33, "2");
      u8g.drawStr(50, 44, "3");
      u8g.drawStr(50, 56, "4");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      u8g.drawStr(50, 22, "5");
      u8g.drawStr(50, 33, "6");
      u8g.drawStr(50, 44, "7");
      u8g.drawStr(50, 56, "8");
      break;
    }
    switch (selected_line_on_screen)
    {
    case 0:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 1;
      }
      draw_rectangle(11);
      break;
    case 1:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 2;
      }
      draw_rectangle(22);
      break;
    case 2:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 3;
      }
      draw_rectangle(33);
      break;
    case 3:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 4;
      }
      draw_rectangle(44);
      break;
    case 4:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 5;
      }
      draw_rectangle(11);
      break;
    case 5:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 6;
      }
      draw_rectangle(22);
      break;
    case 7:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 7;
      }
      draw_rectangle(33);
      break;
    case 6:
      if (arrayofButton[0].type == 0)
      {
        arrayofButton[0].type = -1;
        selected_line_on_screen = 0;
        selected_ev_on_screen = 8;
      }
      draw_rectangle(44);
      break;
    }
    break;
  default:
    output_parameter_screen(selected_ev_on_screen);
    break;
  }
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
    time_on = EEPROM.read(mem_address[7][0]);
    break;
  case 4:
    time_on = EEPROM.read(mem_address[7][1]);
    break;
  case 3:
    time_on = EEPROM.read(mem_address[7][2]);
    break;
  case 2:
    time_on = EEPROM.read(mem_address[7][3]);
    break;
  }
  if (arrayofButton[1].type != -1)
  {
    arrayofButton[1].type = -1;
  }
  if (time_on >= 1)
  {
    int mem_value;
    mem_value = EEPROM.read(mem_adress);
    mem_value = mem_value + time_on;
    if (mem_value >= max)
    {
      mem_value = mem_value - max;
    }
    write_eeprom(mem_adress, mem_value);
  }
}

void output_parameter_screen(int ev)
{
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_ev_on_screen = 0;
    selected_line_on_screen = 0;
  }
  change_selected_line(3);
  int mem_value;
  switch (selected_line_on_screen)
  {
  case 0:
    draw_rectangle(11);
    change_value_using_button(mem_address[4][ev - 1], 1);
    break;
  case 1:
    draw_rectangle(22);
    minus_button(mem_address[0][ev - 1], 255);
    change_value_using_button(mem_address[0][ev - 1], 255);
    break;
  case 2:
    draw_rectangle(33);
    change_value_using_button(mem_address[1][ev - 1], 15);
    break;
  case 3:
    draw_rectangle(44);
    change_value_using_button(mem_address[3][ev - 1], 23);
    break;
  }

  print_on_screen(90, 11, ev);
  u8g.drawStr(10, 11, "sortie ");
  switch (selected_line_on_screen)
  {
  case 0:
  case 1:
  case 2:
  case 3:
    // print auto mode state
    activate_auto_mode_screen(mem_address[4][ev - 1], 90, 22);
    // print auto time on
    mem_value = EEPROM.read(mem_address[0][ev - 1]);
    print_on_screen(70, 33, mem_value);
    // print auto frequency
    print_mem_value(70, 44, mem_address[1][ev - 1]);
    // print auto start hour
    print_mem_value(70, 55, mem_address[3][ev - 1]);
    u8g.drawStr(5, 22, "Etat:");
    u8g.drawStr(5, 33, "Temps on :");
    u8g.drawStr(5, 44, "Tous les");
    u8g.drawStr(110, 44, "j");
    u8g.drawStr(5, 56, "Heure :");
    break;
  }
}

void buttons_parameter_screen()
{
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }

  change_selected_line(3);
  switch (selected_line_on_screen)
  {
  case 0:
    draw_rectangle(11);
    minus_button(170, 60);
    change_value_using_button(170, 60);
    break;
  case 1:
    draw_rectangle(22);
    minus_button(171, 120);
    change_value_using_button(171, 120);
    break;
  case 2:
    draw_rectangle(33);
    minus_button(172, 180);
    change_value_using_button(172, 180);
    break;
  case 3:
    draw_rectangle(44);
    minus_button(173, 255);
    change_value_using_button(173, 255);
    break;
  }
  print_mem_value(75, 22, 170);
  print_mem_value(75, 33, 171);
  print_mem_value(75, 44, 172);
  print_mem_value(75, 56, 173);

  u8g.drawStr(10, 11, "les boutons");
  u8g.drawStr(5, 22, "bouton ");
  u8g.drawStr(40, 22, "1 :");
  u8g.drawStr(5, 33, "bouton ");
  u8g.drawStr(40, 33, "2 :");
  u8g.drawStr(5, 44, "bouton ");
  u8g.drawStr(40, 44, "3 :");
  u8g.drawStr(5, 55, "bouton ");
  u8g.drawStr(40, 55, "4 :");
}

void clock_parameter_screen()
{
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }
  change_selected_line(5);
  switch (selected_line_on_screen)
  {
  case 0: // day
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int day;
      day = rtc[4];
      day = day + 1;
      if (day >= 31)
      {
        day = 0;
      }
      RTC.stop();
      RTC.set(DS1307_DATE, day);
      RTC.start();
    }
    u8g.drawBox(7, 22, 4, 11);
    break;
  case 1: // month
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int month;
      month = rtc[5];
      month = month + 1;
      if (month >= 12)
      {
        month = 0;
      }
      RTC.stop();
      RTC.set(DS1307_MTH, month);
      RTC.start();
    }
    u8g.drawBox(28, 22, 4, 11);
    break;
  case 2: // annee
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int year;
      year = rtc[6];
      year = year - 2000;
      year = year + 1;
      RTC.stop();
      RTC.set(DS1307_YR, year);
      RTC.start();
    }
    u8g.drawBox(48, 22, 4, 11);
    break;
  case 3: // Hour
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int hour;
      hour = rtc[2];
      hour++;
      if (hour >= 24)
      {
        hour = 0;
      }

      RTC.stop();
      RTC.set(DS1307_HR, hour);
      RTC.start();
    }

    u8g.drawBox(7, 44, 4, 11);
    break;
  case 4: // minute
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int minute;
      minute = rtc[1];
      minute++;
      if (minute >= 60)
      {
        minute = 0;
      }
      RTC.stop();
      RTC.set(DS1307_MIN, minute);
      RTC.start();
    }

    u8g.drawBox(25, 44, 4, 11);
    break;
  case 5: // seconde
    if (arrayofButton[1].type == 0)
    {
      arrayofButton[1].type = -1;
      int seconde;
      seconde = 0;
      RTC.stop();
      RTC.set(DS1307_SEC, seconde);
      RTC.start();
    }
    u8g.drawBox(45, 44, 4, 11);
    break;
  }

  u8g.drawStr(10, 11, " Heure ");
  print_on_screen(7, 22, rtc[4]);
  print_on_screen(28, 22, rtc[5]);
  print_on_screen(48, 22, rtc[6]);
  u8g.drawStr(20, 22, "/");
  u8g.drawStr(40, 22, "/");
  u8g.drawStr(20, 44, ":");
  u8g.drawStr(40, 44, ":");
  print_on_screen(7, 44, rtc[2]);
  print_on_screen(25, 44, rtc[1]);
  print_on_screen(45, 44, rtc[0]);
}

void other_parameter_screen()
{
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }

  change_selected_line(1);
  switch (selected_line_on_screen)
  {
  case 0:
    draw_rectangle(11);
    change_value_using_button(160, 1);
    break;
  case 1:
    draw_rectangle(22);
    change_value_using_button(161, 30);
    break;
  case 2:
    draw_rectangle(33);
    change_value_using_button(162, max_time_on_ev);
    break;
  case 3:
    draw_rectangle(44);
    change_value_using_button(163, 15);
    break;
  case 4:
    draw_rectangle(11);
    change_value_using_button(164, max_time_on_ev);
    break;
  case 5:
    draw_rectangle(22);
    change_value_using_button(165, 15);
    break;
  }
  u8g.drawStr(10, 11, "divers");
  switch (selected_line_on_screen)
  {
  case 0:
  case 1:
  case 2:
  case 3:
    u8g.drawStr(5, 22, "Auto ete/hiver :");
    activate_auto_mode_screen(160, 90, 22);
    u8g.drawStr(5, 33, "Temp ete/hiver :");
    print_mem_value(70, 33, 161);
    u8g.drawStr(5, 44, "Duree ete :");
    print_mem_value(70, 44, 162);
    u8g.drawStr(5, 55, "Freq ete :");
    print_mem_value(70, 55, 163);
    break;
  case 4:
  case 5:
    u8g.drawStr(5, 22, "duree hiver :");
    print_mem_value(70, 22, 164);
    u8g.drawStr(5, 33, "Freq hivers :");
    print_mem_value(70, 33, 165);
    break;
  }

  u8g.drawStr(10, 11, "Autre");
  u8g.drawStr(5, 22, "Auto ete/hiver :");
  u8g.drawStr(5, 33, "temp ete/hiver :");
  u8g.drawStr(5, 44, "duree ete :");
  u8g.drawStr(5, 55, "Freq ete :");
  u8g.drawStr(5, 11, "duree hivers :");
  u8g.drawStr(5, 55, "Freq hivers :");
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
  if (EEPROM.read(mem_address[4][ev - 1]) == 0)
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
    send_to_module(adr, subScreen_selected);
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
  write_eeprom(150, temp_value);
  int humidity_value;
  humidity_value = (int)humidity.relative_humidity;
  write_eeprom(151, humidity_value);

  int temp_change_season;
  temp_change_season = EEPROM.read(161);

  for (int i = 0; i < 8; i++)
  {
    if (temp_value <= temp_change_season)
    {
      mem_value = EEPROM.read(mem_address[6][2]);
      write_eeprom(mem_address[0][i], mem_value);
      mem_value = EEPROM.read(mem_address[6][3]);
      write_eeprom(mem_address[1][i], mem_value);
    }
    else
    {
      mem_value = EEPROM.read(164);
      write_eeprom(mem_address[0][i], mem_value);
      mem_value = EEPROM.read(165);
      write_eeprom(mem_address[1][i], mem_value);
    }
  }
}

void loop_actualization()
{
  if (rtc_min != rtc[1])
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
    rtc_min = rtc[1];
  }

  if (manual_mode_state == 0)
  {
    horlact();
  }

  // Check Temperature and humidity at 12 o'clock if auto mode on with aht21
  int mem_value;
  mem_value = EEPROM.read(160);
  if (mem_value == 1)
  {
    if (rtc_day != rtc[4])
    {
      if (rtc[2] == 12)
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
    if (EEPROM.read(mem_address[2][i]) == 1)
    {
      // if ev state is on
      if (EEPROM.read(mem_address[4][i]) == 1)
      {
        // if clock h == auto start time
        if (rtc[2] == EEPROM.read(mem_address[3][i]))
        {
          if (arrayOfEV[i].remainingTimeOn == 0)
          {
            // if not started once set next day as today
            if (arrayOfEV[i].nextDayOn == 0)
            {
              arrayOfEV[i].nextDayOn = rtc[4];
            }
            // if clock day == next start day
            if (arrayOfEV[i].nextDayOn == rtc[4])
            {
              // mise en route
              time_on = EEPROM.read(mem_address[0][i]);
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
        if (arrayOfEV[i].nextDayOn == rtc[4])
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
    day_to_add = EEPROM.read(mem_address[1][i]);
  }
  else
  {
    day_to_add = 1;
  }
  day = rtc[4] + day_to_add;
  switch (rtc[5])
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
    manual_selected_ev = arrayofButton[2].type + 1;
    arrayofButton[2].type = -1;
  }
  if (arrayofButton[0].type == 2)
  {
    arrayofButton[0].type = -1;
    selected_screen = 0;
    subScreen_selected = 0;
  }
  switch (arrayofButton[1].type)
  {
  case 5:
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(170);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 4:
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(171);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 3:
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(172);
    manual_time_on = manual_time_on + mem_value;
    break;
  case 2:
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(173);
    manual_time_on = manual_time_on + mem_value;
    break;
  }
  if (manual_time_on >= max_time_on_ev)
  {
    manual_time_on = manual_time_on - max_time_on_ev;
  }
  sprintf(buf, "%d", manual_selected_ev);
  u8g.drawStr(70, 22, buf);
  sprintf(buf, "%d", manual_time_on);
  u8g.drawStr(60, 33, buf);
  u8g.drawStr(10, 11, " manuel ");
  u8g.drawStr(5, 22, " sortie n :");
  u8g.drawStr(5, 33, " temp :");

  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    arrayOfEV[manual_selected_ev - 1].remainingTimeOn = manual_time_on;

    manual_time_on = 0;
    manual_selected_ev = 1;
    subScreen_selected = 0;
    selected_screen = 0;
  }
}

void write_eeprom(int mem_adress, int val)
{
  int mem_value;
  mem_value = EEPROM.read(mem_adress);
  if (mem_value != val)
  {
    EEPROM.write(mem_adress, val);
  }
}

void delay_screen()
{
  if (arrayofButton[2].type != -1)
  {
    manual_selected_ev = arrayofButton[2].type + 1;
    arrayofButton[2].type = -1;
  }

  sprintf(buf, "%d", manual_selected_ev);
  u8g.drawStr(70, 22, buf);
  u8g.drawStr(10, 11, " arret ");
  u8g.drawStr(5, 22, " sortie n :");

  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    arrayOfEV[manual_selected_ev - 1].remainingTimeOn = 0;

    manual_selected_ev = 1;
    subScreen_selected = 0;
    selected_screen = 0;
  }
}

void print_mem_value(int col, int line, int mem_address)
{
  int mem_value;
  mem_value = EEPROM.read(mem_address);
  sprintf(buf, "%d", mem_value);
  u8g.drawStr(col, line, buf);
}

void print_on_screen(int col, int line, int num)
{
  sprintf(buf, "%d", num);
  u8g.drawStr(col, line, buf);
}

void change_selected_line(int num)
{
  if (arrayofButton[0].type == 1)
  {
    arrayofButton[0].type = -1;
    if (selected_line_on_screen == 0)
    {
      selected_line_on_screen = num;
    }
    else
    {
      selected_line_on_screen--;
    }
  }
  if (arrayofButton[0].type == 3)
  {
    arrayofButton[0].type = -1;
    if (selected_line_on_screen == num)
    {
      selected_line_on_screen = 0;
    }
    else
    {
      selected_line_on_screen++;
    }
  }
}

void change_value_using_button(int mem_address, int max_value)
{
  int mem_value;
  if (arrayofButton[1].type == 0)
  {
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(mem_address);
    mem_value++;
    if (mem_value > max_value)
    {
      mem_value = 0;
    }
    if (mem_value <= max_value)
    {
      write_eeprom(mem_address, mem_value);
    }
  }
  if (arrayofButton[1].type == 1)
  {
    arrayofButton[1].type = -1;
    mem_value = EEPROM.read(mem_address);
    if (mem_value <= 0)
    {
      mem_value = max_value;
    }
    else
    {
      mem_value--;
    }
    if (mem_value >= 0)
    {
      write_eeprom(mem_address, mem_value);
    }
  }
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