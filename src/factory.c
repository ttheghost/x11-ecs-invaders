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
  world->mask[p] = COMPONENT_PLAYER | COMPONENT_POS | COMPONENT_SPRITE;
  world->x[p] = x; world->y[p] = y;
  world->width[p] = 40; world->height[p] = 10;
  return p;
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