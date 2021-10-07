#pragma once

#include <vector>

#include "render_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class BattleSystem
{
public:
    void init(RenderSystem* renderer_arg);

    // run the battle 
    void handle_battle();

private:
    //void end_battle(Game& game, vector<Entity> current_enemies); // causes an error right now - commenting temporarily - Naoreen
    RenderSystem *renderer;
};
