#pragma once

#include <vector>

#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class BattleSystem
{
public:
    void init(WorldSystem* world_system_arg);

    // run the battle 
    void handle_battle();

private:
    void end_battle(Game& game, std::vector<Entity> current_enemies);

    WorldSystem* world_system;
};
