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
      }
    }
    if (w->mask[i] & COMPONENT_BULLET) {
      for (int j = 0; j < MAX_ENTITIES; j++) {
        if (w->mask[j] & COMPONENT_ENEMY) {
          if (w->x[i] < w->x[j] + w->width[j] &&
              w->x[i] + w->width[i] > w->x[j] &&
              w->y[i] < w->y[j] + w->height[j] &&
              w->y[i] + w->height[i] > w->y[j]
            ) {
            ecs_destroy_entity(w, j);
            ecs_destroy_entity(w, i);
          }
        }
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

void system_input(World* world) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_PLAYER) {
      world->vx[i] = 0;
      if (world->key_left) world->vx[i] = -200;
      if (world->key_right) world->vx[i] = 200;
      if (world->key_fire) {
        spawn_bullet(world, world->x[i] + 17, world->y[i]);
        world->key_fire = 0;
      }
    }
  }
}

int main() {
  World *w = ecs_init(640, 480);
  int player = spawn_player(w, 320, 400);
  spawn_swarm(w);
  XHandler *handler = create_handler("Space Invaders", 640, 480);
  wait_for_window_map(handler);
  w->running = 1;
  XEvent event;
  const Colors cs = create_colors(handler);
  set_color(handler, cs.green.pixel);
  system_render(w, handler);
  while (w->running) {
    double dt = get_delta_time();
    if (dt > 1) continue;

    while (XPending(handler->display)) {
      XNextEvent(handler->display, &event);
      switch (event.type) {
        case KeyPress: {
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_Left) w->key_left = 1;
          if (keysym == XK_Right) w->key_right = 1;
          if (keysym == XK_space) w->key_fire = 1;
          break;
        }
        case KeyRelease: {
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_Left) w->key_left = 0;
          if (keysym == XK_Right) w->key_right = 0;
          if (keysym == XK_space) w->key_fire = 0;
          break;
        }
        default: break;
      }
    }

    system_input(w);
    system_movement(w, dt);
    system_collision(w);
    system_render(w, handler);
    // printf("Delta time %.3f, fps: %.1f\n", dt, 1/dt);
    usleep(1000); // 1 ms
  }
  destroy_handler(handler);
  ecs_destroy(w);
}