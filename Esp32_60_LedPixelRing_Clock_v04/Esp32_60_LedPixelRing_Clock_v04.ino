/*
  This is a sketch for the tutorial 'LED Ring Clock with WS2812', published
  on https://www.makerguides.com/led-ring-clock/.

  It is using a regular ESP32 with an 60 LED ring with WS2812 LEDs.

  You can see hour marks (white dots) and the seconds running (moving white dot).
  The current hour is marked by the orange dot and the minutes by the yellow dots.

  The original project was using an api for getting the local timezone by the IP address:
  http://worldtimeapi.org/api/ip. Unfortunately, this service is no longer available.

  As a replacement, I'm using the ESP32 built-in time class that offers a local time
  adjustment based on a timezone string. For Germany/Europe it is:
  #define MY_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

  Please change the timezone string to your local place, see a complete list here:
  https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv.

  As the project was developed by Stefan Maetschke, all credits go to him. I just made
  the changes to get a working clock.
  (2026) AndroidCrypto (Michael), https://medium.com/@androidcrypto
  https://github.com/AndroidCrypto/ESP32_60_LED-Pixel_Ring_Clock

  I'm using the most recent version of Arduino (2.3.7) and 
  esp32-Boards by Espressif Systems (3.3.7).
*/

// ----------------------------------------------------------------
// Sketch and Board information
const char* PROGRAM_VERSION = "ESP32 60 LED Pixel Ring Clock V04";
const char* PROGRAM_TITLE = "ESP32 LED Clock";

// ----------------------------------------------------------------
// WiFi / NTP

const char* WIFI_SSID = "change to your router";
const char* WIFI_PASSWORD = "change to your password";

// Time management
// Configuration of NTP
// choose the best fitting NTP server pool for your country
//#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_NTP_SERVER "pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define MY_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

#include <time.h>
#include <WiFi.h>

// Time Globals
time_t now;  // this are the seconds since Epoch (1970) - UTC
tm tm;       // the structure tm holds time information in a more convenient way *
uint8_t hh, mm, ss;
uint8_t dy, mt;  // day and month
uint16_t yr;     // year

unsigned long ms = millis();

void setupNtpTime() {
  // Obtain current time and set variables for the Second, Minute and Hour
  configTime(0, 0, MY_NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TIMEZONE, 1);     // Set environment variable with your time zone
  tzset();
}

void setupWiFi() {
  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void printCurrentTime() {
  time(&now);              // read the current time
  localtime_r(&now, &tm);  // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);  // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);  // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);  // hours since midnight 0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);  // minutes after the hour 0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);  // seconds after the minute 0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);  // days since Sunday 0-6
  if (tm.tm_isdst == 1)      // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

// ----------------------------------------------------------------
// LED
#include "Adafruit_NeoPixel.h"

# define PIXEL_BRIGHTNESS 10

#define DINPIN 13
#define NPIXELS 60
#define OFFSET 29

void printTime(int h, int m, int s) {
  Serial.printf("h: %2d m: %2d s: %2d\n", h, m, s);
}

Adafruit_NeoPixel pixels(NPIXELS, DINPIN, NEO_GRB + NEO_KHZ800);

int index(int i) {
  return (i + OFFSET) % NPIXELS;
}

void setPixel(int i, uint32_t color) {
  pixels.setPixelColor(index(i), color);
}

uint32_t getPixel(int i) {
  return pixels.getPixelColor(index(i));
}

void setTicks(int m) {
  uint32_t tickColor = pixels.Color(50, 50, 50);
  for (int h = 0; h < 12; h++) {
    setPixel(h * 5, tickColor);
  }
}

void setHours(int h) {
  uint32_t hourColor = pixels.Color(200, 50, 0);
  setPixel(h * 5, hourColor);  // h: 0..11 = 12 Hours
}

void setMinutes(int m) {
  uint32_t minColor = pixels.Color(100, 100, 0);
  for (int i = 0; i <= m; i++) {
    setPixel(i, minColor);
  }
}

void setSeconds(int s) {
  uint32_t color = getPixel(s);
  setPixel(s, color + 0x373737);
}

void showTime(int h, int m, int s) {
  pixels.clear();
  setMinutes(m);
  setTicks(m);
  setHours(h);
  setSeconds(s);
  pixels.show();
  //printTime(h, m, s);
}

void updateDisplay() {
// Only update the screen once every second and at the start
  if (millis() - ms >= 1000) {
  hh = tm.tm_hour;
    // 12 hours clock
    if (hh > 11) hh = hh - 12;
    
    mm = tm.tm_min;
    // current measure mode, gives acurrent of 100 mA with brightness of 10
    // during WiFi update up to 170 mA
    // for brightness 30, the current increases to 150 mA / 217 mA
    // for brightness 5, the current decreases to 89 / 159 mA, but the LEDs are 
    // no longer clear to differentate, 10 is a good choice
    //mm = 59;

    ss = tm.tm_sec;
    showTime(hh, mm, ss);
    printCurrentTime();
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(PROGRAM_VERSION);

  setupNtpTime();
  setupWiFi();

  pixels.begin();
  pixels.clear();
  pixels.setBrightness(PIXEL_BRIGHTNESS);
  pixels.show();
}

void loop() {
  updateDisplay();
  delay(100);
}
