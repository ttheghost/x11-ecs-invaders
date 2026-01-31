#include "x11_wrapper.h"
#include <stdlib.h>

XHandler *create_handler(const char* window_name, const unsigned int w, const unsigned int h) {
  XHandler *handler = malloc(sizeof(XHandler));
  handler->display = XOpenDisplay(NULL);
  handler->screen = DefaultScreen(handler->display);
  handler->window = XCreateSimpleWindow(
    handler->display,
    DefaultRootWindow(handler->display),
    0, 0, w, h, 1,
    WhitePixel(handler->display, handler->screen),
    BlackPixel(handler->display, handler->screen)
  );
  handler->gc = XCreateGC(handler->display, handler->window, 0, NULL);
  XSelectInput(handler->display, handler->window, ExposureMask | KeyPressMask | StructureNotifyMask);
  XStoreName(handler->display, handler->window, window_name);
  XMapWindow(handler->display, handler->window);
  return handler;
};

void wait_for_window_map(const XHandler *handler) {
  XEvent event;
  do {
    XNextEvent(handler->display, &event);
  } while (event.type != MapNotify);
}

void clear_screen(const XHandler *handler) {
  XClearWindow(handler->display, handler->window);
}

void set_color(const XHandler *handler, const unsigned long color) {
  XSetForeground(handler->display, handler->gc, color);
}

void draw_rectangle(const XHandler *handler, const int x, const int y, const unsigned int w, const unsigned int h) {
  XFillRectangle(handler->display, handler->window, handler->gc, x, y, w, h);
}

Colors create_colors(const XHandler *handler) {
  const Colormap cm = DefaultColormap(handler->display, handler->screen);
  Colors colors;
  colors.red.red = -1;
  colors.red.green = 0;
  colors.red.blue = 0;
  XAllocColor(handler->display, cm, &colors.red);

  colors.green.red = 0;
  colors.green.green = -1;
  colors.green.blue = 0;
  XAllocColor(handler->display, cm, &colors.green);

  colors.blue.red = 0;
  colors.blue.green = 0;
  colors.blue.blue = -1;
  XAllocColor(handler->display, cm, &colors.blue);
  return colors;
}

XColor create_color(const XHandler *handler, unsigned short red, unsigned short green, unsigned short blue) {
  const Colormap cm = DefaultColormap(handler->display, handler->screen);
  XColor color = {
    .red = red,
    .green = green,
    .blue = blue
  };
  XAllocColor(handler->display, cm, &color);
  return color;
}

void destroy_handler(XHandler *handler) {
  XFreeGC(handler->display, handler->gc);
  XDestroyWindow(handler->display, handler->window);
  XCloseDisplay(handler->display);
  free(handler);
}