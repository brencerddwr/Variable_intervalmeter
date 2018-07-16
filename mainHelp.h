void mainHelp()
{
	if (mainHelpState && !mainHelpRendered)
	{
		Tft.fillScreen(Black);
		Tft.setCursor(25,10);
		Tft.setTextColor(Red);
		Tft.setTextSize(font_size+1);
		Tft.println(F("How to Use"));
		Tft.println();
		Tft.setTextSize(font_size);
		Tft.println(F("Start"));
		Tft.setTextSize(font_size-1);
		Tft.println();
		Tft.println(F("     begin a capture sequence"));
		Tft.println();
		Tft.setTextSize(font_size);
		Tft.println(F("Focus"));
		Tft.setTextSize(font_size-1);
		Tft.println();
		Tft.println(F("     half press for 5 seconds allowing camera to focus"));
		Tft.println();
		Tft.setTextSize(font_size);
		Tft.println(F("Settings"));
		Tft.setTextSize(font_size-1);
		Tft.println();
		Tft.println(F("     access the settings screen"));
		Tft.println();
		Tft.setTextSize(font_size);
		Tft.setTextColor(Pink);
		Tft.println(F("Canon Users"));
		Tft.setTextSize(font_size-1);
		Tft.println();
		Tft.println(F("Disable Darkframe at this time,\nit may not work. Disable slow\nshutter noise reduction in camera\nas well. The times are for Pentax\nCameras."));
		
		drawButton(20,270,80,30,Red,Blue,"Return",font_size, Black);
		mainHelpRendered=!mainHelpRendered;
		mainHelpState=true;
		idle_state=0;
	}
	
	Point p = ts.getPoint();

	p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
	if ((p.z > __PRESURE && (p.x > 20 && p.x < 100) && (p.y > 270 && p.y < 300)))
	{ // check if the press is within the settings area
		idle_state=0;
		mainHelpState=false;
		mainHelpRendered=false;
	}

}