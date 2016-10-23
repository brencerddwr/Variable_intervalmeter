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
	
	if ((p.z > __PRESURE && (p.x > 20 && p.x < 100) && (p.y > 270 && p.y < 300)))
	{ // check if the press is within the settings area
			mainHelpState=true;
			mainHelp ();
	}
	

	if ((p.z > __PRESURE && (p.x > 120 && p.x < 230) && (p.y > 270 && p.y < 300)))
	{ // check if the press is within the settings area
		settings_render();
		idle_state = 0;
		settings_state = 1;
	}
}

