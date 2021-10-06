#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class BattleSystem
{
public:
    // run the battle 
    void handle_battle();

private:
    void init();

    //void end_battle(Game& game, vector<Entity> current_enemies); // causes an error right now - commenting temporarily - Naoreen
};
