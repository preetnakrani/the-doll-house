#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{

};

struct Enemy {

};

// An enemy that the player is currently engaging in battle with
struct CurrentEnemy {

};

struct Health {
    int health = 0;
    int healthDecrement  = 0;
};

enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
    Direction dir = Direction::DOWN;
	int collision_proof = 0;
};

struct Momentum {
    float count_ms = 1000;
    float decrement = 0;
    vec2 velocity;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

enum class AttackType {
    NORMAL = 0,
    NOTNORMAL = 1
};

struct Attack {
    std::string name = "";
    AttackType type = AttackType::NORMAL;
    int damage = 0;
};

struct GameItem {
    float timer = 0;
    int health = 0;
    int speed = 0;
    bool enemyRepel = false;
    bool timed = false;
};

// Idea - could be used to apply to the entity who has the currently active turn
struct Turn {
	// float timer = 0;
	int key;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

enum class GameState {
    PLAYING = 0,
    GAMEOVER = 1,
    STORYSCREEN = 2,
    BOSS = 3,
    BATTLE = 4
};

// Sets the brightness of the screen
struct Game
{
	GameState state = GameState::PLAYING;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Backgrounds
struct Background
{
	int blur_state = 0;

};

// Help screen
struct HelpScreen {
    int order = 0;
};

// Mesh data structure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	DOLL_UP = 0,
	DOLL_RIGHT = DOLL_UP + 1,
	DOLL_DOWN = DOLL_RIGHT + 1,
	DOLL_LEFT = DOLL_DOWN + 1,
	ENEMY_ONE = DOLL_LEFT + 1,
    ENEMY_TWO = ENEMY_ONE + 1,
    ENEMY_THREE = ENEMY_TWO + 1,
    BOSS = ENEMY_THREE + 1,
	TABLE = BOSS + 1,
    CHAIR = TABLE + 1,
    BED = CHAIR + 1,
    HEALTH_ITEM = BED + 1,
    SPEED_ITEM  = HEALTH_ITEM + 1,
    ENEMY_REPEL = SPEED_ITEM + 1,
    BACKGROUND = ENEMY_REPEL + 1,
    HELP_PRESS_A = BACKGROUND + 1,
    HELP_PRESS_D = HELP_PRESS_A + 1,
    HELP_PRESS_S = HELP_PRESS_D + 1,
    HELP_PRESS_W = HELP_PRESS_S + 1,
    HELP_BATTLE_INFO = HELP_PRESS_W + 1,
	TEXTURE_COUNT = HELP_BATTLE_INFO + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	SALMON = COLOURED + 1,
	TEXTURED = SALMON + 1,
	WATER = TEXTURED + 1,
	HELP_SCREEN = WATER + 1,
	EFFECT_COUNT = HELP_SCREEN + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

