// Dark_Frame_calc() was here
void Dark_Frame_calc()
{
	if (Exposure.ShowTotalSeconds() > 15 && use_dark_frame == 1)
	{
		if (Exposure.ShowTotalSeconds() > 29 && Exposure.ShowTotalSeconds() < 121)
		{
			Dark_Frame.SetTimer(Exposure.ShowTotalSeconds());
		}
		else
		{
			Dark_Frame.SetTimer(Exposure.ShowTotalSeconds() - (Exposure.ShowTotalSeconds() / dark_frame_factor) + 10);
		}
	}
	else
	{
		Dark_Frame.SetTimer(0);
	}
	Dark_Frame.PauseTimer();

}
// running_buttons() was here

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
	Tft.fillRect(3, 208, 105, 105, Black);
	Tft.fillRect(128, 213, 95, 95, Red);
	Tft.drawRect(128, 213, 95, 95, Blue);
	Tft.setCursor(145,255);
	Tft.setTextColor(Black);
	Tft.setTextSize(font_size);
	Tft.print(F("ABORT"));
}

void run_ready()
{
	// if there is a press within the start area
	// reset timers
	Exposure.SetTimer(shutter_speeds[initial_exposure + exposure_number][0], shutter_speeds[initial_exposure + exposure_number][1], shutter_speeds[initial_exposure + exposure_number][2]);
	Exposure.PauseTimer();
	Dark_Frame_calc();
	// setup display for run mode
	Tft.fillScreen(Black);
	Tft.setCursor(10,10);
	Tft.setTextColor(Red);
	Tft.setTextSize(font_size);
	Tft.print(F("Exposure "));
	Tft.print(exposure_number+1);
	Tft.print(F(" of "));
	Tft.print(number_of_exposures);
	Tft.fillRect (0,50,240,100,Red);
	sprintf(exposure_buffer, "%02lu:%02lu:%02lu", Exposure.ShowHours(), Exposure.ShowMinutes(), Exposure.ShowSeconds());
	char* g = exposure_buffer;
	Tft.setCursor(25,70);
	Tft.setTextColor(Black,Red);
	Tft.setTextSize(font_size+2);
	Tft.print(g);
	exposure_state = 1;
	Dark_frame_state = 0;
	idle_state = 0;
	Exposure.ResumeTimer();
	running_buttons_render();

}
void Dark_frame()
{
	Dark_Frame.Timer(); // run the timer

	if (Dark_Frame.TimeHasChanged() ) // this prevents the time from being constantly shown.
	{

		// start of Dark_frame counter update
		sprintf(dark_frame_buffer, "%02lu:%02lu:%02lu", Dark_Frame.ShowHours(), Dark_Frame.ShowMinutes(), Dark_Frame.ShowSeconds());
		char* y = dark_frame_buffer;
		Tft.setCursor(25,70);
		Tft.setTextColor(Black,Red);
		Tft.setTextSize(font_size + 2);
		Tft.print(y);
		// end of Dark_frame counter update
	}
	if (Dark_Frame.TimeCheck(0, 0, 0)) // check if Dark_frame timer is done, if so, start idle state
	{
		Tft.fillScreen(Black);
		Tft.setCursor(10,70);
		Tft.setTextColor(Red);
		Tft.setTextSize(font_size+1);
		Tft.print(F("Saving Image"));
		delay(write_delay);
		Tft.fillScreen(Black);
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

void exposure()
{
	digitalWrite(Shutter_Relay, HIGH); // turn on the exposure relay
	Exposure.Timer(); // run the timer


	if (Exposure.TimeHasChanged() ) // this prevents the time from being constantly shown.
	{
		sprintf(exposure_buffer, "%02lu:%02lu:%02lu", Exposure.ShowHours(), Exposure.ShowMinutes(), Exposure.ShowSeconds());
		char* g = exposure_buffer;
		Tft.setCursor(25,70);
		Tft.setTextColor(Black,Red);
		Tft.setTextSize(font_size+2);
		Tft.print(g);
		// end of updating exposure counter
	}
	if (Exposure.TimeCheck(0, 0, 0)) // check to see if exposure timer is done, if so change to Dark_frame state
	{
		digitalWrite(Shutter_Relay, LOW);
		exposure_state = 0;
		Tft.fillScreen(Black);
		if (use_dark_frame && Dark_Frame.ShowTotalSeconds() > 0)
		{
			Dark_Frame.ResumeTimer();
			Dark_Frame.Timer();
			Dark_frame_state = 1;
			Tft.setCursor(10,10);
			Tft.setTextColor(Red);
			Tft.setTextSize(font_size);
			Tft.print(F("Dark Frame "));
			Tft.print(exposure_number+1);
			Tft.print(F(" of "));
			Tft.print(number_of_exposures);
			Tft.fillRect (0,50,240,100,Red);
			sprintf(dark_frame_buffer, "%02lu:%02lu:%02lu", Dark_Frame.ShowHours(), Dark_Frame.ShowMinutes(), Dark_Frame.ShowSeconds());
			char* y = dark_frame_buffer;
			Tft.setCursor(25,70);
			Tft.setTextColor(Black,Red);
			Tft.setTextSize(font_size + 2);
			Tft.print(y);


			running_buttons_render();
		}
		else
		{
			Tft.setCursor(10,70);
			Tft.setTextColor(Red);
			Tft.setTextSize(font_size +1);
			Tft.print(F("Saving Image"));
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
