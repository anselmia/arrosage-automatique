#include <Wire.h> //i2c
#include "Arduino.h"
#include "myeeprom.h"
#include <U8glib.h> // LCD screen
#include <avr/wdt.h>
#include "button.h"
#include "ev.h"
#include "menu.h"
#include <AHT20.h> // AHT21

// Program version
const byte versio = 1;

// Read / Write eeprom
MYEEPROM eeprom = MYEEPROM();

/* EV pins*/
const byte pin_ev1 = 0;
const byte pin_ev2 = 1;
const byte pin_ev3 = 2;
const byte pin_ev4 = 3;
const byte pin_ev5 = 4;
const byte pin_ev6 = 5;

/* Error led pins*/
const byte pin_led = 6;

/* Pin alim scrren */
const byte pin_screen = 7;

// EV
EV arrayOfEV[6] = {EV(pin_ev1, 1), EV(pin_ev2, 2), EV(pin_ev3, 3), EV(pin_ev4, 4), EV(pin_ev5, 5), EV(pin_ev6, 6)};

// SCREEN screen;

// bouton
bool button_state = false;
const byte pin_button_line1 = 0;
const byte pin_button_line2 = 1;
const byte pin_button_line3 = 3;

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
0 : s1
1 : s2
2 : s3
3 : s4
4 : s5
5 : s6
*/
BUTTON arrayofButton[3] = {BUTTON(pin_button_line1), BUTTON(pin_button_line2), BUTTON(pin_button_line3)};

// Menu
MENU menu = MENU();

// error
/* 0 : temperature
   1 : Clock */
bool error[2];

// manual all time counter
int manual_couter = 0;

// inactive time
int inactive = 0;
bool screen_state = true;

// Text buffer
char buf[20];

// Screen
U8GLIB_ST7920_128X64_4X u8g(13, 11, 10);

// Aht21 sensor
AHT20 aht20;

// init millis
unsigned long init_millis;
unsigned long init_duration = 10000;

void setup()
{
  init_millis = millis();
  init_duration += init_millis;
  // Serial.begin(9600);
  wdt_enable(WDTO_4S); // watchdog 4 seconde reinitialisation
  Wire.begin();
  Wire.setClock(31000L); // reglage de horloge de i2c
  Serial.println(F("init wire"));
  Init();
  delay(3000);
}

void Init()
{
  u8g.setColorIndex(1);
  u8g.setCursorFont(u8g_font_cursor);
  u8g.setCursorStyle(144);
  Serial.println(F("init screen"));

  // Init com with ds1307
  inii2c(0x68, 1);
  //  initialisation de ds1307
  menu.initClock(error);
  Serial.println(F("clock ok"));
  Wire.begin(); // Join I2C bus
  if (aht20.begin() == false)
  {
    do
    {
      u8g.setFont(u8g_font_tpss);
      u8g.drawStr(5, 44, "AHT21 Not found");
      Serial.println(F("AHT21 Not found"));
    } while (u8g.nextPage());
    while (1)
      ;
  }

  Serial.println(F("AHT21 ok"));

  // Init EV
  for (byte i = 0; i < 6; i++)
    arrayOfEV[i].init();

  // set output for error led and screen
  pinMode(pin_led, OUTPUT);
  pinMode(pin_screen, OUTPUT);

  // SWitch On the screen
  digitalWrite(pin_screen, LOW);
  delay(200);

  // Init memory
  // eeprom.init_memory();

  u8g.firstPage(); // SÃ©lectionne la 1er page mÃ©moire de l'Ã©cran
  do
  {
    u8g.setFont(u8g_font_tpss);            // Utilise la police de caractÃ¨re standard
    u8g.drawStr(30, 11, "Initialisation"); // 12 line
    if (error[1] == true)
      u8g.drawStr(10, 22, "erreur horloge");

    u8g.drawStr(5, 44, "version");
    print_on_screen(50, 44, versio);
  } while (u8g.nextPage());
}

void inii2c(byte adr, byte i)
{
  Wire.beginTransmission(adr);
  error[i] = (bool)Wire.endTransmission(true);
}

void select_button(byte selected_button)
{
  switch (arrayofButton[0].getSelection())
  {
  case 0: // right
    arrayofButton[0].type = 0;
    menu.forward();
    // Serial.println("right");
    break;
  case 1: // up
    arrayofButton[0].type = 1;
    menu.up();
    // Serial.println("up");
    break;
  case 2: // left
    arrayofButton[0].type = 2;
    menu.backward();
    // Serial.println("left");
    break;
  case 3: // down
    arrayofButton[0].type = 3;
    menu.down();
    // Serial.println("down");
    break;
  }

  switch (arrayofButton[1].getSelection())
  {
  case 0: // +
    arrayofButton[1].type = 0;
    menu.updateValue(eeprom, 1);
    // Serial.println("+");
    break;
  case 1: // -
    arrayofButton[1].type = 1;
    menu.updateValue(eeprom, 0);
    // Serial.println("-");
    break;
  }

  switch (arrayofButton[2].getSelection())
  {
  case 0: // s1
    arrayofButton[2].type = 0;
    menu.selectEV(1);
    // Serial.println("ev 1");
    break;
  case 1: // s12
    arrayofButton[2].type = 1;
    menu.selectEV(2);
    // Serial.println("ev 2");
    break;
  case 2: // s3
    arrayofButton[2].type = 2;
    menu.selectEV(3);
    // Serial.println("ev 3");
    break;
  case 3: // s4
    arrayofButton[2].type = 3;
    menu.selectEV(4);
    // Serial.println("ev 4");
    break;
  case 4: // s5
    arrayofButton[2].type = 4;
    menu.selectEV(5);
    // Serial.println("ev 5");
    break;
  case 5: // s6
    arrayofButton[2].type = 5;
    menu.selectEV(6);
    // Serial.println("ev 6");
    break;
  }
}

void read_button()
{
  for (byte i = 0; i < 3; i++)
  {
    arrayofButton[i].readEvent();
    switch (arrayofButton[i].getEvent())
    {
    case EVENT_PRESSED:
      if (button_state == false)
      {
        select_button(arrayofButton[i].getSelection());
        button_state = true;
      }
      break;
    case EVENT_RELEASED:
      button_state = false;
      break;
    }
  }
}

void loop()
{
  wdt_reset();            // reset watchdog
  menu.getClock(error);   // Check inactive screen
  check_inactiveScreen(); // Update the clock
  read_button();          // Read the buttons

  u8g.firstPage(); // Select the first memory page of the scrren
  do
  {
    print_screen();
  } while (u8g.nextPage()); // Select the next page

  ev_actualization(); // Update ev state
  reset_button();
  check_error();
}

void check_inactiveScreen()
{
  int sec = eeprom.Read(mem_sec);
  if (button_state == false && sec != menu.rtc_sec)
    inactive++;
  else if (button_state == true)
    inactive = 0;

  if (button_state == true && screen_state == false)
  {
    screen_state = true;
    inactive = 0;
    digitalWrite(pin_screen, LOW);
    u8g.sleepOff();
  }

  if (inactive > 30)
  {
    digitalWrite(pin_screen, HIGH);
    u8g.sleepOn();
    screen_state = false;
  }
}

void reset_button()
{
  arrayofButton[0].type = -1;
  arrayofButton[1].type = -1;
  arrayofButton[2].type = -1;
}

void check_error()
{
  if (error[0] == true || error[1] == true)
  {
    digitalWrite(pin_led, HIGH);
  }
  else
    digitalWrite(pin_led, LOW);
}

void print_screen()
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
  case 5:
    meteo_screen();
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

void main_screen()
{
  u8g.disableCursor();
  sprintf(buf, "%02d/%02d/%04d", menu.rtc_day, menu.rtc_month, menu.rtc_year);
  u8g.drawStr(34, 11, buf);

  sprintf(buf, "%02d:%02d:%02d", menu.rtc_hour, menu.rtc_min, menu.rtc_sec);
  u8g.drawStr(44, 21, buf);

  u8g.drawStr(15, 33, "1 : ");
  print_on_screen(30, 33, arrayOfEV[0].remainingTimeOn / 60);
  u8g.drawStr(44, 33, ",");
  print_on_screen(50, 33, arrayOfEV[0].nextDayOn);
  u8g.drawStr(15, 44, "2 : ");
  print_on_screen(30, 44, arrayOfEV[1].remainingTimeOn / 60);
  u8g.drawStr(44, 44, ",");
  print_on_screen(50, 44, arrayOfEV[1].nextDayOn);
  u8g.drawStr(15, 55, "3 : ");
  print_on_screen(30, 55, arrayOfEV[2].remainingTimeOn / 60);
  u8g.drawStr(44, 55, ",");
  print_on_screen(50, 55, arrayOfEV[2].nextDayOn);
  u8g.drawStr(70, 33, "4 : ");
  print_on_screen(85, 33, arrayOfEV[3].remainingTimeOn / 60);
  u8g.drawStr(100, 33, ",");
  print_on_screen(106, 33, arrayOfEV[3].nextDayOn);
  u8g.drawStr(70, 44, "5 : ");
  print_on_screen(85, 44, arrayOfEV[4].remainingTimeOn / 60);
  u8g.drawStr(100, 44, ",");
  print_on_screen(106, 44, arrayOfEV[4].nextDayOn);
  u8g.drawStr(70, 55, "6 : ");
  print_on_screen(85, 55, arrayOfEV[5].remainingTimeOn / 60);
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
  u8g.drawStr(15, 55, "Meteo");
}

void auto_mode_screen()
{
  u8g.drawStr(15, 11, "Sortie :");
  print_on_screen(70, 11, menu.selectedEV);
  // print auto mode state
  u8g.drawStr(15, 22, "Etat :");
  byte mem_value = eeprom.Read(mem_autostate + (menu.selectedEV * 10));
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
  sprintf(buf, "%02d:%02d", eeprom.Read(mem_autoStartHour + (menu.selectedEV * 10)), eeprom.Read(mem_autoStartMin + (menu.selectedEV * 10)));
  u8g.drawStr(70, 55, buf);
}

void clock_parameter_screen()
{
  sprintf(buf, "Date : %02d/%02d/%04d", menu.rtc_day, menu.rtc_month, menu.rtc_year);
  u8g.drawStr(15, 22, buf);

  sprintf(buf, "Heure : %02d:%02d:%02d", menu.rtc_hour, menu.rtc_min, menu.rtc_sec);
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
    byte mem_value = eeprom.Read(mem_autoSeason);
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

void meteo_screen()
{
  u8g.drawStr(25, 11, " Meteo ");
  u8g.drawStr(15, 22, "Temperature : ");
  byte temperature = read_temperature();
  print_on_screen(70, 22, temperature);
  u8g.drawStr(15, 33, "Humidity : ");
  byte humidity = read_humidity();
  print_on_screen(70, 22, humidity);
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
  byte mem_value = eeprom.Read(mem_state + (menu.selectedEV * 10));
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

void draw_cursor()
{
  u8g.enableCursor();
  if (menu.actualLine > 5)
    u8g.setCursorPos(12, ((menu.actualLine - 5) * 11) + 5);
  else
    u8g.setCursorPos(12, (menu.actualLine * 11) + 5);
}

byte read_temperature()
{
  float temperature;
  if (aht20.available() == true)
  {
    temperature = aht20.getTemperature(); // Get the temperature
    eeprom.write(mem_dayTemp, temperature);
    error[0] = false;
  }
  else
  {
    temperature = eeprom.Read(mem_dayTemp);
    error[0] = true;
  }

  return (byte)temperature;
}

byte read_humidity()
{
  float humidity;
  if (aht20.available() == true)
  {
    humidity = aht20.getHumidity(); // Get the humidity
    eeprom.write(mem_dayHumidity, humidity);
    error[0] = false;
  }
  else
  {
    humidity = eeprom.Read(mem_dayHumidity);
    error[0] = true;
  }
  return (byte)humidity;
}

void update_auto_mode_with_ath21()
{
  byte temp_value = read_temperature();

  byte timeon;
  byte freq;

  byte temp_change_season;
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

  for (byte i = 0; i < 6; i++)
    arrayOfEV[i].updateSeason(eeprom, timeon, freq);
}

// Update all EV state based on clock
void ev_actualization()
{
  int sec = eeprom.Read(mem_sec);
  int day = eeprom.Read(mem_day);
  // EV delayed
  if (menu.delay == true)
  {
    arrayOfEV[menu.selectedEV - 1].nextDayOn += menu.screenValue;
    menu.delay = false;
  }

  // EV started manually
  if (menu.manual == true)
  {
    arrayOfEV[menu.selectedEV - 1].remainingTimeOn = menu.screenValue * 60;
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
    // if EV desactivated, don't start it
    if (eeprom.Read(mem_state + (10 * menu.manual_all)) == 0)
      menu.manual_all++;

    // if counter is 0, start the EV
    if (manual_couter == 0)
      arrayOfEV[menu.manual_all - 1].remainingTimeOn = 300;

    // actualization of remaining time every seconde

    if (sec != menu.rtc_sec)
    {
      manual_couter++;
    }

    // if time is over, go to the next EV
    if (manual_couter > 300)
    {
      manual_couter = 0;
      menu.manual_all++;
    }

    // if all EV has been started, reset the mode
    if (menu.manual_all > 5)
    {
      menu.manual_all = 0;
      manual_couter = 0;
    }
  }

  // Stop all EV
  if (menu.stop_all == true)
  {
    for (byte i = 0; i < 6; i++)
      arrayOfEV[i].remainingTimeOn = 0;

    menu.stop_all = false;

    // stop the manual all mode
    menu.manual_all = 0;
    manual_couter = 0;
  }

  // If EV has been desactivated, stop it
  for (byte i = 0; i < 6; i++)
  {
    if (eeprom.Read(mem_state + (10 * (i + 1))) == 0)
    {
      arrayOfEV[i].remainingTimeOn = 0;
      arrayOfEV[i].nextDayOn = 0;
    }
  }

  // Update EV state every minute
  if (sec != menu.rtc_sec)
  {
    // Set saved sec as actual sec
    eeprom.write(mem_sec, menu.rtc_sec);

    byte mem_value;
    // Check Temperature and humidity at 12 o'clock if auto mode on with aht21
    mem_value = eeprom.Read(mem_autoSeason);
    if (mem_value == 1)
    {
      if (day != menu.rtc_day)
      {
        if (menu.rtc_hour == 12)
        {
          update_auto_mode_with_ath21();
        }
        eeprom.write(mem_day, menu.rtc_day);
      }
    }

    // Calculate next day on and remaining time
    for (byte i = 0; i < 6; i++)
    {
      arrayOfEV[i].updateRemainingTime(eeprom, menu.rtc_hour, menu.rtc_min, menu.rtc_day, menu.rtc_month, menu.rtc_year);
    }
  }

  // Start actuating EV after init time
  if (millis() > init_duration)
  {
    for (byte i = 0; i < 6; i++)
      arrayOfEV[i].update_state();
  }
}

void print_mem_value(byte col, byte line, byte mem_address)
{
  byte mem_value;
  mem_value = eeprom.Read(mem_address);
  sprintf(buf, "%d", mem_value);
  u8g.drawStr(col, line, buf);
}

void print_on_screen(byte col, byte line, byte num)
{
  sprintf(buf, "%d", num);
  u8g.drawStr(col, line, buf);
}

void activate_screen(byte value, byte x, byte y)
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