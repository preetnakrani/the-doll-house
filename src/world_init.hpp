#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float DOLL_BB_WIDTH = 0.4f * 64.f;
const float DOLL_BB_HEIGHT = 0.4f * 64.f;
const float ENEMY_BB_WIDTH = 0.4f * 64.f;
const float ENEMY_BB_HEIGHT = 0.4f * 64.f;

Entity createEnemy(RenderSystem *renderer, vec2 pos);

Entity createDoll(RenderSystem *renderer, vec2 pos);
