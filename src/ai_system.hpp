#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include <random>

extern std::vector<std::vector<int>> grid;

class AISystem
{
public:
	void step(float elapsed_ms);
private:
	void do_enemy_turn();
	Turn decideMove(Entity& enemy, Entity& player);
	std::vector<std::pair<int, int>> bfs(std::vector<std::vector<int>> grid, std::pair<int, int> curr, std::pair<int, int> goal);
};
