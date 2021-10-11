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

	// Handles removing components of battle window, should only be called by BattleSystem
	void destroyBattleWindow();
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

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
    Entity battle_screen;
    Entity battle_doll;
    Entity battle_enemy;
	Entity battle_menu_button_area;
	Entity battle_menu_text_area;
	Entity battle_menu_button_attack;
	Entity battle_menu_button_magic;
	Entity battle_menu_button_items;


	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
    void drawBattleWindow();

    void getScreenSize();
};
