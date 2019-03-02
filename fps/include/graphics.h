#ifndef __GRAPHICS_H
#define __GRAPHICS_H


#include <stdint.h>


namespace OLED {

static const int	HEIGHT = 32;
static const int	WIDTH  = 128;
static const int	MAX_TEXT = 20;

// Graphics primitives.
void	setup();
void	clear();
void	pixel(uint16_t x, uint16_t y);
void	clearPixel(uint16_t x, uint16_t y);
void	circle(uint16_t x, uint16_t y, uint16_t r, bool fill);
void	line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void	line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool inv);
void	show();

// Text display routines.
void	clearLines();
void	print(uint8_t line, const char *text);
void	iprint(uint8_t line, const char *text);
void	scrollForward(const char *text, bool inverse);
void	scrollBackward(const char *text, bool inverse);


} // namespace OLED
#endif // __GRAPHICS_H
