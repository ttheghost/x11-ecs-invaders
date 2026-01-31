#pragma once
#include "ecs.h"

int spawn_player(World *world, const int x, const int y);

void spawn_swarm(World *world);

void spawn_bullet(World *world, const int x, const int y);

double get_delta_time();