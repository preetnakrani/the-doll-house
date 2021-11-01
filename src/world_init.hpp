#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float DOLL_BB_WIDTH = 0.4f * 64.f;
const float DOLL_BB_HEIGHT = 0.4f * 64.f;
const float ENEMY_BB_WIDTH = 0.4f * 64.f;
const float ENEMY_BB_HEIGHT = 0.4f * 64.f;

Entity createBackground(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID background);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createBattleWindow(RenderSystem *renderer, vec2 pos);

Entity createBattleMenuItem(RenderSystem *renderer, vec2 pos, BattleMenuItemType item_type, TEXTURE_ASSET_ID texture_id);

Entity createEnemy(RenderSystem *renderer, vec2 pos);

Entity createDoll(RenderSystem *renderer, vec2 pos, vec2 frameBufferSize, std::vector<Attack> attackList, int hp = 100, int hd = 0);

Entity createBattleDoll(RenderSystem *renderer, vec2 pos);

Entity createBattleEnemy(RenderSystem *renderer, vec2 pos);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createPopUpWindow(RenderSystem *renderer, vec2 pos);

Entity createMenuButton(RenderSystem *renderer, vec2 pos);

Entity createMenuOverlay(RenderSystem *renderer, vec2 pos);

Entity createTutorial(RenderSystem *renderer, vec2 pos);

//Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createWallBlock(vec2 pos, vec2 scale);

Entity createBed(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Entity createTable(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Entity createLamp(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Attack createAttack(AttackName name, AttackType type, int damage);

//MagicAttack createMagicAttack(std::string name, MagicType magicType, AttackType attackType, int damage);
//
//MagicEffect createMagicEffect(std::string name, MagicType magicType, bool isTemporary, float timer);
//
//MagicDefense createMagicDefense(std::string name, MagicType magicType, int physical_defense_boost, int magic_defense_boost);

GameProgress createGameProgress(int level, Health health, Motion motion, std::vector<Attack> attack);

Health createHealth(int health = 100, int healthDecrement = 0);

Motion createMotion(vec2 scale, vec2 position, vec2 velocity, float angle, Direction dir, bool collision_proof);

Entity createClickableArea(vec2 leftTop, int width, int height);

Entity createLine(vec2 position, vec2 scale);

void createBox(vec2 centrePosition, vec2 verticalLineScale, vec2 horizontalLineScale, int boxWidth, int boxHeight);
