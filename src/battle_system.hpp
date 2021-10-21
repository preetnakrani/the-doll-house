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
    void process_player_turn(Turn& turn);

    void process_enemy_turn(Turn& turn, Entity enemy);

    void end_battle(Game& game, std::vector<Entity> current_enemies);

    WorldSystem* world_system;
};
