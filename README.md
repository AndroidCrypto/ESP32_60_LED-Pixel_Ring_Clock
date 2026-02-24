# ESP32 60 LED-Pixel Ring Clock
In this tutorial you will learn how to build an LED Ring clock with the WS2812 LED strip and an ESP32. 

The tutorial "**LED Ring Clock with WS2812**" was written by Stefan Maetschke and is published here: https://www.makerguides.com/led-ring-clock/.

It is using a regular ESP32 with an 60 LED ring with WS2812 LEDs.

You can see hour marks (white dots) and the seconds running (moving white dot). The current hour is marked by the orange dot and the minutes by the yellow dots.

The original project was using an api for getting the local timezone by the IP address: http://worldtimeapi.org/api/ip. Unfortunately, this service is no longer available.

As a replacement, I'm using the ESP32 built-in time class that offers a local time adjustment based on a timezone string. For Germany/Europe it is:
  
#define MY_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

Please change the timezone string to your local place, see a complete list here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv.

As the **project was developed by Stefan Maetschke**, all credits go to him. I just made the changes to get a working clock.
  
(2026) AndroidCrypto (Michael), https://medium.com/@androidcrypto

I'm using the most recent version of Arduino (2.3.7) and esp32-Boards by Espressif Systems (3.3.7).

