#include "x11_wrapper.h"
#include <stdlib.h>

XHandler *create_handler(const char* window_name, const unsigned int w, const unsigned int h) {
  XHandler *handler = malloc(sizeof(XHandler));
  handler->width = w; handler->height = h;
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
  handler->buffer = XCreatePixmap(handler->display, handler->window, w, h, DefaultDepth(handler->display, handler->screen));
  XSelectInput(handler->display, handler->window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
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

void begin_draw(const XHandler *handler) {
  XSetForeground(handler->display, handler->gc, BlackPixel(handler->display, handler->screen));
  XFillRectangle(handler->display, handler->buffer, handler->gc, 0, 0, handler->width, handler->height);
}

void draw_rectangle(const XHandler *handler, const int x, const int y, const unsigned int w, const unsigned int h) {
  XFillRectangle(handler->display, handler->buffer, handler->gc, x, y, w, h);
}

void end_draw(const XHandler *handler) {
  XCopyArea(handler->display, handler->buffer, handler->window, handler->gc, 0, 0, handler->width, handler->height, 0, 0);
  XSync(handler->display, False);
}

void clear_screen(const XHandler *handler) {
  XClearWindow(handler->display, handler->window);
}

void set_color(const XHandler *handler, const unsigned long color) {
  XSetForeground(handler->display, handler->gc, color);
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

  colors.white.pixel = WhitePixel(handler->display, handler->screen);
  colors.black.pixel = BlackPixel(handler->display, handler->screen);
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