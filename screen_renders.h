#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

void darkFrameSetup ()
{
//		Dark_Frame_calc();
		// setup display for run mode
		Tft.fillScreen(Black);
		Tft.setCursor(10,10);
		Tft.setTextColor(Red);
		Tft.setTextSize(font_size);
		Tft.print("Exposure ");
		Tft.print(exposure_number+1);
		Tft.print("/");
		Tft.print(number_of_exposures);
		Tft.fillRect (0,50,240,100,Red);
		Tft.setCursor(10,10);
		Tft.setTextColor(Red);
		Tft.setTextSize(font_size);
		Tft.print("Dark Frame ");
		Tft.print(exposure_number+1);
		Tft.print("/");
		Tft.print(number_of_exposures);

		// start of Dark_frame counter update
		sprintf(dark_frame_buffer, "%02lu:%02lu:%02lu", Dark_Frame.ShowHours(), Dark_Frame.ShowMinutes(), Dark_Frame.ShowSeconds());
		char* y = dark_frame_buffer;
		Tft.setCursor(10,70);
		Tft.setTextColor(Black,Red);
		Tft.setTextSize(font_size + 2);
		Tft.print(y);

}
