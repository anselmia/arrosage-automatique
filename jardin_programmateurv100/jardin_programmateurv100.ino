/* memory address
20:90 auto time on
21:91 auto frequency
22:92 output state
23:93 auto start time
27:97 auto mode state
150 day temp at 12
151 day humidity at 12
160 auto mode with ath21 sensor
161 winter/ete temp
162 duration summer
163 frequency summer
164 duration winter
165 frequency winter
170:171 manual time value from button 1 to 4
*/

#include <Wire.h>//i2c
#include "Arduino.h"
#include <EEPROM.h>
#include <U8glib.h>
#include <avr/wdt.h>
#include <DS1307.h>
#include <Adafruit_AHTX0.h>

int versio=100;
boolean aar=true;//mode wire endtransmission
int module_state[3];
int manual_mode_state = 0;
int next_day_on[8];
int ev_remaining_time_on[8];
int manual_selected_ev = 0;
int manual_time_on = 0;
char buf[10];
int ecran_princ_min = 0;
int ecran_princ_sec = 0;
int selected_line_on_screen = 0;
int main_screen_selected = 0;
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

//bouton
const byte NOT_PRESSED = 0;
const byte PUSHED = 1;
const byte PRESSED = 2;
const byte NO_EVENT = 0;
const byte EVENT_PRESSED = 1;
const byte EVENT_RELEASED = 2;

//line 1
const int pin_button_line1 = 0;
byte saved_button_state_line1 = NOT_PRESSED;
int button_state_line1 = -1;

//line 2
const int pin_button_line2 = 1;
byte saved_button_state_line2 = NOT_PRESSED;
int button_state_line2 = -1;
//line 3
const int pin_button_line3 = 3;
byte saved_button_state_line3 = NOT_PRESSED;
int button_state_line3 = -1;

int button_state=0;
int selected_screen = 0;
int plus_button_state = 0;
int minus_button_state = 0;
int up_button_state = 0;
int down_button_state = 0;
int left_button_state = 0;
int right_button_state = 0;
int other_button_state = 0;
int selected_ev = 0;
int error = 0;

// Lecture poussoire entrée 0
int read_button_line_1()
{
  int selected_button_line1 = (analogRead(pin_button_line1) + 190) / 128;
  int new_button_state_line1 = button_state_line1; /* Ã  priori rien ne change */
  switch (saved_button_state_line1) {
    case NOT_PRESSED:
      if (selected_button_line1 < 9)
        saved_button_state_line1 = PUSHED;
      break;
    case PUSHED:
      if (selected_button_line1 < 9) {
        saved_button_state_line1 = PRESSED;
        new_button_state_line1 = selected_button_line1;
      }
      else {
        saved_button_state_line1 = NOT_PRESSED;
      }
      break;
    case PRESSED:
      if (selected_button_line1 == 9) {
        saved_button_state_line1 = NOT_PRESSED;
        new_button_state_line1 = -1;
      }
      break;
  }

  return new_button_state_line1;
}

/*
   construction d'un Ã©vÃ©nement en comparant
   le nouvel Ã©tat des poussoirs avec l'Ã©tat prÃ©cÃ©dent.
*/
byte read_event_button_line1(int *selected_button_line1)
{
  byte event_button;
  int new_button_state_line1 = read_button_line_1();

  if (new_button_state_line1 == button_state_line1)
    event_button = NO_EVENT;
  if (new_button_state_line1 >= 0 && button_state_line1 == -1)
    event_button = EVENT_PRESSED;
  if (new_button_state_line1 == -1 && button_state_line1 >= 0)
    event_button = EVENT_RELEASED;
  button_state_line1 = new_button_state_line1;
  *selected_button_line1 = button_state_line1;

  return event_button;
}

// Lecture poussoire entrée 1
int read_button_line_2()
{
  int selected_button_line2 = (analogRead(pin_button_line2) + 190) / 128;
  int new_button_state_line2 = button_state_line2; /* Ã  priori rien ne change */
  switch (saved_button_state_line2) {
    case NOT_PRESSED:
      if (selected_button_line2 < 9)
        saved_button_state_line2 = PUSHED;
      break;
    case PUSHED:
      if (selected_button_line2 < 9) {
        saved_button_state_line2 = PRESSED;
        new_button_state_line2 = selected_button_line2;
      }
      else {
        saved_button_state_line2 = NOT_PRESSED;
      }
      break;
    case PRESSED:
      if (selected_button_line2 == 9) {
        saved_button_state_line2 = NOT_PRESSED;
        new_button_state_line2 = -1;
      }
      break;
  }

  return new_button_state_line2;
}

/*
   construction d'un Ã©vÃ©nement en comparant
   le nouvel Ã©tat des poussoirs avec l'Ã©tat prÃ©cÃ©dent.
*/
byte read_event_button_line2(int *selected_button_line2)
{
  byte event_button;
  int new_button_state_line2 = read_button_line_2();

  if (new_button_state_line2 == button_state_line2)
    event_button = NO_EVENT;
  if (new_button_state_line2 >= 0 && button_state_line2 == -1)
    event_button = EVENT_PRESSED;
  if (new_button_state_line2 == -1 && button_state_line2 >= 0)
    event_button = EVENT_RELEASED;
  button_state_line2 = new_button_state_line2;
  *selected_button_line2 = button_state_line2;

  return event_button;
}

// Lecture poussoire entrée 3
int read_button_line_3()
{
  int selected_button_line3 = (analogRead(pin_button_line3) + 190) / 128;
  int new_button_state_line3 = button_state_line3; /* Ã  priori rien ne change */
  switch (saved_button_state_line3) {
    case NOT_PRESSED:
      if (selected_button_line3 < 9)
        saved_button_state_line3 = PUSHED;
      break;
    case PUSHED:
      if (selected_button_line3 < 9) {
        saved_button_state_line3 = PRESSED;
        new_button_state_line3 = selected_button_line3;
      }
      else {
        saved_button_state_line3 = NOT_PRESSED;
      }
      break;
    case PRESSED:
      if (selected_button_line3 == 9) {
        saved_button_state_line3 = NOT_PRESSED;
        new_button_state_line3 = -1;
      }
      break;
  }

  return new_button_state_line3;
}

/*
   construction d'un Ã©vÃ©nement en comparant
   le nouvel Ã©tat des poussoirs avec l'Ã©tat prÃ©cÃ©dent.
*/
byte read_event_button_line3(int *selected_button_line3)
{
  byte event_button;
  int new_button_state_line3 = read_button_line_3();

  if (new_button_state_line3 == button_state_line3)
    event_button = NO_EVENT;
  if (new_button_state_line3 >= 0 && button_state_line3 == -1)
    event_button = EVENT_PRESSED;
  if (new_button_state_line3 == -1 && button_state_line3 >= 0)
    event_button = EVENT_RELEASED;
  button_state_line3 = new_button_state_line3;
  *selected_button_line3 = button_state_line3;

  return event_button;
}

void setup() {
  wdt_enable(WDTO_4S);// watchdog 4 seconde reinitialisation
  Wire.begin();
  Wire.setClock(31000L);//reglage de horloge de i2c
  u8g.setColorIndex(1);
  DDRC |= _BV(2) | _BV(3); // POWER:Vcc Gnd
  PORTC |= _BV(3); // VCC PINC3
  // initialisation de ds1307
  RTC.get(rtc, true);
  RTC.SetOutput(DS1307_SQW32KHZ);

  Serial.begin(9600);
  Init();
}

void prinheu() {
  ecran_princ_min = rtc[1] + 2;
  if (ecran_princ_min >= 60) {
    ecran_princ_min = 0;
  }
  ecran_princ_sec = rtc[0];
  main_screen_selected = 1;
}

void prinheu2() {
  if (main_screen_selected == 0) {
    prinheu();
  }
}

void send_to_module(int adr, int val) {
  Wire.beginTransmission(adr);
  Wire.write(val);
  error= Wire.endTransmission(aar);
}

void init_ev_state() {
  // Set all EV off
  send_to_module(addr_mod_relay, 102);
  ev_remaining_time_on[0] = 0;
  ev_remaining_time_on[1] = 0;
  ev_remaining_time_on[2] = 0;
  ev_remaining_time_on[3] = 0;
  ev_remaining_time_on[4] = 0;
  ev_remaining_time_on[5] = 0;
  ev_remaining_time_on[6] = 0;
  ev_remaining_time_on[7] = 0;
}

void bouton1(int selected_button) {
  switch (selected_button) {
    case 1:// right
      prinheu();
      right_button_state = 1;
      break;
    case 5: // up
      prinheu();
      up_button_state = 1;
      if (selected_screen == 0) {
        selected_screen = 7;
      }
      break;
    case 7:// left
      prinheu();
      left_button_state = 1;
      break;
    case 8:// down
      prinheu();
      down_button_state = 1;
      break;
  }
}

void bouton2(int selected_button) {
  switch (selected_button) {
    case 1://+
      prinheu();
      plus_button_state = 1;
      switch (selected_screen) {
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
    case 2://-
      prinheu();
      minus_button_state = 1;
      if(selected_screen==0){
        main_screen_selected=0;
        selected_line_on_screen=0;
        selected_parameter=0;
      }
      break;
    case 3://b4
      prinheu();
      other_button_state = 4;
      if (selected_screen == 0) {
        selected_screen = 8;
      }
      break;
    case 4://b3
      prinheu();
      other_button_state = 3;
      if (selected_screen == 0) {
        selected_screen = 10;
      }
      break;
    case 5://b2
      prinheu();
      other_button_state = 2;
      if (selected_screen == 0) {
        selected_screen = 9;
      }
      break;
    case 6://b1
      prinheu();
      other_button_state = 1;
      if (selected_screen == 0) {
        selected_screen = 3;
      }
      break;
  }
}

void bouton3(int selected_button) {
  switch (selected_button) {
    case 1://s7
      prinheu2();
      selected_ev = 7;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 2://s6
      prinheu2();
      selected_ev = 6;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 3://s5
      prinheu2();
      selected_ev = 5;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 4://s4
      prinheu2();
      selected_ev = 4;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 5://s3
      prinheu2();
      selected_ev = 3;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 6://s2
      prinheu2();
      selected_ev = 2;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
    case 7:
      prinheu2();
      break;
    case 8://s1
      prinheu2();
      selected_ev = 1;
      if (selected_screen <= 1) {
        selected_screen = 4;
      }
      break;
  }
}

void screen_selection() {
  RTC.get(rtc, true);
  if (ecran_princ_min <= rtc[1]) {
    if (ecran_princ_sec <= rtc[0]) {
      selected_line_on_screen = 0;
      main_screen_selected = 0;
      selected_screen = 0;
      selected_ev_on_screen = 0;
      selected_parameter = 0;
      manual_mode_state = 0;
    }
  }

  if (main_screen_selected == 0) {
    manual_mode_state = 0;
    main_screen();
  } else {
    select_screen();
  }
}

void loop() {
  wdt_reset();//reset watchdog

  // Button line 1
  int selected_button_line1;
  byte event_button = read_event_button_line1(&selected_button_line1);
  switch (event_button) {
    case EVENT_PRESSED:
      if(button_state==0){
        bouton1(selected_button_line1);
        button_state=1;
      }
    break;
    case EVENT_RELEASED:
      button_state=0;
    break;
  }

  // Button line 2
  int selected_button_line2;
  byte event_button2 = read_event_button_line2(&selected_button_line2);
  switch (event_button2) {
    case EVENT_PRESSED:
      if(button_state==0){
        bouton2(selected_button_line2);
        button_state=1;
      }
      break;
    case EVENT_RELEASED:
      button_state=0;
      break;
  }

  //line poussoir 3
  int selected_button_line3;
  byte event_button3 = read_event_button_line3(&selected_button_line3);
  switch (event_button3) {
    case EVENT_PRESSED:
      if(button_state==0){
        bouton3(selected_button_line3);
        button_state=1;
      }
      break;
    case EVENT_RELEASED:
      button_state=0;
      break;
  }

  u8g.firstPage(); // SÃ©lectionne la 1er page mÃ©moire de l'Ã©cran
  do {
    u8g.setFont(u8g_font_tpss); // Utilise la police de caractÃ¨re standard
    screen_selection();
  } while (u8g.nextPage()); // SÃ©lectionne la page mÃ©moire suivante
  
  delay(100);
  loop_actualization();
  update_all_ev_state();
  if (EEPROM.read(27) == 0) {
    if(ev_remaining_time_on[0]!=0){
      ev_remaining_time_on[0]=0;
    }
  }
  if (EEPROM.read(37) == 0) {
    if(ev_remaining_time_on[1]!=0){
      ev_remaining_time_on[1]=0;
    }
  }
  if (EEPROM.read(47) == 0) {
    if(ev_remaining_time_on[2]!=0){
          ev_remaining_time_on[2]=0;
    }
  }
  if (EEPROM.read(57) == 0) {
    if(ev_remaining_time_on[3]!=0){
          ev_remaining_time_on[3]=0;
    }
  }
  if (EEPROM.read(67) == 0) {
    if(ev_remaining_time_on[4]!=0){
          ev_remaining_time_on[4]=0;
    }
  }
  if (EEPROM.read(77) == 0) {
    if(ev_remaining_time_on[5]!=0){
          ev_remaining_time_on[5]=0;
    }
  }
  if (EEPROM.read(87) == 0) {
    if(ev_remaining_time_on[6]!=0){
          ev_remaining_time_on[6]=0;
    }
  }
  if (EEPROM.read(97) == 0) {
    if(ev_remaining_time_on[7]!=0){
          ev_remaining_time_on[7]=0;
    }
  }
}

void inii2c(int adr){  
  Wire.beginTransmission(adr);
  error = Wire.endTransmission(aar);
}

void print_actual_time(){  
  u8g.drawStr( 10, 11, " Heure ");        
  print_on_screen(7, 22, rtc[4]);
  print_on_screen(28, 22, rtc[5]);
  print_on_screen(48, 22, rtc[6]);
  u8g.drawStr( 20, 22, "/");
  u8g.drawStr( 40, 22, "/");
  u8g.drawStr( 20, 33, ":");
  u8g.drawStr( 40, 33, ":");
  print_on_screen(7, 33, rtc[2]);
  print_on_screen(25, 33, rtc[1]);
  print_on_screen(45, 33, rtc[0]);
}

void Init() {
  delay(1000);
  // Init com with arduino relay module
  inii2c(addr_mod_relay);
  if (error != 0) {
    module_state[0] = 1;
  }
  // Init com with ds1307
  inii2c(0x68); 
  if (error != 0) {
    module_state[1] = 1;
  }
  // Init com with aht21 sensor
  inii2c(0x38); 
  if (error != 0) {
    module_state[2] = 1;
  }

  init_ev_state();

  u8g.firstPage(); // SÃ©lectionne la 1er page mÃ©moire de l'Ã©cran
  do {
      u8g.setFont(u8g_font_tpss); // Utilise la police de caractÃ¨re standard
      u8g.drawStr( 5, 11, "Initialisation"); // 12 line
      if (module_state[1] == 0) {
        RTC.get(rtc, true);
        print_actual_time();
      } else {
        u8g.drawStr( 10, 22, "erreur horloge");
      }
      u8g.drawStr(5, 44, "version");
      print_on_screen(50, 44, versio);
    } while (u8g.nextPage());

  delay(2000);
}

void main_screen() {
  if (error == 1) {
   u8g.drawStr(20, 11, "erreur");
  } else {
    print_actual_time();
  }
  print_active_ev(); 
}

void select_screen() { 
  switch (selected_screen) {
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

void automatic_mode_status_screen() {
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_screen = 0;
    main_screen_selected = 0;
  }
  change_selected_line(1);
  int mem_address = 0;
  int mem_value = 0;
  switch (selected_line_on_screen) {
    case 0:
      draw_rectangle(11); 
      if (selected_ev != 0) {
        switch (selected_ev) {
          case 1:
            mem_address = 22;
            break;
          case 2:
            mem_address = 32;
            break;
          case 3:
            mem_address = 42;
            break;
          case 4:
            mem_address = 52;
            break;
          case 5:
            mem_address = 62;
            break;
          case 6:
            mem_address = 72;
            break;
          case 7:
            mem_address = 82;
            break;
          case 8:
            mem_address = 92;
            break;
        }        
        selected_ev = 0;
        if (EEPROM.read(mem_address) == 1) {
          mem_value = 0;
        } else {
          mem_value = 1;
        }
        write_eeprom(mem_address, mem_value);
      }
      break;
    case 1:
      draw_rectangle(33);
      if (selected_ev != 0) {
        switch (selected_ev) {
          case 1:
            mem_address = 27;
            break;
          case 2:
            mem_address = 37;
            break;
          case 3:
            mem_address = 47;
            break;
          case 4:
            mem_address = 57;
            break;
          case 5:
            mem_address = 67;
            break;
          case 6:
            mem_address = 77;
            break;
          case 7:
            mem_address = 87;
            break;
          case 8:
            mem_address = 97;
            break;
        }

        selected_ev = 0;
        if (EEPROM.read(mem_address) == 1) {
          mem_value = 0;
        } else {
          mem_value = 1;
        }
        write_eeprom(mem_address, mem_value);
      }
      break;
  }
  u8g.drawStr( 2, 11, "activation des sorties :");
  u8g.drawStr( 6, 22, "mode automatique :");
  if (EEPROM.read(22) == 1) {
    u8g.drawStr( 2, 33, "1");
  }
  if (EEPROM.read(32) == 1) {
    u8g.drawStr( 15, 33, "2");
  }
  if (EEPROM.read(42) == 1) {
    u8g.drawStr( 32, 33, "3");
  }
  if (EEPROM.read(52) == 1) {
    u8g.drawStr( 52, 33, "4");
  }
  if (EEPROM.read(62) == 1) {
    u8g.drawStr( 72, 33, "5");
  }
  if (EEPROM.read(72) == 1) {
    u8g.drawStr( 92, 33, "6");
  }
  if (EEPROM.read(82) == 1) {
    u8g.drawStr( 112, 33, "7");
  }
  if (EEPROM.read(92) == 1) {
    u8g.drawStr( 132, 33, "8");
  }
  u8g.drawStr( 6, 44, "sorties actives :");
  if (EEPROM.read(27) == 1) {
    u8g.drawStr( 2, 56, "1");
  }
  if (EEPROM.read(37) == 1) {
    u8g.drawStr( 15, 56, "2");
  }
  if (EEPROM.read(47) == 1) {
    u8g.drawStr( 32, 56, "3");
  }
  if (EEPROM.read(57) == 1) {
    u8g.drawStr( 52, 56, "4");
  }
  if (EEPROM.read(67) == 1) {
    u8g.drawStr( 72, 56, "5");
  }
  if (EEPROM.read(77) == 1) {
    u8g.drawStr( 92, 56, "6");
  }
  if (EEPROM.read(87) == 1) {
    u8g.drawStr( 112, 56, "7");
  }
  if (EEPROM.read(97) == 1) {
    u8g.drawStr( 132, 56, "8");
  }
}

void manual_mode_screen() {
  if (plus_button_state == 1) {
    manual_mode_state = 0;
    left_button_state = 1;
    main_screen_selected=0;
  } 
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_screen = 0;
    main_screen_selected = 0;
    manual_mode_state = 0;
  }  
  u8g.drawStr( 20, 11, " mode manuel");
  ecran_princ_min = 60;
  manual_mode_state = 1;
  if (selected_ev != 0) {
    if (ev_remaining_time_on[selected_ev - 1] == 0) {
      ev_remaining_time_on[selected_ev - 1] = 10;
    } else {
      ev_remaining_time_on[selected_ev - 1] = 0;
    }
    selected_ev = 0;
  }
  print_active_ev(); 
}

void print_active_ev(){
   if (ev_remaining_time_on[0] != 0) {
    u8g.drawStr( 2, 56, "1");
  }
  if (ev_remaining_time_on[1] != 0) {
    u8g.drawStr( 15, 56, "2");
  }
  if (ev_remaining_time_on[2] != 0) {
    u8g.drawStr( 32, 56, "3");
  }
  if (ev_remaining_time_on[3] != 0) {
    u8g.drawStr( 52, 56, "4");
  }
  if (ev_remaining_time_on[4] != 0) {
    u8g.drawStr( 72, 56, "5");
  }
  if (ev_remaining_time_on[5] != 0) {
    u8g.drawStr( 92, 56, "6");
  }
  if (ev_remaining_time_on[6] != 0) {
    u8g.drawStr( 112, 56, "7");
  }
  if (ev_remaining_time_on[7] != 0) {
    u8g.drawStr( 132, 56, "8");
  }
}

void output_screen() {
  if (left_button_state == 1) {
    selected_screen = 0;
    left_button_state = 0;
    main_screen_selected = 0;
  }
  change_selected_line(1);
  switch (selected_line_on_screen) {
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
      print_on_screen(57, 11, ev_remaining_time_on[0]);
      print_on_screen(57, 22, ev_remaining_time_on[1]);
      print_on_screen(57, 33, ev_remaining_time_on[2]);
      print_on_screen(57, 44, ev_remaining_time_on[3]);
      print_on_screen(57, 56, ev_remaining_time_on[4]);
      u8g.drawStr(85, 33, ",");
      u8g.drawStr(85, 44, ",");
      u8g.drawStr(85, 56, ",");
      print_on_screen(90, 11, next_day_on[0]);
      print_on_screen(90, 22, next_day_on[1]);
      print_on_screen(90, 33, next_day_on[2]);
      print_on_screen(90, 44, next_day_on[3]);
      print_on_screen(90, 56, next_day_on[4]);      
      break;
    case 1:
      u8g.drawStr(0, 11, "sortie");
      u8g.drawStr(0, 22, "sortie");
      u8g.drawStr(0, 33, "sortie");      
      u8g.drawStr(35, 11, "6 :");
      u8g.drawStr(35, 22, "7 :");
      u8g.drawStr(35, 33, "8 :");
      print_on_screen(57, 11, ev_remaining_time_on[5]);
      print_on_screen(57, 22, ev_remaining_time_on[6]);
      print_on_screen(57, 33, ev_remaining_time_on[7]);
      u8g.drawStr(85, 11, ",");
      u8g.drawStr(85, 22, ",");
      u8g.drawStr(85, 33, ",");
      print_on_screen(90, 11, next_day_on[5]);
      print_on_screen(90, 22, next_day_on[6]);
      print_on_screen(90, 33, next_day_on[7]);
      break;
  }
}

void parameter_screen() {
  switch (selected_parameter) {
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
      if (left_button_state == 1) {
        left_button_state = 0;
        selected_screen = 0;
        main_screen_selected = 0;
        selected_parameter = 0;
        selected_line_on_screen = 0;
      } 
      plus_button_state = 0;
      minus_button_state = 0;
      change_selected_line(4);
      switch (selected_line_on_screen) {
        case 0:
          if (right_button_state == 1) {
            selected_parameter = 1;
            selected_ev_on_screen = 0;
            selected_line_on_screen = 0;
          }
          draw_rectangle(11);
          break;
        case 1:
          if (right_button_state == 1) {
            selected_parameter = 2;
          }
          draw_rectangle(22);
          break;
        case 2:
          if (right_button_state == 1) {
            selected_parameter = 3;
          }
          draw_rectangle(33);
          break;
        case 3:
          if (right_button_state == 1) {
            selected_parameter = 4;
          }
          draw_rectangle(44);
          break;
      }
      if (right_button_state == 1) {
        right_button_state = 0;
        selected_line_on_screen = 0;
      }
      u8g.drawStr(10, 11, "parametre");
      switch (selected_line_on_screen) {
        case 0:
        case 1:
        case 2:
        case 3:
          u8g.drawStr( 5, 22, "les sorties");
          u8g.drawStr( 5, 33, "boutons");
          u8g.drawStr( 5, 44, "heure");
          u8g.drawStr( 5, 55, "Autre");
          break;
      }
      break;
  }
}

void draw_rectangle(int line) {
  int x = 0;
  int y = line;
  int length = 4;
  int height = 11; 
  u8g.drawBox(x, y, length, height);
}

void select_output_parameter_screen() {
  switch (selected_ev_on_screen) {
    case 0:
      if (left_button_state == 1) {
        left_button_state = 0;
        selected_parameter = 0;
        selected_line_on_screen = 0;
      }
      change_selected_line(7);
      u8g.drawStr( 5, 22, "sortie");
      u8g.drawStr( 5, 33, "sortie");
      u8g.drawStr( 5, 44, "sortie");  
      u8g.drawStr( 5, 56, "sortie");
      u8g.drawStr( 10, 11, "les sorties");
      switch (selected_line_on_screen) {
        case 0:
        case 1:
        case 2:
        case 3:          
          u8g.drawStr( 50, 22, "1");
          u8g.drawStr( 50, 33, "2");
          u8g.drawStr( 50, 44, "3");
          u8g.drawStr( 50, 56, "4");
          break;
        case 4:
        case 5:
        case 6:
        case 7:
          u8g.drawStr( 50, 22, "5");
          u8g.drawStr( 50, 33, "6");
          u8g.drawStr( 50, 44, "7");
          u8g.drawStr( 50, 56, "8");
          break;
      }
      switch (selected_line_on_screen) {
        case 0:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 1;
          }
          draw_rectangle(11);
          break;
        case 1:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 2;
          }
          draw_rectangle(22);
          break;
        case 2:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 3;
          }
          draw_rectangle(33);
          break;
        case 3:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 4;
          }
          draw_rectangle(44);
          break;
        case 4:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 5;
          }
          draw_rectangle(11);
          //u8g.drawBox(0, 11, 4, 11);
          break;
        case 5:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 6;
          }
          draw_rectangle(22);
          break;
        case 7:
          if (right_button_state == 1) {
            right_button_state = 0;
            selected_line_on_screen = 0;
            selected_ev_on_screen = 7;
          }
          draw_rectangle(33);
          break;
        case 6:
          if (right_button_state == 1) {
            right_button_state = 0;
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

void minus_button(int mem_adress, int max) {
  int time_on = 0;
  if (selected_ev != 0) {
    time_on = 7 * selected_ev;
    selected_ev=0;
  }
 
  switch (other_button_state) {
    case 1://170
      time_on = EEPROM.read(170);
      break;
    case 2:
      time_on = EEPROM.read(171);
      break;
    case 3:
      time_on = EEPROM.read(172);
      break;
    case 4:
      time_on = EEPROM.read(173);
      break;
  }
  if (other_button_state!=0){
    other_button_state=0;
  }
  if (time_on >= 1) {
    int mem_value;
    mem_value = EEPROM.read(mem_adress);
    mem_value = mem_value + time_on;
    if (mem_value >= max) {
      mem_value = mem_value - max;
    }
    write_eeprom(mem_adress, mem_value);
  }
}

void output_parameter_screen(int ev) {
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_ev_on_screen = 0;
    selected_line_on_screen = 0;
  }
  change_selected_line(3);
  int mem_address;
  int mem_value;
  switch (selected_line_on_screen) {
    case 0:
      draw_rectangle(11);
      mem_address = 17 + (10 * ev);
      change_value_using_button(mem_address, 1);
      break;
    case 1:
      draw_rectangle(22);
      mem_address = 10 + (10 * ev);
      minus_button(mem_address, 255);
      change_value_using_button(mem_address, 255);
      break;
    case 2:
      draw_rectangle(33);
      mem_address = 11 + (10 * ev);
      change_value_using_button(mem_address, 15);
      break;
    case 3:
      draw_rectangle(44);
      mem_address = 13 + (10 * ev);
      change_value_using_button(mem_address, 23);
      break;
  }

  print_on_screen(90, 11, ev);
  u8g.drawStr( 10, 11, "sortie ");
  switch (selected_line_on_screen) {
    case 0:
    case 1:
    case 2:
    case 3:
      // print auto mode state
      mem_address = 17 + (10 * ev);
      activate_auto_mode_screen(mem_address, 90, 22);
      // print auto time on
      mem_address = 10 + (10 * ev);      
      mem_value = EEPROM.read(mem_address);
      mem_value = mem_value*2;
      print_on_screen(70, 33, mem_value);
      // print auto frequency
      mem_address = 11 + (10 * ev);
      print_mem_value(70, 44, mem_address);
      // print auto start hour
      mem_address = 13 + (10 * ev);
      print_mem_value(70, 55, mem_address);
      u8g.drawStr( 5, 22, "Etat:");
      u8g.drawStr( 5, 33, "Temps on :");
      u8g.drawStr( 5, 44, "Tous les");
      u8g.drawStr( 110, 44, "j");
      u8g.drawStr( 5, 56, "Heure :");
      break;
  }
}

void buttons_parameter_screen() {
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }

  change_selected_line(3);
  switch (selected_line_on_screen) {
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

void clock_parameter_screen() {
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }
  change_selected_line(5);
  switch (selected_line_on_screen) {
    case 0: // day
      if (plus_button_state == 1) {
        plus_button_state = 0;
        int day;
        day = rtc[4];
        day = day + 1;
        if (day >= 31) {
          day = 0;
        }
        RTC.stop();
        RTC.set(DS1307_DATE, day);
        RTC.start();
      }
      u8g.drawBox(7, 22, 4, 11);
      break;
    case 1: // month
      if (plus_button_state == 1) {
        plus_button_state = 0;
        int month;
        month = rtc[5];
        month = month + 1;
        if (month >= 12) {
          month = 0;
        }
        RTC.stop();
        RTC.set(DS1307_MTH, month);
        RTC.start();
      }
      u8g.drawBox(28, 22, 4, 11);
      break;
    case 2: //annee
      if (plus_button_state == 1) {
        plus_button_state = 0;
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
      if (plus_button_state == 1) {
        plus_button_state = 0;
        int hour;
        hour = rtc[2];
        hour++;
        if (hour >= 24) {
          hour = 0;
        }

        RTC.stop();
        RTC.set(DS1307_HR, hour);
        RTC.start();
      }

      u8g.drawBox(7, 44, 4, 11);
      break;
    case 4: //minute
      if (plus_button_state == 1) {
        plus_button_state = 0;
        int minute;
        minute = rtc[1];
        minute++;
        if (minute >= 60) {
          minute = 0;
        }
        RTC.stop();
        RTC.set(DS1307_MIN, minute);
        RTC.start();
      }

      u8g.drawBox(25, 44, 4, 11);
      break;
    case 5: //seconde
      if (plus_button_state == 1) {
        plus_button_state = 0;
        int seconde;
        seconde = 0;
        RTC.stop();
        RTC.set(DS1307_SEC, seconde);
        RTC.start();
      }
      u8g.drawBox(45, 44, 4, 11);
      break;
  }

  u8g.drawStr( 10, 11, " Heure ");
  print_on_screen(7, 22, rtc[4]);
  print_on_screen(28, 22, rtc[5]);
  print_on_screen(48, 22, rtc[6]);
  u8g.drawStr( 20, 22, "/");
  u8g.drawStr( 40, 22, "/");
  u8g.drawStr( 20, 44, ":");
  u8g.drawStr( 40, 44, ":");
  print_on_screen(7, 44, rtc[2]);
  print_on_screen(25, 44, rtc[1]);
  print_on_screen(45, 44, rtc[0]);
}

void other_parameter_screen() {
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_parameter = 0;
    selected_line_on_screen = 0;
  }

  change_selected_line(1);
  switch (selected_line_on_screen) {
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
  switch (selected_line_on_screen) {
    case 0:
    case 1:
    case 2:
    case 3: 
      u8g.drawStr( 5, 22, "Auto ete/hiver :");
      activate_auto_mode_screen(160, 90, 22);
      u8g.drawStr( 5, 33, "Temp ete/hiver :");
      print_mem_value(70, 33, 161);
      u8g.drawStr( 5, 44, "Duree ete :");
      print_mem_value(70, 44, 162);
      u8g.drawStr( 5, 55, "Freq ete :");
      print_mem_value(70, 55, 163);
      break;
    case 4:
    case 5:
      u8g.drawStr( 5, 22, "duree hiver :");
      print_mem_value(70, 22, 164);
      u8g.drawStr( 5, 33, "Freq hivers :");
      print_mem_value(70, 33, 165);
      break;
  }
  
  u8g.drawStr( 10, 11, "Autre");
  u8g.drawStr( 5, 22, "Auto ete/hiver :");
  u8g.drawStr( 5, 33, "temp ete/hiver :");
  u8g.drawStr( 5, 44, "duree ete :");
  u8g.drawStr( 5, 55, "Freq ete :");
  u8g.drawStr( 5, 11, "duree hivers :");
  u8g.drawStr( 5, 55, "Freq hivers :");
}

void update_all_ev_state() {
  set_output_state(addr_mod_relay, 1, ev_remaining_time_on[0]);
  set_output_state(addr_mod_relay, 2, ev_remaining_time_on[1]);
  set_output_state(addr_mod_relay, 3, ev_remaining_time_on[2]);
  set_output_state(addr_mod_relay, 4, ev_remaining_time_on[3]);
  set_output_state(addr_mod_relay, 5, ev_remaining_time_on[4]);
  set_output_state(addr_mod_relay, 6, ev_remaining_time_on[5]);
  set_output_state(addr_mod_relay, 7, ev_remaining_time_on[6]);
  set_output_state(addr_mod_relay, 8, ev_remaining_time_on[7]);
}

void set_output_state(int adr, int ev, int val) {
  int mem_adress;
  mem_adress = 17 + (10 * ev);
  if (EEPROM.read(mem_adress) == 0) {
    val = 0;
  }

  if (val == 0) {
    send_to_module(adr, ev);
    send_to_module(adr, 100);
  } else {
    send_to_module(adr, ev);
    send_to_module(adr, 101);
  }
}

void update_auto_mode_with_ath21(){
  int mem_address;
  int mem_address2;
  int mem_value;
  int ev;
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
  
  for (int i = 0; i < 8; i++) {
    ev = i + 1;
    mem_address = 10 + (10 * ev);
    mem_address2 = 11 + (10 * ev);
    if (temp_value <= temp_change_season){
      mem_value = EEPROM.read(162);
      write_eeprom(mem_address, mem_value);
      mem_value = EEPROM.read(163);
      write_eeprom(mem_address2, mem_value);
    } else {
      mem_value = EEPROM.read(164);
      write_eeprom(mem_address, mem_value);
      mem_value = EEPROM.read(165);
      write_eeprom(mem_address2, mem_value);
    }
  }  
}

void loop_actualization() {
  if (rtc_min != rtc[1]) {
    // check remainining time for ev on
    for (int i = 0; i < 8; i++) {
      if (ev_remaining_time_on[i] != 0) {
        ev_remaining_time_on[i]--;      
    
        if (ev_remaining_time_on[i] <= 0) {
          ev_remaining_time_on[i] = 0;
        }
      }
    }
    rtc_min = rtc[1];
  }

  if (manual_mode_state == 0) {
    horlact();
  }

  // Check Temperature and humidity at 12 o'clock if auto mode on with aht21
  int mem_value;
  mem_value = EEPROM.read(160);
  if (mem_value == 1) {
    if (rtc_day != rtc[4]) {    
      if (rtc[2] == 12) {
        update_auto_mode_with_ath21();
      }
    }
  }
}

void horlact() {
  for (int i = 0; i < 8; i++) {
    int mem_address;  
    int mem_value;
    int ev;
    ev = i + 1;
    mem_address = ev * 10;
    mem_address = 12 + mem_address;
    
    if(EEPROM.read(mem_address)==1){
      mem_address = ev * 10;
      mem_address = 17 + mem_address;

      mem_value = EEPROM.read(mem_address);
      if (mem_value == 1) {
        mem_address = ev * 10;
        mem_address = 13 + mem_address;

        mem_value = EEPROM.read(mem_address);

        if (rtc[2] == mem_value) {
          if (ev_remaining_time_on[i] == 0) {
            if (next_day_on[i] == 0) {
              next_day_on[i] = rtc[4];
            }
            if (next_day_on[i] == rtc[4]) {
              mem_address = 10 + (ev * 10);
              mem_value = EEPROM.read(mem_address);
              mem_value = mem_value*2;
              //mise en route
              if (mem_value <= 0) {
                mem_value = 0;
              } if (mem_value >= max_time_on_ev) {
                mem_value = max_time_on_ev;
              }
              ev_remaining_time_on[i] = mem_value;
              calculate_next_day(i);
            } else { //
              if (next_day_on[i] == 0) {
                calculate_next_day(i);
              }
            }
          }
        }
      } else {
        ev_remaining_time_on[i] = 0;
        if (next_day_on[i] == rtc[4]) {
          next_day_on[i] = 0;
        }
      }
    }
  }
}

void calculate_next_day(int i) {
  int mem_address;
  int day_to_add;
  int day_in_month;
  int day;
  if (plus_button_state == 0) {
    mem_address = 11 + ((i + 1) * 10);
    day_to_add = EEPROM.read(mem_address);
  } else {
    day_to_add = plus_button_state;
  }
  day = rtc[4] + day_to_add;
  switch (rtc[5]) {
    case 1: //31
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      day_in_month = 31;
      break;
    case 2: //28
      day_in_month = 28;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      day_in_month = 30;
      break;
  }
  if (day > day_in_month) {
    day = day - day_in_month;
  }
  next_day_on[i] = day;
}

void manual_activation_screen() {
  int mem_value;
  if (selected_ev != 0) {
    manual_selected_ev = selected_ev;
    selected_ev = 0;
  }
  if (left_button_state == 1) {
    left_button_state = 0;
    selected_screen = 0;
    main_screen_selected=0;
  }
  switch (other_button_state) {
    case 1:
      other_button_state = 0;
      mem_value = EEPROM.read(170);
      manual_time_on = manual_time_on + mem_value;
      break;
    case 2:
      other_button_state = 0;
      mem_value = EEPROM.read(171);
      manual_time_on = manual_time_on + mem_value;
      break;
    case 3:
      other_button_state = 0;
      mem_value = EEPROM.read(172);
      manual_time_on = manual_time_on + mem_value;
      break;
    case 4:
      other_button_state = 0;
      mem_value = EEPROM.read(173);
      manual_time_on = manual_time_on + mem_value;
      break;
  }
  if (manual_time_on >= max_time_on_ev) {
    manual_time_on = manual_time_on - max_time_on_ev;
  }
  sprintf (buf, "%d", manual_selected_ev);
  u8g.drawStr( 70, 22, buf);
  sprintf (buf, "%d",  manual_time_on);
  u8g.drawStr( 60, 33, buf);
  u8g.drawStr( 10, 11, " manuel ");
  u8g.drawStr( 5, 22, " sortie n :");
  u8g.drawStr( 5, 33, " temp :");

  if (plus_button_state == 1) {
    plus_button_state = 0;
    ev_remaining_time_on[manual_selected_ev - 1] = manual_time_on;

    manual_time_on = 0;
    manual_selected_ev = 1;
    main_screen_selected = 0;
    selected_screen = 0;
  }
}

void write_eeprom(int mem_adress, int val) {
  int mem_value;
  mem_value = EEPROM.read(mem_adress);
  if (mem_value != val) {
    EEPROM.write(mem_adress, val);
  }
}

void delay_screen() {
  if (selected_ev != 0) {
    manual_selected_ev = selected_ev;
    selected_ev = 0;
  }

  sprintf (buf, "%d", manual_selected_ev);
  u8g.drawStr( 70, 22, buf);
  u8g.drawStr( 10, 11, " arret ");
  u8g.drawStr( 5, 22, " sortie n :");

  if (plus_button_state == 1) {
    plus_button_state = 0;
    ev_remaining_time_on[manual_selected_ev - 1] = 0;

    manual_selected_ev = 1;
    main_screen_selected = 0;
    selected_screen = 0;
  }
}

void print_mem_value(int col, int line, int mem_address) {
  int mem_value;
  mem_value = EEPROM.read(mem_address);
  sprintf (buf, "%d", mem_value);
  u8g.drawStr( col, line, buf);
}

void print_on_screen(int col, int line, int num) {
  sprintf (buf, "%d", num);
  u8g.drawStr( col, line, buf);
}

void change_selected_line(int num) {
  if (up_button_state == 1) {
    up_button_state = 0;
    if (selected_line_on_screen == 0) {
      selected_line_on_screen = num;
    } else {
      selected_line_on_screen--;
    }
  }
  if (down_button_state == 1) {
    down_button_state = 0;
    if (selected_line_on_screen == num) {
      selected_line_on_screen = 0;
    } else {
      selected_line_on_screen++;
    }
  }
}

void change_value_using_button(int mem_address, int max_value) {
  int mem_value;
  if (plus_button_state == 1) {
    plus_button_state = 0;
    mem_value = EEPROM.read(mem_address);
    mem_value++;
    if (mem_value > max_value) {
      mem_value = 0;
    }
    if (mem_value <= max_value) {
      write_eeprom(mem_address, mem_value);
    }
  }
  if (minus_button_state == 1) {
    minus_button_state = 0;
    mem_value = EEPROM.read(mem_address);
    if (mem_value <= 0) {
      mem_value = max_value;
    } else {
      mem_value--;
    }
    if (mem_value >= 0) {
      write_eeprom(mem_address, mem_value);
    }
  }
}

void activate_auto_mode_screen(int v, int col, int li) {
  switch (v) {
    case 0:
      u8g.drawStr( col, li, "desactiver");
      break;
    case 1:
      u8g.drawStr( col, li, "activer");
      break;
  }
}