# Variable Intervalmeter for long exposure HDR photography
variable intervalmeter for Pentax/Canon cameras

This firmware is for an arduino based Interval meter mainly focused on Pentax DSLR cameras for creating a set of
bracketed files that are one EV apart for use in creating HDR photographs.

It should theorically work with Canon cameras that have the some wired remote connector with long exposure noise
reduction turned off at this time.  I do not have a Canon to determine the relationship between their long exposures
and the dark frames.

# Parts Needed

1 Arduino Uno or compatible

1 2.8" TFT display shield ( https://www.adafruit.com/products/1651 or compatible )

1 prototype shield

2 5v SPST reed relays

other misc parts that I will list here after I look at the prototype I have been using



# Using the Interval Meter

Go to the settings screen and select the duration of the initial exposure and the number of exposures you wish
in the bracketing sequence.  Enable or disable dark frame mode as needed (make sure you go into your camera
settings and adjust the long exposure noise reduction to match)

The USE button will return you to the main screen without saving the current settings to the eeprom.

The SAVE button will return you to the main screen after saving the current settings to the eeprom.

The RECALL button recalls the settings saved in the eeprom and remains on the settings screen.
