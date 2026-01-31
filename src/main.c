#include "x11_wrapper.h"

int main() {
  XHandler *handler = create_handler("Space Invaders", 640, 480);
  wait_for_window_map(handler);
  while (1) {}
  destroy_handler(handler);
}