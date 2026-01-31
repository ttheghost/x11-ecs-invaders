#include "factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int spawn_player(World *world, const int x, const int y) {
  const int p = ecs_create_entity(world);
  if (p == -1) {
    fprintf(stderr, "Failed to create player entity\n");
    exit(-1);
  }
  world->mask[p] = COMPONENT_PLAYER | COMPONENT_POS | COMPONENT_SPRITE | COMPONENT_VEL;
  world->x[p] = x; world->y[p] = y;
  world->width[p] = 40; world->height[p] = 10;
  return p;
}

void spawn_swarm(World *world) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 11; j++) {
      const int a = ecs_create_entity(world);
      if (a == -1) {
        fprintf(stderr, "Failed to create alien entity\n");
        exit(-1);
      }
      world->mask[a] = COMPONENT_ENEMY | COMPONENT_SPRITE | COMPONENT_POS | COMPONENT_COLLIDER | COMPONENT_VEL;
      world->x[a] = 40.0 * j + 20; world->y[a] = 40 * i + 20;
      world->vy[a] = 5; world->vx[a] = 0;
      world->width[a] = 30; world->height[a] = 30;
    }
  }
}

void spawn_bullet(World *world, const int x, const int y) {
  const int b = ecs_create_entity(world);
  if (b == -1) {
    printf("Failed to create entity\n");
    exit(1);
  }
  world->mask[b] = COMPONENT_SPRITE | COMPONENT_VEL | COMPONENT_POS | COMPONENT_BULLET | COMPONENT_COLLIDER;
  world->x[b] = x;
  world->y[b] = y;
  world->vx[b] = 0;
  world->vy[b] = -500;
  world->width[b] = 6;
  world->height[b] = 10;
}

double get_delta_time() {
  static double last = 0;
  struct timespec cur;
  timespec_get(&cur, TIME_UTC);
  const double now = (double)cur.tv_sec + (double)cur.tv_nsec / 1e9;
  const double delta = now - last;
  last = now;
  return delta;
}