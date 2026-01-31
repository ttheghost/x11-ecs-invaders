#include "x11_wrapper.h"
#include <X11/keysym.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "ecs.h"
#include "factory.h"

// extern int usleep (__useconds_t);

void system_movement(World* w, const double dt) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if ((w->mask[i] & (COMPONENT_POS | COMPONENT_VEL)) == (COMPONENT_POS | COMPONENT_VEL)) {
      w->x[i] += w->vx[i] * dt;
      w->y[i] += w->vy[i] * dt;
    }
  }
}

void system_collision(World* w) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (w->mask[i] & COMPONENT_POS) {
      if (w->x[i] < 0 || w->x[i] > w->screen_width
        || w->y[i] < 0 || w->y[i] > w->screen_height) {
        ecs_destroy_entity(w, i);
        printf("Boom\n");
        fflush(stdout);
      }
    }
  }
}

void system_render(const World *world, const XHandler *handler) {
  clear_screen(handler);
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_SPRITE) {
      draw_rectangle(handler, world->x[i], world->y[i], world->width[i], world->height[i]);
    }
  }
}

int main() {
  World *w = ecs_init(640, 480);
  int player = spawn_player(w, 320, 400);
  XHandler *handler = create_handler("Space Invaders", 640, 480);
  wait_for_window_map(handler);
  w->running = 1;
  XEvent event;
  const Colors cs = create_colors(handler);
  set_color(handler, cs.green.pixel);
  system_render(w, handler);
  while (w->running) {
    double dt = get_delta_time();

    while (XPending(handler->display)) {
      XNextEvent(handler->display, &event);
      switch (event.type) {
        case KeyPress: {
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_a) {
            if (w->x[player] - 10 >= 0) w->x[player] -= 10;
          } else if (keysym == XK_d) {
            if (w->x[player] + 50 <= 640) w->x[player] += 10;
          } else if (keysym == XK_q) {
            w->running = 0;
          } else if (keysym == XK_space) {
            // shut
            int b = ecs_create_entity(w);
            if (b == -1) {
              printf("Failed to create entity\n");
              exit(1);
            }
            w->mask[b] = COMPONENT_SPRITE | COMPONENT_VEL | COMPONENT_POS; //| COMPONENT_BULLET | COMPONENT_COLLIDER;
            w->x[b] = w->x[player];
            w->y[b] = w->y[player];
            w->vx[b] = 0;
            w->vy[b] = -20;
            w->width[b] = 5;
            w->height[b] = 10;
          }
          break;
        }
        default: break;
      }
    }

    system_movement(w, dt);
    system_collision(w);
    system_render(w, handler);
    // printf("Delta time %.3f, fps: %.1f\n", dt, 1/dt);
    usleep(1000); // 1 ms
  }
  destroy_handler(handler);
  ecs_destroy(w);
}