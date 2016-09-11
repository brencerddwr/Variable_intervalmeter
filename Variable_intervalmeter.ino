// start of includes for libraries
#include <stdint.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SeeedTouchScreen.h>
#include <CountUpDownTimer.h>
// end of includes for libraries

// start of includes for defines and global variables
#include "RGBColors.h"
#include "defines.h"
#include "variables.h"
// end of includes for defines and global variables

// start of display setup
Adafruit_ILI9341 Tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// end of display setup

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM);

// end of setup for touchscreen

// setup of counter objects
CountUpDownTimer Exposure(DOWN);
CountUpDownTimer Dark_Frame(DOWN);

// start of includes for local functions
#include "drawbutton.h"
#include "focus.h"
#include "settings.h"
#include "timing.h"
#include "idle.h"
// end of includes for local functions


void setup() {
	pinMode(Focus_Relay, OUTPUT);
	pinMode(Shutter_Relay, OUTPUT);

	// set all relays low
	digitalWrite(Focus_Relay, LOW);
	digitalWrite(Shutter_Relay, LOW);
	
	Tft.begin();
	
	// update settings from eeprom
	recall_settings();

	// set initial exposure timer and pause
	//	Exposure.SetTimer(exposure_time[0],exposure_time[1],exposure_time[2]);
	Exposure.SetTimer(shutter_speeds[initial_exposure][0], shutter_speeds[initial_exposure][1], shutter_speeds[initial_exposure][2]);
	Exposure.PauseTimer();
	Exposure.Timer();

	// set dark frame timer and pause
	//	Dark_Frame.SetTimer(Dark_frame_time[0],Dark_frame_time[1],Dark_frame_time[2]);
	Dark_Frame_calc();
	Dark_Frame.Timer();

	// resume Exposure timer
	Exposure.ResumeTimer();
	Exposure.Timer();


}

void loop() {
	if (exposure_state == 0 && Dark_frame_state == 0 && idle_state == 0 && settings_state == 0) // initial state and idle state setup
	{
		Tft.fillScreen (Black);
		drawButton(40, 70, 160,55, Red, Blue, "Start", font_size + 3, Black);
		drawButton(70, 165, 100,40, Red, Blue, "Focus", font_size + 1, Black);
		drawButton(120, 270, 110, 30, Red, Blue, "Settings", font_size, Black);
		idle_state = 1;
	}

	if (settings_state == 1 ) // settings routines
	{
		Settings();
	}
	if (idle_state == 1)  // Idle state loop
	{
		idle();
	}

	if (exposure_state == 1) // exposure state loop
	{
		exposure();
		running_buttons();
	}

	if (exposure_state == 0 && Dark_frame_state == 1)
	{ // Dark_frame run state loop
		Dark_frame();
		running_buttons();
	}

}

