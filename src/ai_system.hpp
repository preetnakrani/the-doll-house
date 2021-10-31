#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include <random>

class AISystem
{
public:
	void step(float elapsed_ms);
private:
	void do_enemy_turn();
	Turn decideMove(Entity& enemy, Entity& player);

};
