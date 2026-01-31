#include "x11_wrapper.h"
#include "ecs.h"
#include "factory.h"
#include <X11/keysym.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define TARGET_FRAME_TIME_MICRO 16667 // 16.666 ms = 60 FPS

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
            w->score++;
          }
        }
      }
    }
  }
}

void system_render(const World *world, const XHandler *handler, const double dt) {
  begin_draw(handler);
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_SPRITE) {
      if (world->mask[i] & COMPONENT_ENEMY) {
        draw_sprite(handler, (int)world->x[i], (int)world->y[i]);
        continue;
      }
      set_color(handler, world->color[i]);
      draw_rectangle(handler, (int)world->x[i], (int)world->y[i], world->width[i], world->height[i]);
    }
  }
  if (world->game_over) {
    set_color(handler, WhitePixel(handler->display, handler->screen));
    draw_rectangle(handler, 0, 0, world->screen_width, world->screen_height);
    set_color(handler, BlackPixel(handler->display, handler->screen));
    draw_text(handler, "GAME OVER", 9, world->screen_width/2, world->screen_height/2);
    draw_text(handler, "Press ESC to Quit", 17, world->screen_width/2, world->screen_height/2 + 40);
  }
  set_color(handler, WhitePixel(handler->display, handler->screen));
  char buffer[20];
  int l = sprintf(buffer, "Score: %d", world->score);
  draw_text_exact_x(handler, buffer, l, 5, 10);
  l = sprintf(buffer, "FPS: %.1f", 1/dt);
  draw_text_exact_x(handler, buffer, l, 5, 30);
  end_draw(handler);
}

void system_input(World* world, const Colors *cs) {
  if (world->game_over) {
    if (world->key_escape) {
      world->running = 1;
      world->score = 0;
      world->game_over = 0;
      world->key_escape = 0;
      for (int i = 0; i < MAX_ENTITIES; i++) {
        if (world->mask[i] & COMPONENT_PLAYER) {
          world->x[i] = 320; world->y[i] = 400;
        } else {
          ecs_destroy_entity(world, i);
        }
      }
      spawn_swarm(world, cs);
    }
    return;
  }
  if (world->key_escape) {
    world->running = 0;
  }
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] & COMPONENT_PLAYER) {
      world->vx[i] = 0;
      if (world->key_left) {
        if (world->x[i] - 5 > 0) world->vx[i] = -200;
      }
      if (world->key_right)
        if (world->x[i] + world->width[i] + 5 < world->screen_width) world->vx[i] = 200;
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
      if (world->y[i] >= 400) {
        world->game_over = 1;
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
  load_sprites(handler);
  w->running = 1;
  XEvent event;
  const Colors cs = create_colors(handler);
  set_color(handler, cs.green.pixel);
  int player = spawn_player(w, 320, 400, &cs);
  spawn_swarm(w, &cs);
  long start_time = get_time_micro();
  long end_time = start_time;
  while (w->running) {
    start_time = get_time_micro();
    const double dt = (double)(start_time - end_time) / 1000000.0;
    if (dt > 1) continue;

    while (XPending(handler->display)) {
      XNextEvent(handler->display, &event);
      switch (event.type) {
        case KeyPress: {
          const KeySym keysym = XLookupKeysym(&event.xkey, 0);
          if (keysym == XK_Left) w->key_left = 1;
          if (keysym == XK_Right) w->key_right = 1;
          if (keysym == XK_space) w->key_fire = 1;
          if (keysym == XK_Escape) w->key_escape = 1;
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
          if (keysym == XK_Escape) w->key_escape = 0;
          break;
        }
        default: break;
      }
    }

    system_input(w, &cs);
    if (!w->game_over) {
      system_movement(w, dt);
      system_collision(w);
      system_enemy_ai(w);
    }
    system_render(w, handler, dt);
    end_time = get_time_micro();
    long time_to_wait = TARGET_FRAME_TIME_MICRO - (end_time - start_time);
    if (time_to_wait > 0) {
      struct timespec req, rem;
      req.tv_sec = 0;
      req.tv_nsec = time_to_wait * 1000;
      nanosleep(&req, &rem);
    }
  }
  destroy_handler(handler);
  ecs_destroy(w);
}