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
      if (w->x[i] < -1 || w->x[i] > w->screen_width
        || w->y[i] < -1 || w->y[i] > w->screen_height) {
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
  begin_draw(handler);
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_SPRITE) {
      set_color(handler, world->color[i]);
      draw_rectangle(handler, world->x[i], world->y[i], world->width[i], world->height[i]);
    }
  }
  end_draw(handler);
}

void system_input(World* world, const Colors *cs) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_PLAYER) {
      world->vx[i] = 0;
      if (world->key_left) world->vx[i] = -200;
      if (world->key_right) world->vx[i] = 200;
      if (world->key_fire) {
        spawn_bullet(world, world->x[i] + 17, world->y[i], cs);
        world->key_fire = 0;
      }
    }
  }
}

void system_enemy_ai(World* world) {
  int hit_edge = 0;
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_ENEMY) {
      if ((world->x[i] <= 0 && world->vx[i] < 0) ||
        (world->x[i] + world->width[i] >= world->screen_width && world->vx[i] > 0)) {
        hit_edge = 1;
        break;
      }
    }
  }
  if (hit_edge) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
      if (world->mask[i] & COMPONENT_ENEMY) {
        world->vx[i] *= -1;
        world->y[i] += 20;
      }
    }
  }
}

int main() {
  World *w = ecs_init(640, 480);
  XHandler *handler = create_handler("Space Invaders", 640, 480);
  wait_for_window_map(handler);
  w->running = 1;
  XEvent event;
  const Colors cs = create_colors(handler);
  set_color(handler, cs.green.pixel);
  int player = spawn_player(w, 320, 400, &cs);
  spawn_swarm(w, &cs);
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
          if (XEventsQueued(handler->display, QueuedAfterReading)) {
            XEvent next;
            XPeekEvent(handler->display, &next);
            if (next.type == KeyPress && next.xkey.time == event.xkey.time && next.xkey.keycode == event.xkey.keycode) {
              break;
            }
          }
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_Left) w->key_left = 0;
          if (keysym == XK_Right) w->key_right = 0;
          if (keysym == XK_space) w->key_fire = 0;
          break;
        }
        default: break;
      }
    }

    system_input(w, &cs);
    system_movement(w, dt);
    system_collision(w);
    system_enemy_ai(w);
    system_render(w, handler);
    // printf("Delta time %.3f, fps: %.1f\n", dt, 1/dt);
    usleep(1000); // 1 ms
  }
  destroy_handler(handler);
  ecs_destroy(w);
}