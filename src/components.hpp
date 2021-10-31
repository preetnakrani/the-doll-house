#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <map>

// Player component
struct Player
{

};

enum class EnemyName {
	DUST_BUNNY = 0,
};

struct Enemy {
	EnemyName name;
};

struct AnimatedSprite {
	bool paused = true;
	// maps how many frames are drawn for each animation type in the sprite sheet
	// animation type is the key, and the number of frames is the value
	std::map<int, int> num_frames_per_type = {};

	float frame_height = 0.25;
	float frame_width = 0.25;

	float animation_speed_ms = 250;
	float ms_since_last_update = 0;

	int current_type = 0; // y axis of sprite sheet
	int current_frame = 0; // x axis of sprite sheet
};

struct Wall {

};

struct Health {
    int health = 0;
    int healthDecrement = 0;
	  int maxHP = 0;
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

enum class BattleState {
	NONE = 1,
	START = 2,
	PLAYER_TURN = 3,
	ENEMY_TURN = 4,
	WON = 5,
	LOST = 6,
};

enum class EnemyState {
	NONE = 1,
	HEALTHY = 2,
	DAMAGED = 3,
	NEAR_DEATH = 4,
	BOLD = 5,
	THREATENED = 6,
	TAUNTED = 7,
};

struct Status {
	bool NONE = true;
	bool TAUNTED = false;
	bool POISONED = false;
	bool SHIELDED = false;
	int timer = 0;

};

// An enemy that the player is currently engaging in battle with
struct CurrentEnemy {
	EnemyState enemyState = EnemyState::HEALTHY;
};


enum class MoveType {
	NONE = 0,
	ATTACK = 1,
	MAGIC = 2,
	ITEM = 3,
};

enum class AttackName {
	NONE = 0,
	PUNCH = 1,
	SNEEZE = 2, // dust bunny
};

enum class AttackType {
    NORMAL = 0,
    NOTNORMAL = 1,
};

struct Attack {
	AttackName name;
    AttackType type = AttackType::NORMAL;
    int damage = 0;

	std::string nameAsString() {
		switch (this->name) {
			case AttackName::PUNCH :
				return "PUNCH";
			case AttackName::SNEEZE :
				return "SNEEZE";
			default:
				return "";
		}
	}
};

struct AttackList {
	std::vector<Attack> available_attacks;

	void addAttack(AttackName name, AttackType type, int damage) {
		Attack attack = {};
		attack.name = name;
		attack.type = type;
		attack.damage = damage;
		available_attacks.push_back(attack);
	}

	bool hasAttack(AttackName name) {
		// https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		auto iterator = find_if(available_attacks.begin(), available_attacks.end(), [&name](const Attack& attack) {
			return attack.name == name;
		});

		return iterator != available_attacks.end();
	}

	Attack getAttack(AttackName name) {
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

enum class MagicName {
	NONE = 0,
	LIGHTNING = 1,
	SHIELD = 2,
	POISON = 3,
	TAUNT = 4,
};

enum class MagicType {
	ATTACK = 0,
	DEFENSE = 1,
	STATUS_EFFECT = 2, // Magic that changes the target's status
	NONE = 3,
};

struct Magic {
	MagicName name;
	
	std::string nameAsString() {
		switch (this->name) {
			case MagicName::LIGHTNING :
				return "LIGHTNING";
			case MagicName::SHIELD :
				return "SHIELD";
			case MagicName::POISON :
				return "POISON";
			case MagicName::TAUNT:
				return "TAUNT";
			default:
				return "";
		}
	}
	MagicType magicType = MagicType::NONE;
	AttackType attack_type = AttackType::NORMAL;
	int damage = 0;
	int physical_defense_boost = 0;
	int magic_defense_boost = 0;
	int timer = 0;
	int countdown = 0;
	bool isTemporary;

};


struct MagicList {
	std::vector<Magic> available_magic;

	void addMagicAttack(MagicName name, AttackType attack_type, int damage) {
		Magic magic_attack = {};
		magic_attack.name = name;
		magic_attack.magicType = MagicType::ATTACK;
		magic_attack.attack_type = attack_type;
		magic_attack.damage = damage;
		available_magic.push_back(magic_attack);
	}

	void addMagicDefense(MagicName name, int physical_defense_boost, int magic_defense_boost, int timer) {
		Magic magic_defense = {};
		magic_defense.name = name;
		magic_defense.magicType = MagicType::DEFENSE;
		magic_defense.physical_defense_boost = physical_defense_boost;
		magic_defense.magic_defense_boost = magic_defense_boost;
		magic_defense.timer = timer;
		available_magic.push_back(magic_defense);
	}

	void addMagicEffect(MagicName name, bool isTemporary, float timer) {
		Magic magic_effect = {};
		magic_effect.name = name;
		magic_effect.magicType = MagicType::STATUS_EFFECT;
		magic_effect.isTemporary = isTemporary;
		magic_effect.timer = timer;
		available_magic.push_back(magic_effect);
	}

	bool hasMagic(MagicName name) {
		auto iterator = find_if(available_magic.begin(), available_magic.end(), [&name](const Magic& magic) {
			return magic.name == name;
			});

		return iterator != available_magic.end();
	}

	Magic getAttack(MagicName name) {
		auto iterator = find_if(available_magic.begin(), available_magic.end(), [&name](const Magic& magic) {
			return magic.name == name;
			});

		if (iterator != available_magic.end()) {
			return *iterator;
		}
		else {
			return {};
		}
	}
};

enum class GameItemName {
	NONE = 0,
	HEALING_POTION = 1,
};

struct GameItem {
	GameItemName item_name;
    float timer = 0;
    int health = 0;
    int speed = 0;
    bool enemyRepel = false;
    bool timed = false;
};

// Register this with a player or enemy so their turns can be applied in the Battle system
struct Turn {
	MoveType move_type = MoveType::NONE;
	AttackName attack_name = AttackName::NONE;
	MagicName magic_name = MagicName::NONE;
	GameItemName item_name = GameItemName::NONE;

	Turn(AttackName attack_name) {
		this->move_type = MoveType::ATTACK;
		this->attack_name = attack_name;
	}

	Turn(MagicName magic_name) {
		this->move_type = MoveType::MAGIC;
		this->magic_name = magic_name;
	}

	Turn(GameItemName item_name) {
		this->move_type = MoveType::ITEM;
		this->item_name = item_name;
	}
};

struct TutorialTimer {
	float timePerSprite = 7000.f;
	bool tutorialCompleted = false;
	int tutorialIndex = 0;
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
	BattleState battle_state = BattleState::NONE;
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
	DOLL = DUST_BUNNY + 1,
	ENEMY_ONE = DOLL + 1,
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
	TEXTURED_ANIMATION = TEXTURED + 1,
	BLUR = TEXTURED_ANIMATION + 1,
	REBLUR = BLUR + 1,
	HELP_SCREEN = REBLUR + 1,
//    BATTLE_SCREEN = HELP_SCREEN + 1,
	EFFECT_COUNT = HELP_SCREEN + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	SPRITE_ANIMATION = SPRITE + 1,
	DEBUG_LINE = SPRITE_ANIMATION + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	HELP_SCREEN = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = HELP_SCREEN + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
