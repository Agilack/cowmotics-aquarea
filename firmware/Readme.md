Cowmotics-Aquarea firmware
==========================

This folder contains a firmware for the ESP32 processor. For the moment, we
have focus on communication with Aquarea using serial port (UART). At this
point wireless communication is not available/supported but will be added in
the future.

This software has been developped using Espressif IDF version 4.3.2. To
build the firmware, you can just type `idf.py build` into a shell where
esp-idf environment is properbly defined (see df documentation).
As the cowmotics board is not already available, tests of this software are
made using another ESP32 board.
