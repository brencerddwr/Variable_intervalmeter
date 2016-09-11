#define TFT_DC 6
#define TFT_CS 5
// set font size for display
#define font_size 2

//  start of setup for touchscreen
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

// start of relay definitions
#define relay1 3
#define relay2 2

#define Focus_Relay relay2
#define Shutter_Relay relay1

// end of relay definitions

// define eeprom locations for settings
#define eeprom_initial_exposure 0
#define eeprom_number_of_exposures 1
#define eeprom_focus_before_shoot 2
#define eeprom_use_dark_frame 3
#define dark_frame_factor 4
