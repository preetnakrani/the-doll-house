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

Entity createDoll(RenderSystem* renderer, vec2 pos)
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
    // motion.scale.x *= 1; // point front to the right

    Health& health = registry.health.emplace(entity);
    health.health = 100;
    health.healthDecrement = 0;

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
            { TEXTURE_ASSET_ID::DUST_BUNNY, // TEXTURE_COUNT indicates that no txture is needed
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}
