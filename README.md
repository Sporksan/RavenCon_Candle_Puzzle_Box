# RavenCon_Candle_Puzzle_Box
A color theory escape room prop with RC255 RFID readers via the MFRC522 Library.

This Project uses an Arduino Nano and 2 RC522 RFID readers in conjunction with 3 LED lights mounted as fake candlesticks.
Players need to place each pair of primary colors on readers to light corosponding candle, Red + Blue = purple, Yellow + Blue = Green, Red + Yellow = orange.
Correctly lighting all three candles simultanisouly will trigger an electromagnet to release allowning the box to open.

Code orignaly created by Playful Technologies (https://www.youtube.com/watch?v=f_f_5cL0Pd0&t=2s)

Updated 9/12/25 for RavenCon Escape Room. (https://www.ravencon.com)

To Do:
  * Update color tests to allow for more than one set of items/make tags one time use.
  * Fix SerialOut messages to give more details for futue use and debbuging.
  * Update design to allow for 12V maglock.
  * Impliment puzzle reset command (Button or RFID Tag)
  * Add timer to turn off candles after set period of time.

Components:
  * Arduino Nano (https://wiki.seeedstudio.com/Seeeduino-Nano/)
  * 2 x RC255 RFID Readers (https://circuitdigest.com/microcontroller-projects/interfacing-rfid-reader-module-with-arduino)
  * 3V Electromagent component (https://wiki.keyestudio.com/KS0320_Keyestudio_Electromagnet_Module_(Black_and_Environmental-friendly))
  * 3 x LED Flash Candle Flame DC3V
  * Box (Homemade with divider to hide guts)
  * Candelabra (Cheep plastic candlebra from Dollar Store, removed pre-wired LEDS and replaced)
  * 3 NFC Tags Nfc215 stickers. (Or Fobs or Cards)
