#include "ecs.h"
#include <stdlib.h>

World* ecs_init(const int screen_width, const int screen_height) {
  World* world = malloc(sizeof(World));
  world->screen_width = screen_width;
  world->screen_height = screen_height;
  return world;
}

int ecs_create_entity(World* world) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (world->mask[i] == COMPONENT_NONE) {
      world->x[i] = 0;
      world->y[i] = 0;
      world->vx[i] = 0;
      world->vy[i] = 0;
      world->width[i] = 0;
      world->height[i] = 0;
      return i;
    }
  }
  return -1;
}

void ecs_destroy_entity(World* w, int id) {
  if (id < MAX_ENTITIES && id >= 0) {
    w->mask[id] = COMPONENT_NONE;
  }
}

void ecs_destroy(World *world) {
  free(world);
}