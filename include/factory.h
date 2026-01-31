#pragma once
#include "x11_wrapper.h"
#include "ecs.h"

int spawn_player(World *world, const int x, const int y, const Colors *cs);

void spawn_swarm(World *world, const Colors *cs);

void spawn_bullet(World *world, const int x, const int y, const Colors *cs);

double get_delta_time();