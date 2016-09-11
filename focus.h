void focus()
{
	Tft.fillScreen(Black);
	drawButton(20, 150, 200, 40, Blue, Yellow, "Focusing Camera", font_size, Black);
	digitalWrite(Focus_Relay, HIGH);
	delay(5000);
	digitalWrite(Focus_Relay, LOW);
	idle_state = 0;
}