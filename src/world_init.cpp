#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"


Entity createBackground(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.collision_proof = 1;
    motion.scale.y = (pos.y * 2) / mesh.original_size.y; // fit to screen
    motion.scale.x = (pos.x * 2) / mesh.original_size.x; // fit to screen

    registry.backgrounds.emplace(entity);

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::BACKGROUND,
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

// this entity can be blurred when blur activates
Entity createHelpWindow(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();
    // width: height = 2 : 1
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
//    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::HELP_PRESS_A,
              EFFECT_ASSET_ID::HELP_SCREEN,
              GEOMETRY_BUFFER_ID::HELP_SCREEN});
    return entity;
}

Entity createMenuButton(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::MENU_BUTTON,
              EFFECT_ASSET_ID::HELP_SCREEN,
              GEOMETRY_BUFFER_ID::HELP_SCREEN});
    return entity;
}

Entity createMenuOverlay(RenderSystem *renderer, vec2 pos) {
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
    registry.helpScreens.emplace(entity);
//    registry.renderRequests.insert(
//            entity,
//            { TEXTURE_ASSET_ID::MENU_OVERLAY,
//              EFFECT_ASSET_ID::HELP_SCREEN,
//              GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createTutorial(RenderSystem* renderer, vec2 pos) {
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
    registry.tutorialTimer.emplace(entity);
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TUTORIAL_ONE,
          EFFECT_ASSET_ID::HELP_SCREEN,
          GEOMETRY_BUFFER_ID::HELP_SCREEN });
    return entity;
}

Entity createBattleWindow(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale.y = (pos.y * 2) / mesh.original_size.y; // fit to screen
    motion.scale.x = (pos.x * 2) / mesh.original_size.x; // fit to screen
    motion.collision_proof = 1;
    registry.battleScreens.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::BATTLE_BACKGROUND_1,
        // help screen effect just renders it like the help screen
        EFFECT_ASSET_ID::HELP_SCREEN,
        GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity createBattleMenuItem(RenderSystem* renderer, vec2 pos, BattleMenuItemType item_type, TEXTURE_ASSET_ID texture_id)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;

    if (item_type == BattleMenuItemType::BUTTON_AREA || item_type == BattleMenuItemType::TEXT_AREA) {
        BattleMenu& battle_menu = registry.battleMenus.emplace(entity);
        battle_menu.menu_type = item_type;
    }
    else if (
        item_type == BattleMenuItemType::ATTACK_BUTTON ||
        item_type == BattleMenuItemType::MAGIC_BUTTON ||
        item_type == BattleMenuItemType::ITEMS_BUTTON ||
        item_type == BattleMenuItemType::LEARN_BUTTON ||
        item_type == BattleMenuItemType::GO_BUTTON)
    {
        BattleMenuButton& battle_menu_button = registry.battleMenuButtons.emplace(entity);
        battle_menu_button.button_type = item_type;
    }
    else if (item_type == BattleMenuItemType::ATTACK_PUNCH) {
        BattleMenuPlayerMove& player_move = registry.battleMenuPlayerMoves.emplace(entity);
    }

    registry.renderRequests.insert(
        entity,
        { texture_id,
          EFFECT_ASSET_ID::HELP_SCREEN,
          GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}
Entity createDoll(RenderSystem* renderer, vec2 pos, vec2 frameBufferSize)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;

    Health& health = registry.health.emplace(entity);
    health.health = 100;
    health.maxHP = 100;
    health.healthDecrement = 0;

    // Give the player an attack list, magic list, and add some default moves
    AttackList& player_attacks = registry.attackLists.emplace(entity);
    player_attacks.addAttack(AttackName::PUNCH, AttackType::NORMAL, 10);

    MagicList& player_magic = registry.magicLists.emplace(entity);
    player_magic.addMagicAttack(MagicName::LIGHTNING, AttackType::NORMAL, 30);
    player_magic.addMagicDefense(MagicName::SHIELD, 30, 0, 2);
    player_magic.addMagicEffect(MagicName::POISON, false, 3);
    player_magic.addMagicEffect(MagicName::TAUNT, false, 2);

    Game& game = registry.game.emplace(entity);
    game.state = GameState::PLAYING;
    registry.players.insert(entity, {(int)frameBufferSize.x, (int)frameBufferSize.y});
    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = true;
    animated_sprite.num_frames_per_type = { { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 } };

    registry.status.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL,
              EFFECT_ASSET_ID::TEXTURED_ANIMATION,
              GEOMETRY_BUFFER_ID::SPRITE_ANIMATION });

    return entity;
}

Entity createBattleDoll(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE_ANIMATION);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 100, 100 };
    motion.collision_proof = 1;
    registry.battleDolls.emplace(entity);
    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = { { 0, 2 } };
    animated_sprite.animation_speed_ms = 750;

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL,
              EFFECT_ASSET_ID::TEXTURED_ANIMATION,
              GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});
    return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.scale.x *= -1; // point front to the right

    Health& health = registry.health.emplace(entity);
    health.health = 100;
    health.maxHP = 100;
    health.healthDecrement = 0;

    // Give it a status
    registry.status.emplace(entity);
    // TEMPORARY - We will want to give different enemies different attacks/moves, but for now I'm just assuming dust bunny here - Naoreen
    AttackList& enemy_attacks = registry.attackLists.emplace(entity);
    // One attack move: Sneeze
    enemy_attacks.addAttack(AttackName::SNEEZE, AttackType::NORMAL, 5);
    // One Healing Potion
    GameItem& item = registry.gameItems.emplace(entity);
    item.item_name = GameItemName::HEALING_POTION;
    item.health = 0.3 * health.health;
    // One status effect debuff
    MagicList& magic = registry.magicLists.emplace(entity);
    magic.addMagicEffect(MagicName::TAUNT, true, 2);
    // One defense effect buff 
    magic.addMagicDefense(MagicName::SHIELD, 5, 0, 2);


    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = { { 0, 2 } };
    animated_sprite.animation_speed_ms = 750;

    // Create an enemy
    Enemy& enemy = registry.enemies.emplace(entity);
    enemy.name = EnemyName::DUST_BUNNY;

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DUST_BUNNY,
              EFFECT_ASSET_ID::TEXTURED_ANIMATION,
              GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});

    return entity;
}

Entity createBattleEnemy(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE_ANIMATION);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos + vec2(0, 25);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = {50, 50};
    motion.collision_proof = 1;
//    motion.scale.x *= -1; // point front to the right

    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = { { 0, 2 } };
    animated_sprite.animation_speed_ms = 750;

    registry.battleEnemies.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DUST_BUNNY,
              EFFECT_ASSET_ID::TEXTURED_ANIMATION,
              GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});

    return entity;
}

Entity createWallBlock(vec2 pos)
{
    auto entity = Entity();

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.scale.x = 100.f;
    motion.scale.y = 100.f;
    motion.velocity = { 0.f, 0.f };

    registry.walls.emplace(entity);
    return entity;
}

//Entity createClickableArea(vec2 topLeft, int width, int height) {
//    auto entity = Entity();
//    ClickableArea& ca = registry.clickableArea.emplace(entity);
//    ca.topLeft = topLeft;
//    ca.width = width;
//    ca.height = height;
//    return entity;
//}


Entity createLine(vec2 position, vec2 scale)
{
    Entity entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
              EFFECT_ASSET_ID::PEBBLE,
              GEOMETRY_BUFFER_ID::DEBUG_LINE });
    // Create motion
    Motion& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;
    motion.scale = scale;
    registry.debugComponents.emplace(entity);
    return entity;
}

void createBox(vec2 centrePosition, vec2 verticalLineScale, vec2 horizontalLineScale, int boxWidth, int boxHeight) {
    int w = boxWidth / 2;
    int h = boxHeight / 2;
    createLine({centrePosition.x - w, centrePosition.y}, verticalLineScale);
    createLine({centrePosition.x + w, centrePosition.y}, verticalLineScale);
    createLine({centrePosition.x, centrePosition.y - h}, horizontalLineScale);
    createLine({centrePosition.x, centrePosition.y + h}, horizontalLineScale);
}

void createBoxWithTopLeft (vec2 topLeftCoord, vec2 verticalLineScale, vec2 horizontalLineScale, int boxWidth, int boxHeight) {
    createLine({topLeftCoord.x, topLeftCoord.y}, verticalLineScale);
    createLine({topLeftCoord.x + boxWidth, topLeftCoord.y}, verticalLineScale);
    createLine({topLeftCoord.x, topLeftCoord.y}, horizontalLineScale);
    createLine({topLeftCoord.x, topLeftCoord.y + boxHeight}, horizontalLineScale);
}
