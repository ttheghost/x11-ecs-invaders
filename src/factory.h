#pragma once
#include "ecs.h"

int spawn_player(World *world, const int x, const int y);

void spawn_swarm(World *world);

double get_delta_time();