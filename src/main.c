#include <stdio.h>
#include "X11/Xlib.h"
#include <X11/keysym.h>

void drawRectangle(Display* disp, const Window win, const GC *gc, const int x, const int y, const unsigned int w, const unsigned int h, const unsigned long color) {
  XSetForeground(disp, *gc, color);
  XFillRectangle(disp, win, *gc, x, y, w, h);
}

int main() {
  Display* display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "unable to connect to display");
    return 1;
  }
  const int screen_num = DefaultScreen(display);
  const Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 640, 489, 2, WhitePixel(display, screen_num), BlackPixel(display, screen_num));
  XMapWindow(display, win);

  GC gc = XCreateGC(display, win, 0, 0);
  Colormap colormap = DefaultColormap(display, screen_num);
  XColor green = {
    .red = 0,
    .green = -1,
    .blue = 0,
  };
  XAllocColor(display, colormap, &green);

  XSelectInput(display, win, StructureNotifyMask|KeyPressMask|KeyReleaseMask|KeymapStateMask);

  XEvent event;
  while (1) {
    while (XPending(display)) {
      XNextEvent(display, &event);
      switch (event.type) {
        case KeymapNotify:
          XRefreshKeyboardMapping(&event.xmapping);
          break;
        case KeyRelease:
        case KeyPress: {
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_Escape || keysym == XK_q) {
            XCloseDisplay(display);
            return 0;
          }
          break;
        }
        default: break;
      }
    }
    drawRectangle(display, win, &gc, 0, 0, 10, 80, green.pixel);
  }
}