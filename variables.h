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

unsigned int shutter_speeds[15][3] = //hours:minutes:seconds
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
	{2, 0, 0},
	{4, 0, 0}
};
