#include <stdio.h>
#include "X11/Xlib.h"
#include <X11/keysym.h>

int main() {
  Display* display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "unable to connect to display");
    return 1;
  }
  const int screen_num = DefaultScreen(display);
  Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 640, 489, 2, WhitePixel(display, screen_num), BlackPixel(display, screen_num));
  XMapWindow(display, win);
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
  }
}