// Header
#include "world_system.hpp"
#include "world_init.hpp"
// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include "battle_system.hpp"
#include <iostream>

// Game configuration
const size_t MAX_ENEMY = 5;
const size_t ENEMY_DELAY_MS = 2000 * 3;
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0)
    , next_enemy_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window(int width, int height) {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Salmon Game Assignment", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_click_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_click_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("salmon_dead.wav").c_str(),
			audio_path("salmon_eat.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    SCREEN_HEIGHT = screen_height;
    SCREEN_WIDTH = screen_width;

	// Set all states to default
    restart_game();
}

//void WorldSystem::getScreenSize() {
//    int screen_width, screen_height;
//    glfwGetFramebufferSize(window, &screen_width, &screen_height);
//    SCREEN_HEIGHT = screen_height;
//    SCREEN_WIDTH = screen_width;
//}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Get the screen dimensions
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
    SCREEN_HEIGHT = screen_height;
    SCREEN_WIDTH = screen_width;

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
		    registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

    // Spawning new enemy
	
    next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
    if (registry.enemies.components.size() < MAX_ENEMY && next_enemy_spawn < 0.f && registry.game.has(player_doll) && registry.game.get(player_doll).state == GameState::PLAYING ) {
        // Reset timer
        next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);

        vec2 position =
                vec2(50.f + uniform_dist(rng) * (screen_width - 100.f),
					50.f + screen_height / 3 + uniform_dist(rng) * (2 * screen_height/3 - 100.f));

        vec2 bounding = vec2(10.f, 10.f);
        if (!s.checkFakeCollision(position, bounding)) {
			Entity new_enemy = createEnemy(renderer, position);
			registry.motions.get(new_enemy).scale = registry.motions.get(new_enemy).scale * float(screen_width / 8);
			//createEnemy(renderer, position);
        }
    }

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// create a background
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	background = createBackground(renderer, { screen_width / 2.f, screen_height / 2.f });

	// Create a new doll
	player_doll = createDoll(renderer, { screen_width / 5, screen_height/3 });
	Motion& motion = registry.motions.get(player_doll);
	motion.scale = motion.scale * float(screen_width / 8);


    player_speed = 200.f;
	registry.colors.insert(player_doll, {1, 0.8f, 0.8f});

    helpScreen = createHelpWindow(renderer, { screen_width / 2.f, screen_height / 2.f });
	Motion& help_motion = registry.motions.get(helpScreen);
	help_motion.scale = help_motion.scale * float(screen_width / 8);
}


void WorldSystem::drawBattleWindow() {
	selected_move_menu = BattleMenuItemType::NONE;
	battle_screen = createBattleWindow(renderer, { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
    battle_doll = createBattleDoll(renderer, { SCREEN_WIDTH / 3.f, SCREEN_HEIGHT / 2.f });
    battle_enemy = createBattleEnemy(renderer, { SCREEN_WIDTH - SCREEN_WIDTH / 3.f, SCREEN_HEIGHT / 2.f });

	float SCALE = SCREEN_HEIGHT / 160; // 160 px is the height of the Aseprite drawing of the battle screen
	
	// The numbers that the "POSITION" quantities are being multiplied by are the positions of the assets on the Aseprite drawing
	vec2 BUTTON_AREA_POSITION = { 32 * SCALE, 132 * SCALE };
	battle_menu_button_area = createBattleMenuItem(renderer, BUTTON_AREA_POSITION, BattleMenuItemType::BUTTON_AREA, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_AREA);	
	scaleUIAsset(battle_menu_button_area, { 60, 51 }, SCALE);

	vec2 TEXT_AREA_POSITION = { 151 * SCALE, 132 * SCALE };
	battle_menu_text_area = createBattleMenuItem(renderer, TEXT_AREA_POSITION, BattleMenuItemType::TEXT_AREA, TEXTURE_ASSET_ID::BATTLE_MENU_TEXT_AREA);
	scaleUIAsset(battle_menu_text_area, { 173, 51 }, SCALE);

	vec2 ATTACK_BUTTON_POSITION = { 32 * SCALE, 132 * SCALE - 73 };
	battle_menu_button_attack = createBattleMenuItem(renderer, ATTACK_BUTTON_POSITION, BattleMenuItemType::ATTACK_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_ATTACK);
	scaleUIAsset(battle_menu_button_attack, { 52, 13 }, SCALE);

	vec2 MAGIC_BUTTON_POSITION = { 32 * SCALE, 132 * SCALE };
	battle_menu_button_magic = createBattleMenuItem(renderer, MAGIC_BUTTON_POSITION, BattleMenuItemType::MAGIC_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_MAGIC);
	scaleUIAsset(battle_menu_button_magic, { 52, 13 }, SCALE);

	vec2 ITEMS_BUTTON_POSITION = { 32 * SCALE, 132 * SCALE + 73 };
	battle_menu_button_items = createBattleMenuItem(renderer, ITEMS_BUTTON_POSITION, BattleMenuItemType::ITEMS_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_ITEMS);
	scaleUIAsset(battle_menu_button_items, { 52, 13 }, 5);
	
	// Commented these out for now - implementation of the buttons below is a TODO
	//vec2 LEARN_BUTTON_POSITION = { 224 * SCALE, 132 * SCALE };
	//battle_menu_button_learn = createBattleMenuItem(renderer, LEARN_BUTTON_POSITION, BattleMenuItemType::LEARN_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_LEARN);
	//scaleUIAsset(battle_menu_button_learn, { 18, 13 }, 5);

	//vec2 GO_BUTTON_POSITION = { 224 * SCALE, 132 * SCALE + 73 };
	//battle_menu_button_go = createBattleMenuItem(renderer, GO_BUTTON_POSITION, BattleMenuItemType::GO_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_GO);
	//scaleUIAsset(battle_menu_button_go, { 18, 13 }, SCALE);
}

void WorldSystem::scaleUIAsset(Entity entity, vec2 originalDimensions, float scaleFactor) {
	assert(registry.motions.has(entity));
	Motion& motion = registry.motions.get(entity);
	motion.scale.x = originalDimensions[0] * scaleFactor;
	motion.scale.y = originalDimensions[1] * scaleFactor;
}

void WorldSystem::destroyBattleWindow() {
	clearBattlePlayerMoveButtons();
	registry.remove_all_components_of(battle_screen);
	registry.remove_all_components_of(battle_doll);
	registry.remove_all_components_of(battle_enemy);
	registry.remove_all_components_of(battle_menu_button_area);
	registry.remove_all_components_of(battle_menu_text_area);
	registry.remove_all_components_of(battle_menu_button_attack);
	registry.remove_all_components_of(battle_menu_button_magic);
	registry.remove_all_components_of(battle_menu_button_items);
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the salmon
		if (registry.players.has(entity)) {
			Player& player = registry.players.get(entity);


			if (registry.enemies.has(entity_other)) {
				// initiate death unless already dying
				// registry.remove_all_components_of(entity_other); // Need enemy info for battle so I commented it out - Naoreen
                Game& game = registry.game.get(player_doll);
                game.state = GameState::BATTLE;
				if (!registry.currentEnemies.has(entity_other)) {
					registry.currentEnemies.emplace(entity_other);
                    drawBattleWindow();
				}
				
			}
		}
	}
	// Remove all collisions from this simulation step
	registry.collisions.clear();
}


// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	auto gamestate = registry.game.get(player_doll).state;
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        gamestate = GameState::PLAYING;
	}

	// Temporary: Testing for blur/background
	Background& bg_motion = registry.backgrounds.get(background);
	if (action == GLFW_PRESS) {
		// Blur
		if (key == GLFW_KEY_P) {
			bg_motion.blur_state = 1;
		}
		// Regular
		else if (key == GLFW_KEY_O) {
			bg_motion.blur_state = 0;
		}
		// Tweaking
		else if (key == GLFW_KEY_I) {
			bg_motion.blur_state = 2;
		}
	}

    Motion& motion = registry.motions.get(player_doll);
	if (action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W) {
			motion.dir = Direction::UP;
			motion.velocity = vec2{ 0, -player_speed };
		}
		else if (key == GLFW_KEY_S) {
			motion.dir = Direction::DOWN;
			motion.velocity = vec2{ 0, +player_speed };
		}
		else if (key == GLFW_KEY_A) {
			motion.dir = Direction::LEFT;
			motion.velocity = vec2{ -player_speed, 0 };
		}
		else if (key == GLFW_KEY_D) {
			motion.dir = Direction::RIGHT;
			motion.velocity = vec2{ +player_speed, 0 };
		}
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            motion.dir = Direction::UP;
        } else if (key == GLFW_KEY_S) {
            motion.dir = Direction::DOWN;
        } else if (key == GLFW_KEY_A) {
            motion.dir = Direction::LEFT;
        } else if (key == GLFW_KEY_D) {
            motion.dir = Direction::RIGHT;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W) {
            motion.velocity = vec2{motion.velocity[0], 0};
        } else if (key == GLFW_KEY_S) {
            motion.velocity = vec2{motion.velocity[0], 0};
        } else if (key == GLFW_KEY_A) {
            motion.velocity = vec2{0, motion.velocity[1]};
        } else if (key == GLFW_KEY_D) {
            motion.velocity = vec2{0, motion.velocity[1]};
        }
    }

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::on_mouse_click(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && registry.game.get(player_doll).state == GameState::BATTLE) {
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		BattleMenuItemType button_clicked = getBattleScreenButtonClicked(xPos, yPos);	
		if (button_clicked != BattleMenuItemType::NONE) {
			handleBattleScreenButtonClick(button_clicked);
		}
	}
}

void WorldSystem::setRenderRequests() {
    if (registry.game.get(player_doll).state == GameState::PLAYING) {
        RenderRequest& rr = registry.renderRequests.get(player_doll);
        Direction dir = registry.motions.get(player_doll).dir;
        if (dir == Direction::UP) {
            rr.used_texture = TEXTURE_ASSET_ID::DOLL_UP;
        } else if (dir == Direction::RIGHT) {
            rr.used_texture = TEXTURE_ASSET_ID::DOLL_RIGHT;
        } else if (dir == Direction::DOWN) {
            rr.used_texture = TEXTURE_ASSET_ID::DOLL_DOWN;
        } else if (dir == Direction::LEFT) {
            rr.used_texture = TEXTURE_ASSET_ID::DOLL_LEFT;
        }
    }
}

void WorldSystem::handleBattleScreenButtonClick(BattleMenuItemType button_clicked) {
	assert(registry.game.get(player_doll).state == GameState::BATTLE);

	if (button_clicked == BattleMenuItemType::ATTACK_BUTTON) {
		selected_move_menu = button_clicked;
		float SCALE = SCREEN_HEIGHT / 160;

		AttackList& player_attacks = registry.attackLists.get(player_doll);
		if (player_attacks.hasAttack("PUNCH") && !registry.battleMenuPlayerMoves.has(punch_button)) {
			vec2 PUNCH_BUTTON_POSITION = { 90 * SCALE, 132 * SCALE - 73 };
			punch_button = createBattleMenuItem(renderer, PUNCH_BUTTON_POSITION, BattleMenuItemType::ATTACK_PUNCH, TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH);
			scaleUIAsset(punch_button, { 37, 11 }, SCALE);
		}		
	}
	else if (button_clicked == BattleMenuItemType::MAGIC_BUTTON || button_clicked == BattleMenuItemType::ITEMS_BUTTON) {
		selected_move_menu = button_clicked;
		// Just have it clear the menu for now
		clearBattlePlayerMoveButtons();
	}
	else if (selected_move_menu == BattleMenuItemType::ATTACK_BUTTON && button_clicked == BattleMenuItemType::ATTACK_PUNCH) {
		// Make the button look selected
		RenderRequest& rr = registry.renderRequests.get(punch_button);
		rr.used_texture = TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH_SELECTED;

		// Issue attack - in future we may want to add an additional step (using the "Go" button)
		registry.turns.emplace(player_doll);
		Turn& turn = registry.turns.get(player_doll);
		turn.move = "PUNCH";
	}
}

void WorldSystem::clearBattlePlayerMoveButtons() {
	assert(registry.game.get(player_doll).state == GameState::BATTLE);
	if (registry.battleMenuPlayerMoves.has(punch_button)) {
		registry.remove_all_components_of(punch_button);
	}
}

BattleMenuItemType WorldSystem::getBattleScreenButtonClicked(double x, double y) {
	assert(registry.game.get(player_doll).state == GameState::BATTLE);

	const double BUTTON_HEIGHT = 58;
	const double BUTTON_WIDTH_LARGER = 254; // Attack, Magic, and Items buttons
	const double BUTTON_WIDTH_SMALLER = 85; // Learn and Go buttons

	// Coordinates of button's top left corner, {x, y}
	const vec2 ACTION_BUTTON_COORDS = { 33, 560 };
	const vec2 MAGIC_BUTTON_COORDS = { 33, 631 };
	const vec2 ITEMS_BUTTON_COORDS = { 33, 703 };
	const vec2 LEARN_BUTTON_COORDS = { 1075, 631 };
	const vec2 GO_BUTTON_COORDS = { 1075, 703 };
	const vec2 PUNCH_BUTTON_COORDS = { 364, 569 };

	if (isClickInRegion(x, y, ACTION_BUTTON_COORDS, BUTTON_HEIGHT, BUTTON_WIDTH_LARGER)) {
		return BattleMenuItemType::ATTACK_BUTTON;
	}
	else if (isClickInRegion(x, y, MAGIC_BUTTON_COORDS, BUTTON_HEIGHT, BUTTON_WIDTH_LARGER)) {
		return BattleMenuItemType::MAGIC_BUTTON;
	}
	else if (isClickInRegion(x, y, ITEMS_BUTTON_COORDS, BUTTON_HEIGHT, BUTTON_WIDTH_LARGER)) {
		return BattleMenuItemType::ITEMS_BUTTON;
	}
	else if (isClickInRegion(x, y, LEARN_BUTTON_COORDS, BUTTON_HEIGHT, BUTTON_WIDTH_SMALLER)) {
		return BattleMenuItemType::LEARN_BUTTON;
	}
	else if (isClickInRegion(x, y, GO_BUTTON_COORDS, BUTTON_HEIGHT, BUTTON_WIDTH_SMALLER)) {
		return BattleMenuItemType::GO_BUTTON;
	}
	else if (selected_move_menu == BattleMenuItemType::ATTACK_BUTTON && isClickInRegion(x, y, PUNCH_BUTTON_COORDS, 37, 174)) {
		return BattleMenuItemType::ATTACK_PUNCH;
	}
	else {
		return BattleMenuItemType::NONE;
	}
}

bool WorldSystem::isClickInRegion(double x, double y, vec2 top_left_coords, double height, double width) {
	return
		x >= top_left_coords[0] &&
		x <= top_left_coords[0] + width &&
		y >= top_left_coords[1] &&
		y <= top_left_coords[1] + height;
}
