# WSPR_HAB_tracker
High Altitude Balloon tracker for Arduino and Si5351, using WSPR protocol

This program will let you turn your Arduino, GPS and Si5351 module into a low power QRP WSPR beacon that can send telemetry data from a vehicle like a balloon to the WSPR database. Telemetry data will contain time, Maidenhead, altitude, temperature, voltage. 

What you need:

* Arduino (Uno, Nano, Pro Mini 8MHz or 16MHz, or compatible)
* GPS module (I recommend the ATGM336H but a NEO6, NEO7, or NEO8 will also do the job)
* Si5351 breakout board

You need to dowload these libraries:

https://github.com/etherkit/Si5351Arduino

https://github.com/mikalhart/TinyGPSPlus

https://github.com/etherkit/JTEncode

The first thing you should do is edit the settings in the settings.h file. Lots of instructions are in the settings.h file. Be sure to read that file.
You will need a HAM license to use this software.

Have fun!
