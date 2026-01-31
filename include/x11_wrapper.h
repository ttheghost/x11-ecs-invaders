#pragma once
#include <X11/Xlib.h>

typedef struct {
  Display *display;
  Window window;
  int screen;
  GC gc;
  Pixmap buffer;
  XFontStruct *font;
  unsigned int width, height;
} XHandler;

typedef struct {
  XColor red, green, blue, white, black;
} Colors;

XHandler *create_handler(const char* window_name, unsigned int w, unsigned int h);

void wait_for_window_map(const XHandler *handler);

void begin_draw(const XHandler *handler);

void draw_rectangle(const XHandler *handler, int x, int y, unsigned int w, unsigned int h);

void draw_text(const XHandler *handler, const char *text, const int len, int x, int y);

void draw_text_exact_x(const XHandler *handler, const char *text, const int len, int x, int y);

void end_draw(const XHandler *handler);

void clear_screen(const XHandler *handler);

void set_color(const XHandler *handler, unsigned long color);

Colors create_colors(const XHandler *handler);

XColor create_color(const XHandler *handler, unsigned short red, unsigned short green, unsigned short blue);

void destroy_handler(XHandler *handler);