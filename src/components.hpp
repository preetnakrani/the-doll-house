#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{
    int fBWidth;
    int fBHeight;
};

struct Enemy {

};

// An enemy that the player is currently engaging in battle with
struct CurrentEnemy {

};

struct Wall {

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

struct ClickableArea{
    vec2 topLeft;
    int width;
    int height;
};

struct AttackList {
	std::vector<Attack> available_attacks;

	void addAttack(std::string name, AttackType type, int damage) {
		Attack attack = {};
		attack.name = name;
		attack.type = type;
		attack.damage = damage;
		available_attacks.push_back(attack);
	}

	bool hasAttack(std::string name) {
		// https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		auto iterator = find_if(available_attacks.begin(), available_attacks.end(), [&name](const Attack& attack) {
			return attack.name == name;
		});

		return iterator != available_attacks.end();
	}

	Attack getAttack(std::string name) {
		auto iterator = find_if(available_attacks.begin(), available_attacks.end(), [&name](const Attack& attack) {
			return attack.name == name;
			});

		if (iterator != available_attacks.end()) {
			return *iterator;
		}
		else {
			return {};
		}
	}
};

enum class MagicType {
	ATTACK = 0,
	DEFENSE = 1,
	STATUS_EFFECT = 2 // Magic that changes the target's status
};

struct Magic {
	std::string name = "";
};

struct MagicAttack : Magic {
	MagicType magic_type = MagicType::ATTACK;
	AttackType attack_type = AttackType::NORMAL;
	int damage = 0;
};

struct MagicDefense : Magic {
	MagicType magic_type = MagicType::DEFENSE;
	int physical_defense_boost = 0;
	int magic_defense_boost = 0;
};

struct MagicEffect : Magic {
	MagicType magic_type = MagicType::STATUS_EFFECT;
	bool isTemporary;
	float timer; // set to 0 if no timer needed (permanent effect)
};

struct MagicList {
	std::vector<Magic> available_magic;

	void addMagicAttack(std::string name, AttackType attack_type, int damage) {
		MagicAttack magic_attack = {};
		magic_attack.name = name;
		magic_attack.attack_type = attack_type;
		magic_attack.damage = damage;
	}

	void addMagicDefense(std::string name, int physical_defense_boost, int magic_defense_boost) {
		MagicDefense magic_defense = {};
		magic_defense.name = name;
		magic_defense.physical_defense_boost = physical_defense_boost;
		magic_defense.magic_defense_boost = magic_defense_boost;
	}

	void addMagicEffect(std::string name, bool isTemporary, float timer) {
		MagicEffect magic_effect = {};
		magic_effect.name = name;
		magic_effect.isTemporary = isTemporary;
		magic_effect.timer = timer;
	}
};

struct GameItem {
    float timer = 0;
    int health = 0;
    int speed = 0;
    bool enemyRepel = false;
    bool timed = false;
};

struct TutorialTimer {
	float timePerSprite = 7000.f;
	bool tutorialCompleted = false;
	int tutorialIndex = 0;
};

// Idea - could be used to apply to the entity who has the currently active turn
struct Turn {
	// float timer = 0;
	std::string move;
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
    BATTLE = 4,
    MENUOVERLAY = 5,
    TUTORIAL = 6
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

struct BattleScreen {

};

struct BattleDoll {

};

struct BattleEnemy {

};

enum class BattleMenuItemType {
	NONE = 0, // Do not assign this to any assets
	BUTTON_AREA = 1,
	TEXT_AREA = 2,
	ATTACK_BUTTON = 3,
	MAGIC_BUTTON = 4,
	ITEMS_BUTTON = 5,
	LEARN_BUTTON = 6, // User can click on this to learn more about the move they selected
	GO_BUTTON = 7, // It's more like a "submit" button, but I wanted to save space on the UI - Naoreen
	ATTACK_PUNCH = 8,
};

struct BattleMenu {
	BattleMenuItemType menu_type;
};

struct BattleMenuButton {
	BattleMenuItemType button_type;
};

struct BattleMenuPlayerMove {
	BattleMenuItemType move_type;
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
	DUST_BUNNY = 0,
	DOLL_UP = DUST_BUNNY + 1,
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
	STAY_AWAY = HELP_PRESS_W + 1,
	MENU_BUTTON = STAY_AWAY + 1,
	MENU_OVERLAY = MENU_BUTTON + 1,
	TUTORIAL_ONE = MENU_OVERLAY + 1,
	TUTORIAL_TWO = TUTORIAL_ONE + 1,
	TUTORIAL_THREE = TUTORIAL_TWO + 1,
	TUTORIAL_FOUR = TUTORIAL_THREE + 1,
	TUTORIAL_FIVE = TUTORIAL_FOUR + 1,
	BATTLE_BACKGROUND_1 = TUTORIAL_FIVE + 1,
	BATTLE_MENU_BUTTON_AREA = BATTLE_BACKGROUND_1 + 1,
	BATTLE_MENU_TEXT_AREA = BATTLE_MENU_BUTTON_AREA + 1,
	BATTLE_MENU_BUTTON_ATTACK = BATTLE_MENU_TEXT_AREA + 1,
	BATTLE_MENU_BUTTON_MAGIC = BATTLE_MENU_BUTTON_ATTACK + 1,
	BATTLE_MENU_BUTTON_ITEMS = BATTLE_MENU_BUTTON_MAGIC + 1,
	BATTLE_MENU_BUTTON_LEARN = BATTLE_MENU_BUTTON_ITEMS + 1,
	BATTLE_MENU_BUTTON_GO = BATTLE_MENU_BUTTON_LEARN + 1,
	ATTACK_OPTIONS_PUNCH = BATTLE_MENU_BUTTON_GO + 1,
	ATTACK_OPTIONS_PUNCH_SELECTED = ATTACK_OPTIONS_PUNCH + 1,
	PLAYER_TEMP = ATTACK_OPTIONS_PUNCH_SELECTED + 1,
	ENEMY_TEMP = PLAYER_TEMP + 1,
	TEXTURE_COUNT = ENEMY_TEMP + 1,
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	SALMON = COLOURED + 1,
	TEXTURED = SALMON + 1,
	BLUR = TEXTURED + 1,
	REBLUR = BLUR + 1,
	HELP_SCREEN = REBLUR + 1,
    DRAW_TO_SCREEN = HELP_SCREEN + 1,
    PEBBLE = DRAW_TO_SCREEN + 1,
	EFFECT_COUNT = PEBBLE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	HELP_SCREEN = SCREEN_TRIANGLE + 1,
	PEBBLE = HELP_SCREEN + 1,
	GEOMETRY_COUNT = PEBBLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
