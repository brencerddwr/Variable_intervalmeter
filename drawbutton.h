void drawButton(unsigned int poX,unsigned int poY,unsigned int length,unsigned int width,unsigned int fillColor,unsigned int edgeColor, const char *string,unsigned int size,unsigned int textColor)
{
	Tft.fillRect(poX, poY, length, width, fillColor);
	Tft.drawRect(poX, poY, length, width, edgeColor);
	Tft.setCursor(poX+7,poY +9);
	Tft.setTextColor(textColor);
	Tft.setTextSize(size);
	Tft.print(string);
	//  Tft.print(string, poX + 5, poY + 9, size, textColor);
}
