#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float DOLL_BB_WIDTH = 0.4f * 64.f;
const float DOLL_BB_HEIGHT = 0.4f * 64.f;
const float ENEMY_BB_WIDTH = 0.4f * 64.f;
const float ENEMY_BB_HEIGHT = 0.4f * 64.f;

Entity createBackground(RenderSystem* renderer, vec2 pos);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createBattleWindow(RenderSystem *renderer, vec2 pos);

Entity createBattleMenuItem(RenderSystem* renderer, vec2 pos, BattleMenuItemType item_type, TEXTURE_ASSET_ID texture_id);

Entity createEnemy(RenderSystem *renderer, vec2 pos);

Entity createDoll(RenderSystem *renderer, vec2 pos, vec2 frameBufferSize);

Entity createBattleDoll(RenderSystem* renderer, vec2 pos);

Entity createBattleEnemy(RenderSystem* renderer, vec2 pos);

Entity createHelpWindow(RenderSystem* renderer, vec2 pos);

Entity createMenuButton(RenderSystem* renderer, vec2 pos);

Entity createMenuOverlay(RenderSystem* renderer, vec2 pos);

Entity createTutorial(RenderSystem* renderer, vec2 pos);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createWallBlock(vec2 pos);

Entity  createClickableArea(vec2 leftTop, int width, int height);

Entity createLine(vec2 position, vec2 scale);

void createBox(vec2 centrePosition, vec2 verticalLineScale, vec2 horizontalLineScale, int boxWidth, int boxHeight);