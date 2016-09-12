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

void shutter_speed()
{
	sprintf(shutterSpeed, "%02u:%02u:%02u", shutter_speeds[initial_exposure][0], shutter_speeds[initial_exposure][1], shutter_speeds[initial_exposure][2]);
}

void setting_darkframe()
{
	if (!use_dark_frame)
	{
		drawButton(25, 210, 45, 30, Black, Blue, "off", font_size, Purple);
	}
	else
	{
		drawButton(25, 210, 45, 30, Black, Blue, "on", font_size, Cyan);
	}
}
void settings_render_shutter_speed()
{
	shutter_speed();
	Tft.setCursor(46,50);
	Tft.setTextColor(Black,Red);
	Tft.setTextSize(font_size + 1);
	Tft.print(shutterSpeed);
}
void settings_render_exposure_count()
{
	Tft.setCursor(110,125);
	Tft.setTextColor(Black,Red);
	Tft.setTextSize(font_size +1);
	Tft.print(number_of_exposures);
}

void settings_render()
{
	Tft.fillScreen(Red);
	Tft.drawRect(0, 0,240, 85, Green);

	// initial exposure section start
	Tft.setCursor(25,10);
	Tft.setTextColor(Blue);
	Tft.setTextSize(font_size);
	Tft.print("Initial Exposure");
	drawButton(200, 40, 30, 40, Black, Blue, "+", font_size + 1, Red);
	Tft.fillRect(43, 40, 153,40, Red);
	Tft.drawRect(43, 40, 153,40, Blue);
	settings_render_shutter_speed();
	drawButton(10, 40, 30, 40, Black, Blue, "-", font_size + 1, Red);
	// initial exposure section end

	// Number of exposures section start
	Tft.drawRect(0, 85, 240,85, Green);
	Tft.setCursor(38,92);
	Tft.setTextColor(Blue);
	Tft.setTextSize(font_size);
	Tft.print("Exposure Count");
	drawButton(10, 115, 30, 40, Black, Blue, "-", font_size + 1, Red);
	Tft.fillRect(43, 115, 153,40, Red);
	Tft.drawRect(43, 115, 153,40, Blue);
	settings_render_exposure_count();
	drawButton(200, 115, 30, 40, Black, Blue, "+", font_size + 1, Red);
	// Number of exposures section end

	// use darkframe
	Tft.setCursor(5,190);
	Tft.setTextColor(Blue);
	Tft.setTextSize(font_size);
	Tft.print("Dark Frame");
	setting_darkframe();

	// store button start
	if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
	focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
	{
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
	}
	else
	{
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Red);
	}
	// store button end

	// recall button start
	if (initial_exposure != EEPROM.read(eeprom_initial_exposure) || number_of_exposures != EEPROM.read(eeprom_number_of_exposures) ||
	focus_before_shoot != EEPROM.read(eeprom_focus_before_shoot) || use_dark_frame != EEPROM.read(eeprom_use_dark_frame))
	{
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
	}
	else
	{
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Red);
	}

	// recall button end

	// use button start
	drawButton(5, 265, 80, 40, Black, Blue, "Use", font_size + 1, Green);
	// use button end
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
		if (initial_exposure > 14)
		{ initial_exposure = 14;
		}
		if (number_of_exposures > 15 - initial_exposure)
		{
			number_of_exposures = 15 - initial_exposure;
		}
		settings_render_shutter_speed();
		settings_render_exposure_count();
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
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
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
		settingsMatch = false;
		settingsRecalled = false;
	}
	// end initial exposure decrease

	// start number of exposures increase
	if (p.z > __PRESURE && (p.x > 200 && p.x < 230) && (p.y > 115 && p.y < 155))
	{
		if (number_of_exposures < 15)
		{
			number_of_exposures = number_of_exposures + 1;
		}
		if (number_of_exposures > 15 - initial_exposure && initial_exposure > 0 )
		{
			initial_exposure = initial_exposure - 1;
			//number_of_exposures=14-initial_exposure;
		}
		settings_render_shutter_speed();
		settings_render_exposure_count();
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
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
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
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
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Blue);
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Red);
		setting_darkframe();
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
				drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Red);
			}
			else
			{
				drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Blue);
			}
			drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Red);
			settingsMatch = true;
		}
		// end current settings match check

	}

	if (p.z > __PRESURE && (p.x > 25 && p.x < 70) && (p.y > 210 && p.y < 240))
	{
		use_dark_frame = !use_dark_frame;
		setting_darkframe();
		drawButton(150, 205, 85, 40, Black, Blue, "Store", font_size, Green);
		drawButton(150, 265, 85, 40, Black, Blue, "Recall", font_size, Green);
		settingsMatch = false;
		settingsRecalled = false;
	}

	delay(350);
}

