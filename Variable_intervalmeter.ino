#include <stdint.h>
#include <EEPROM.h>
#include <SPI.h>
#include <TFTv2.h>
#include <SeeedTouchScreen.h>
#include <CountUpDownTimer.h>

/* some RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F

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

// initial setup of variables
char exposure_buffer[12];
char dark_frame_buffer[12];
char shutterSpeed[12];
byte initial_exposure = 0;
byte number_of_exposures = 4;
byte exposure_number = 0;
boolean focus_before_shoot = 0;
boolean use_dark_frame = 1;
boolean exposure_state = 0;
boolean Dark_frame_state = 0;
boolean idle_state = 0;
boolean settings_state = 0;
boolean settingsMatch = true;
boolean settingsRecalled = true;
int write_delay = 5000;  // delay to allow writing to memory card
// initial time variables
// this is updated later to match the times in the eeprom on the main chip.
unsigned int shutter_speeds[14][3] = //hours:minutes:seconds
{
  {0, 0, 1},
  {0, 0, 2},
  {0, 0, 4},
  {0, 0, 8},
  {0, 0, 15},
  {0, 0, 30},
  {0, 1, 0},
  {0, 2, 0},
  {0, 4, 0},
  {0, 8, 0},
  {0, 15, 0},
  {0, 30, 0},
  {1, 0, 0},
  {2, 0, 0}
};

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM);

// end of setup for touchscreen

// setup of counter objects

CountUpDownTimer Exposure(DOWN);
CountUpDownTimer Dark_Frame(DOWN);

void setup() {
  // put your setup code here, to run once:
  pinMode(Focus_Relay, OUTPUT);
  pinMode(Shutter_Relay, OUTPUT);
  // set all relays low
  digitalWrite(Focus_Relay, LOW);
  digitalWrite(Shutter_Relay, LOW);
  
    
  // set up input pins
  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library
 
 // tweek SPI settings
 SPI.beginTransaction (SPISettings(8000000, MSBFIRST, SPI_MODE0));
 SPI.endTransaction ();
 
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
  // put your main code here, to run repeatedly:
  if (exposure_state == 0 && Dark_frame_state == 0 && idle_state == 0 && settings_state == 0) // initial state and idle state setup
  {
    Tft.fillScreen(0, 240, 0, 320, BLACK);
    drawButton(40, 70, 160, 55, RED, BLUE, "Start", font_size + 3, BLACK);
    drawButton(70, 165, 100, 40, RED, BLUE, "Focus", font_size + 1, BLACK);
    drawButton(120, 270, 110, 30, RED, BLUE, "Settings", font_size, BLACK);
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
void idle()
{
  // watch for a press on the touch screen
  Point p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

  if ((p.z > __PRESURE && (p.x > 40 && p.x < 200) && (p.y > 70 && p.y < 155)))
  { // check if the press is within the start area
    run_ready();
  }

  if ((p.z > __PRESURE && (p.x > 70 && p.x < 170) && (p.y > 165 && p.y < 205)))
  { // check if the press is within the start area
    focus();
  }

  if ((p.z > __PRESURE && (p.x > 120 && p.x < 230) && (p.y > 270 && p.y < 300)))
  { // check if the press is within the settings area
    settings_render();
    idle_state = 0;
    settings_state = 1;
  }
}

void run_ready()
{

  // if there is a press within the start area
  // reset timers
  //Exposure.SetTimer(exposure_time[0],exposure_time[1],exposure_time[2]);
  Exposure.SetTimer(shutter_speeds[initial_exposure + exposure_number][0], shutter_speeds[initial_exposure + exposure_number][1], shutter_speeds[initial_exposure + exposure_number][2]);
  Exposure.PauseTimer();
  //	Dark_Frame.SetTimer(Dark_frame_time[0],Dark_frame_time[1],Dark_frame_time[2]);
  Dark_Frame_calc();
  // setup display for run mode
  Tft.fillRectangle(0, 0, 240, 320, BLACK);
  Tft.drawString("Exposure", 10, 10, font_size, RED);
  Tft.fillRectangle(0, 50, 240, 100, RED);
  sprintf(exposure_buffer, "%02lu:%02lu:%02lu", Exposure.ShowHours(), Exposure.ShowMinutes(), Exposure.ShowSeconds());
  char* g = exposure_buffer;
  Tft.drawString(g, 10, 70, font_size + 2, BLACK);
  exposure_state = 1;
  Dark_frame_state = 0;
  idle_state = 0;
  Exposure.ResumeTimer();
  running_buttons_render();

}

void exposure()
{
  digitalWrite(Shutter_Relay, HIGH); // turn on the exposure relay
  Exposure.Timer(); // run the timer


  if (Exposure.TimeHasChanged() ) // this prevents the time from being constantly shown.
  {
    Tft.drawString("Exposure", 10, 10, font_size, RED);
    Tft.fillRectangle(0, 50, 240, 100, RED); // start of updating exposure counter
    sprintf(exposure_buffer, "%02lu:%02lu:%02lu", Exposure.ShowHours(), Exposure.ShowMinutes(), Exposure.ShowSeconds());
    char* g = exposure_buffer;
    Tft.drawString(g, 10, 70, font_size + 2, BLACK); // end of updating exposure counter
  }
  if (Exposure.TimeCheck(0, 0, 0)) // check to see if exposure timer is done, if so change to Dark_frame state
  { exposure_state = 0;
    Tft.fillScreen();
    //		digitalWrite(Focus_Relay,LOW);
    digitalWrite(Shutter_Relay, LOW);
    if (use_dark_frame)
    {
      Dark_Frame.ResumeTimer();
      Dark_Frame.Timer();
      Dark_frame_state = 1;
      running_buttons_render();
    }
    else
    {
      Tft.drawString("Saving Image", 10, 70, font_size + 1, RED);
      delay(write_delay);
      Dark_frame_state = 0;
      exposure_number++;
      if (exposure_number < number_of_exposures)
      {
        run_ready();
      }
      else
      {
        exposure_number = 0;
      }

    }
  }
}
void Dark_frame()
{
  Dark_Frame.Timer(); // run the timer

  if (Dark_Frame.TimeHasChanged() ) // this prevents the time from being constantly shown.
  {
    Tft.drawString("Dark Frame", 10, 10, font_size, RED);
    Tft.fillRectangle(0, 50, 240, 100, RED); // start of Dark_frame c/*ounter update
    sprintf(dark_frame_buffer, "%02lu:%02lu:%02lu", Dark_Frame.ShowHours(), Dark_Frame.ShowMinutes(), Dark_Frame.ShowSeconds());
    char* y = dark_frame_buffer;
    Tft.drawString(y, 10, 70, font_size + 2, BLACK); // end of Dark_frame counter update
  }
  if (Dark_Frame.TimeCheck(0, 0, 0)) // check if Dark_frame timer is done, if so, start idle state
  {
    Tft.fillScreen();
    Tft.drawString("Saving Image", 10, 70, font_size + 1, RED);
    delay(write_delay);
    Tft.fillScreen();
    Dark_frame_state = 0;
    exposure_number++;
    if (exposure_number < number_of_exposures)
    {
      run_ready();
    }
    else
    {
      exposure_number = 0;
    }
  }
}


void Dark_Frame_calc()
{
  if (Exposure.ShowAsSeconds() > 30 && use_dark_frame == 1)
  {
    if (Exposure.ShowAsSeconds() > 59 && Exposure.ShowAsSeconds() < 121)
    {
      Dark_Frame.SetTimer(Exposure.ShowAsSeconds());
    }
    else
    {
      Dark_Frame.SetTimer(Exposure.ShowAsSeconds() - (Exposure.ShowAsSeconds() / dark_frame_factor) + 10);
    }
  }
  else
  {
    Dark_Frame.SetTimer(0);
  }
  Dark_Frame.PauseTimer();

}
void running_buttons()
{
  Point p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);


  // start emergency stop button
  if (p.z > __PRESURE && (p.x > 128 && p.x < 223) && (p.y > 213 && p.y < 308))
  {
    exposure_state = 0;
    Dark_frame_state = 0;
    idle_state = 0;
    exposure_number = 0;
    digitalWrite(Shutter_Relay, LOW);

  }
  // end emergency stop button
}

void running_buttons_render()
{
  Tft.fillRectangle(3, 208, 105, 105, BLACK);
  Tft.fillRectangle(128, 213, 95, 95, RED);
  Tft.drawRectangle(128, 213, 95, 95, BLUE);
  Tft.drawString("ABORT", 145, 255, font_size, BLACK);
}

void Settings()
{
  Point p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

  // start initial exposure increase
  if (p.z > __PRESURE && (p.x > 200 && p.x < 230) && (p.y > 40 && p.y < 80))
  {
    initial_exposure = initial_exposure + 1;
    if (initial_exposure > 13)
    { initial_exposure = 13;
    }
    if (number_of_exposures > 14 - initial_exposure)
    {
      number_of_exposures = 14 - initial_exposure;
    }
    settings_render_shutter_speed();
    settings_render_exposure_count();
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
    settingsMatch = false;
    settingsRecalled = false;
  }
  // end initial exposure increase

  // start initial exposure decrease
  if (p.z > __PRESURE && (p.x > 10 && p.x < 40) && (p.y > 40 && p.y < 80))
  {
    if (initial_exposure > 0)
    {
      initial_exposure = initial_exposure - 1;
    }
    settings_render_shutter_speed();
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
    settingsMatch = false;
    settingsRecalled = false;
  }
  // end initial exposure decrease

  // start number of exposures increase
  if (p.z > __PRESURE && (p.x > 200 && p.x < 230) && (p.y > 115 && p.y < 155))
  {
    if (number_of_exposures < 14)
    {
      number_of_exposures = number_of_exposures + 1;
    }
    if (number_of_exposures > 14 - initial_exposure && initial_exposure > 0 )
    {
      initial_exposure = initial_exposure - 1;
      //number_of_exposures=14-initial_exposure;
    }
    settings_render_shutter_speed();
    settings_render_exposure_count();
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
    settingsMatch = false;
    settingsRecalled = false;
  }
  // end number of exposures increase

  // start number of exposures decrease
  if (p.z > __PRESURE && (p.x > 10 && p.x < 40) && (p.y > 115 && p.y < 155))
  {
    number_of_exposures = number_of_exposures - 1;
    if (number_of_exposures < 1)
    {
      number_of_exposures = 1;
    }
    settings_render_exposure_count();
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
    settingsMatch = false;
    settingsRecalled = false;
  }
  // end number of exposures decrease

  // start store button
  if (p.z > __PRESURE && (p.x > 150 && p.x < 235) && (p.y > 205 && p.y < 245))
  {
    if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
        focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
    {
      save_settings();
      settingsRecalled = false;
      settings_state = 0;
      idle_state = 0;
    }
    else
    {
      settings_state = 0;
      idle_state = 0;
    }
  }
  // end store button

  // start recall button
  if (p.z > __PRESURE && (p.x > 150 && p.x < 235) && (p.y > 265 && p.y < 305))
  {
    recall_settings();
    settings_render_shutter_speed();
    settings_render_exposure_count();
    settingsRecalled = true;
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, BLUE);
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, RED);
    setting_darkframe();
    //	settings_state=0;
    //	idle_state=0;
  }
  // end recall button

  // start use button
  if (p.z > __PRESURE && (p.x > 5 && p.x < 85) && (p.y > 265 && p.y < 305))
  {
    settings_state = 0;
    idle_state = 0;
  }
  // end use button

  // check if current settings match eeprom and set buttons as needed.
  if (!settingsMatch) {
    if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
        focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
    {

    }
    else
    {
      if (!settingsRecalled)
      {
        drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, RED);
      }
      else
      {
        drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, BLUE);
      }
      drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, RED);
      settingsMatch = true;
    }
    // end current settings match check

  }

  if (p.z > __PRESURE && (p.x > 25 && p.x < 70) && (p.y > 210 && p.y < 240))
  {
    use_dark_frame = !use_dark_frame;
    setting_darkframe();
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
    settingsMatch = false;
    settingsRecalled = false;
  }

  delay(350);
}

void settings_render()
{
  Tft.fillRectangle(0, 0, 240, 320, RED);
  Tft.drawRectangle(0, 0, 240, 84, GREEN);
  // initial exposure section start
  Tft.drawString("Initial Exposure", 25, 10, font_size, BLUE);
  drawButton(200, 40, 30, 40, BLACK, BLUE, "+", font_size + 1, RED);
  settings_render_shutter_speed();
  drawButton(10, 40, 30, 40, BLACK, BLUE, "-", font_size + 1, RED);
  // initial exposure section end

  // Number of exposures section start
  Tft.drawRectangle(0, 85, 240, 90, GREEN);
  Tft.drawString("Exposure Count", 25, 90, font_size, BLUE);
  drawButton(10, 115, 30, 40, BLACK, BLUE, "-", font_size + 1, RED);
  settings_render_exposure_count();
  drawButton(200, 115, 30, 40, BLACK, BLUE, "+", font_size + 1, RED);
  Tft.drawChar('-', 40, 213, font_size + 2, RED);
  // Number of exposures section end

  // use darkframe
  Tft.drawString("Dark Frame", 5, 190, font_size, BLUE);
  setting_darkframe();

  // store button start
  if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
      focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
  {
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, GREEN);
  }
  else
  {
    drawButton(150, 205, 85, 40, BLACK, BLUE, "Store", font_size, RED);
  }
  // store button end

  // recall button start
  if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
      focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
  {
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, GREEN);
  }
  else
  {
    drawButton(150, 265, 85, 40, BLACK, BLUE, "Recall", font_size, RED);
  }

  // recall button end

  // use button start
  drawButton(5, 265, 80, 40, BLACK, BLUE, "Use", font_size + 1, GREEN);
  // use button end
}
void save_settings()
{
  EEPROM.write(eeprom_initial_exposure, initial_exposure);
  EEPROM.write(eeprom_number_of_exposures, number_of_exposures);
  EEPROM.write(eeprom_focus_before_shoot, focus_before_shoot);
  EEPROM.write(eeprom_use_dark_frame, use_dark_frame);
}

void recall_settings()
{
  initial_exposure = EEPROM.read(eeprom_initial_exposure);
  number_of_exposures = EEPROM.read(eeprom_number_of_exposures);
  focus_before_shoot = EEPROM.read(eeprom_focus_before_shoot);
  use_dark_frame = EEPROM.read(eeprom_use_dark_frame);
}
void drawButton(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U fillColor, INT16U edgeColor, const char *string, INT16U size, INT16U textColor)
{
  Tft.fillRectangle(poX, poY, length, width, fillColor);
  Tft.drawRectangle(poX, poY, length, width, edgeColor);
  Tft.drawString(string, poX + 5, poY + 9, size, textColor);
}
void shutter_speed()
{
  sprintf(shutterSpeed, "%02u:%02u:%02u", shutter_speeds[initial_exposure][0], shutter_speeds[initial_exposure][1], shutter_speeds[initial_exposure][2]);

}
void settings_render_shutter_speed()
{
  shutter_speed();
  Tft.fillRectangle(43, 40, 153, 40, RED);
  Tft.drawRectangle(43, 40, 153, 40, BLUE);
  Tft.drawString(shutterSpeed, 46, 50, font_size + 1, BLACK);
}
void settings_render_exposure_count()
{
  Tft.fillRectangle(43, 115, 153, 40, RED);
  Tft.drawRectangle(43, 115, 153, 40, BLUE);
  Tft.drawNumber(number_of_exposures, 110, 125, font_size + 1, BLACK);
}
void focus()
{
  Tft.fillRectangle(0, 0, 240, 320, BLACK);
  drawButton(20, 150, 200, 40, BLUE, YELLOW, "Focusing Camera", font_size, BLACK);
  digitalWrite(Focus_Relay, HIGH);
  delay(5000);
  digitalWrite(Focus_Relay, LOW);
  idle_state = 0;
}
void setting_darkframe()
{
  if (!use_dark_frame)
  {
    drawButton(25, 210, 45, 30, BLACK, BLUE, "off", font_size, Purple);
  }
  else
  {
    drawButton(25, 210, 45, 30, BLACK, BLUE, "on", font_size, Cyan);
  }
}