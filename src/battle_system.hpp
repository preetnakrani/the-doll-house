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
    //void end_battle(Game& game, vector<Entity> current_enemies); // causes an error right now - commenting temporarily - Naoreen
    WorldSystem* world_system;
};
