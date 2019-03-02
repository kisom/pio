#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <string.h>

#include <graphics.h>


namespace OLED {


// The Adafruit SSD1306 in use is the FeatherWing version.
// 	https://www.adafruit.com/product/2900
Adafruit_SSD1306	oled(128, 32, &Wire);

const uint8_t		NLINES = 4;

typedef struct {
	char	text[MAX_TEXT+1];	// tested manually
	bool	inverse;
} screenLine;

screenLine	lines[NLINES];

void
setup()
{
	oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	oled.setTextSize(1);
	oled.setTextColor(WHITE);        // Draw white text
	oled.clearDisplay();
}


void
clear()
{
	oled.clearDisplay();
}


void
pixel(uint16_t x, uint16_t y)
{
	oled.drawPixel(x, y, WHITE);
}


void
clearPixel(uint16_t x, uint16_t y)
{
	oled.drawPixel(x, y, BLACK);
}


void	
circle(uint16_t x, uint16_t y, uint16_t r, bool fill)
{
	if (fill) {
		oled.fillCircle(x, y, r, WHITE);
	}
	else {
		oled.drawCircle(x, y, r, WHITE);
	}
}


void
line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	oled.drawLine(x0, y0, x1, y1, WHITE);
}


void
line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool inv)
{
	if (inv) {
		oled.drawLine(x0, y0, x1, y1, WHITE);
	}
	else {
		oled.drawLine(x0, y0, x1, y1, BLACK);
	}
}



void
print(uint16_t x, uint16_t y, const char *text)
{
	oled.setCursor(x, y);
	oled.print(text);
	oled.display();
}


void
print(uint16_t x, uint16_t y, double value, int precision)
{
	oled.setCursor(x, y);
	oled.print(value, precision);
	oled.display();
}


void
show()
{
	oled.display();
}


void
clearLines()
{
	for (uint8_t i = 0; i < NLINES; i++) {
		memset(lines[i].text, 0, MAX_TEXT+1);
		lines[i].inverse = false;
	}
	oled.clearDisplay();
	oled.display();
}


static void
showLines()
{
	oled.clearDisplay();
	for (uint8_t i = 0; i < NLINES; i++) {
		oled.setCursor(0, i * 10);
		if (lines[i].inverse) {
			oled.setTextColor(BLACK, WHITE);
		}
		else {
			oled.setTextColor(WHITE, BLACK);
		}
		oled.println(lines[i].text);
	}
	oled.display();
}


void
print(uint8_t line, const char *text)
{
	if (line >= NLINES) {
		return;
	}

	int	slen = strnlen(text, MAX_TEXT);

	memset(lines[line].text, 0, MAX_TEXT+1);
	strncpy(lines[line].text, text, slen);
	lines[line].inverse = false;
	showLines();	
}


void
iprint(uint8_t line, const char *text)
{
	if (line >= NLINES) {
		return;
	}

	int	slen = strnlen(text, MAX_TEXT);

	memset(lines[line].text, 0, MAX_TEXT+1);
	strncpy(lines[line].text, text, slen);
	lines[line].inverse = true;
	showLines();	
}


} // namespace OLED
