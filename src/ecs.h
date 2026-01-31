#pragma once
#include <stdint.h>

#define MAX_ENTITIES 512

typedef enum {
  COMPONENT_NONE      = 0,
  COMPONENT_POS       = 1 << 0,
  COMPONENT_VEL       = 1 << 1,
  COMPONENT_SPRITE    = 1 << 2,
  COMPONENT_COLLIDER  = 1 << 3,
  COMPONENT_PLAYER    = 1 << 4,
  COMPONENT_ENEMY     = 1 << 5,
  COMPONENT_BULLET    = 1 << 6
} ComponentType;

typedef struct {
  uint32_t mask[MAX_ENTITIES];

  float x[MAX_ENTITIES];
  float y[MAX_ENTITIES];
  float vx[MAX_ENTITIES];
  float vy[MAX_ENTITIES];
  int width[MAX_ENTITIES];
  int height[MAX_ENTITIES];

  int score;
  int game_over;
  int running;
  int screen_width;
  int screen_height;
} World;

World* ecs_init(int screen_width, int screen_height);

int ecs_create_entity(World* world);

void ecs_destroy_entity(World* w, int id);

void ecs_destroy(World *world);