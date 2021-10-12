#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "physics_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();
    PhysicsSystem s;
	// Creates a window
	GLFWwindow* create_window(int width, int height);

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

    void setRenderRequests();
	void progressTutorial(float elapsed_ms_since_last_update);
	void escapeTutorial(bool isComplete);

	void swap_help_screen();

	// Handles removing components of battle window, should only be called by BattleSystem
	void destroyBattleWindow();
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mods);
	void openMenuOverlayScreen();
	void closeMenuOverlayScreen();
	// restart level
	void restart_game();

	// creates the battle window and all its UI elements
	void drawBattleWindow();

	// helper function that scales UI assets
	void scaleUIAsset(Entity entity, vec2 originalDimensions, float scaleFactor);

	// handles a button click from the battle screen
	void handleBattleScreenButtonClick(BattleMenuItemType button_clicked);

	// clears the player move options from the battle menu's text area
	void clearBattlePlayerMoveButtons();

	// returns which button was clicked on the battle screen, if any
	BattleMenuItemType getBattleScreenButtonClicked(double x, double y);

	// helper function that returns whether a mouse click on coordinates x, y occured in a given (rectangular) region
	// where top_left_coords is the coordinate of the region's top left corner
	bool isClickInRegion(double x, double y, vec2 top_left_coords, double height, double width);

	void getScreenSize();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
    float player_speed = 100.f;
    float next_enemy_spawn;
	Entity player_doll;
	Entity background;
	Entity helpScreen;
	Entity menuButton;
	Entity menuOverlay;
	Entity tutorialScreen;

	// Entities related to rendering battle screen
    Entity battle_screen;
    Entity battle_doll;
    Entity battle_enemy;
	Entity battle_menu_button_area;
	Entity battle_menu_text_area;
	Entity battle_menu_button_attack;
	Entity battle_menu_button_magic;
	Entity battle_menu_button_items;
	Entity battle_menu_button_learn;
	Entity battle_menu_button_go;
	Entity punch_button;

	// Other variables related to battle screen
	BattleMenuItemType selected_move_menu; // Either attack, magic, or items

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
