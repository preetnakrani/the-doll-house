#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"


Entity createBackground(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID background)
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
        { TEXTURE_ASSET_ID(background),
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
    registry.helpScreens.emplace(entity);
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

Entity createDoll(RenderSystem* renderer, vec2 pos, std::vector<Attack> attackList, std::vector<Magic> magicList, int hp, int hd)
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
    health.health = hp;
    health.healthDecrement = hd;

    // Give the player an attack list, magic list, and add some default moves
    AttackList& player_attacks = registry.attackLists.emplace(entity);
    for(Attack a: attackList) {
        player_attacks.addAttack(a);
    }
//    player_attacks.addAttack("PUNCH", AttackType::NORMAL, 10);
//    player_attacks.addAttack("TAUNT", AttackType::NORMAL, 20);

    MagicList& player_magic = registry.magicLists.emplace(entity);
    for(Magic m: magicList) {
        player_magic.addMagic(m);
    }
//    player_magic.addMagicAttack("LIGHTNING", AttackType::NORMAL, 30);
//    player_magic.addMagicDefense("SHIELD", 30, 0);
//    player_magic.addMagicEffect("POISON", false, 0);

    Game& game = registry.game.emplace(entity);
    game.state = GameState::PLAYING;

    registry.players.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL_DOWN,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createBattleDoll(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 200, 100 };
    motion.collision_proof = 1;
    registry.battleDolls.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::PLAYER_TEMP,
                    // help screen effect just renders it like the help screen
              EFFECT_ASSET_ID::HELP_SCREEN,
              GEOMETRY_BUFFER_ID::SPRITE});
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
    health.healthDecrement = 0;

    // Create an enemy
    registry.enemies.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DUST_BUNNY,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}


Entity createBattleEnemy(RenderSystem* renderer, vec2 pos)
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
    motion.scale = {200, 100};
    motion.collision_proof = 1;
//    motion.scale.x *= -1; // point front to the right

    // Create and (empty) Salmon component to be able to refer to all turtles
    registry.battleEnemies.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::ENEMY_TEMP,
              EFFECT_ASSET_ID::HELP_SCREEN,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createWallBlock(vec2 pos, vec2 scale)
{
    auto entity = Entity();

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.scale = scale;
    motion.velocity = { 0.f, 0.f };

    registry.walls.emplace(entity);
    return entity;
}

Entity createBed(RenderSystem* renderer, vec2 pos, Direction dir, float angle) {
    auto entity = Entity();

    // TODO: fix rendering
    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;

    registry.players.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL_DOWN,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}


Entity createTable(RenderSystem* renderer, vec2 pos, Direction dir, float angle) {
    auto entity = Entity();

    // TODO: fix rendering
    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;

    registry.players.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL_DOWN,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createLamp(RenderSystem* renderer, vec2 pos, Direction dir, float angle) {
    auto entity = Entity();

    // TODO: fix rendering
    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;

    registry.players.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DOLL_DOWN,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Attack createAttack(std::string name, AttackType type, int damage) {
    Attack a = {};
    a.name = name;
    a.type = type;
    a.damage = damage;
    return a;
}

Magic createMagicAttack(std::string name, MagicType magicType, AttackType attackType, int damage) {
    MagicAttack m;
    m.name = name;
    m.magic_type = magicType;
    m.attack_type = attackType;
    m.damage = damage;
    return m;
}

Magic createMagicEffect(std::string name, MagicType magicType, bool isTemporary, float timer) {
    MagicEffect m;
    m.name = name;
    m.magic_type = magicType;
    m.isTemporary = isTemporary;
    m.timer = timer;
    return m;
}

Magic createMagicDefense(std::string name, MagicType magicType, int physical_defense_boost, int magic_defense_boost) {
    MagicDefense m;
    m.name = name;
    m.magic_type = magicType;
    m.physical_defense_boost = physical_defense_boost;
    m.magic_defense_boost = magic_defense_boost;
    return m;
}

Motion createMotion(vec2 scale, vec2 position, vec2 velocity, float angle, Direction dir, bool collision_proof) {
    Motion m;
    m.scale = scale;
    m.position = position;
    m.velocity = velocity;
    m.angle = angle;
    m.dir = dir;
    m.collision_proof = collision_proof;
    return m;
}

Health createHealth(int health, int healthDecrement) {
    Health h;
    h.health = health;
    h.healthDecrement = healthDecrement;
    return h;
}

GameProgress createGameProgress(int level, Health health, Motion motion, std::vector<Attack> attack, std::vector<Magic> magic) {
    GameProgress g;
    g.level = level;
    g.health = health;
    g.motion = motion;
    g.attack = attack;
    g.magic = magic;
    return g;
}