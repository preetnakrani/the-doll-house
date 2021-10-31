// Header
#include "world_system.hpp"
#include "world_init.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>

#include "physics_system.hpp"
#include "battle_system.hpp"


// Game configuration
const size_t MAX_ENEMY = 5;
const size_t ENEMY_DELAY_MS = 2000 * 3;
std::array<TEXTURE_ASSET_ID, 5> helpScreenOptions = { TEXTURE_ASSET_ID::HELP_PRESS_A,
													 TEXTURE_ASSET_ID::HELP_PRESS_D,
													 TEXTURE_ASSET_ID::HELP_PRESS_W,
													 TEXTURE_ASSET_ID::HELP_PRESS_S,
													 TEXTURE_ASSET_ID::STAY_AWAY };
std::array<TEXTURE_ASSET_ID, 5> tutorialScreenOptions = { TEXTURE_ASSET_ID::TUTORIAL_ONE,
													 TEXTURE_ASSET_ID::TUTORIAL_TWO,
													 TEXTURE_ASSET_ID::TUTORIAL_THREE,
													 TEXTURE_ASSET_ID::TUTORIAL_FOUR,
													 TEXTURE_ASSET_ID::TUTORIAL_FIVE };
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
//	glfwWindowHint(GLFW_RESIZABLE, 0);

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
//    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    SCREEN_HEIGHT = window_height_px;
    SCREEN_WIDTH = window_width_px;

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
//    std::cout << "framebuffer size: " << registry.players.get(player_doll).fBHeight << ", " << registry.players.get(player_doll).fBWidth << std::endl;
    if (registry.game.get(player_doll).state == GameState::TUTORIAL) {
        progressTutorial(elapsed_ms_since_last_update);
    }
    if (registry.game.get(player_doll).state != GameState::PLAYING) {
        debugging.in_debug_mode = false;
    }	// Get the screen dimensions
	int screen_width, screen_height;
    screen_height = window_height_px;
    screen_width = window_width_px;
//	glfwGetFramebufferSize(window, &screen_width, &screen_height);
    SCREEN_HEIGHT = window_height_px;
    SCREEN_WIDTH = window_width_px;


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
					50.f + 250 + uniform_dist(rng) * (2 * 250 - 100.f));

        vec2 bounding = vec2(100.f, 100.f);
		Entity new_enemy = createEnemy(renderer, position);
		registry.motions.get(new_enemy).position = position;
		registry.motions.get(new_enemy).scale = bounding;

		for (std::function<void(Entity)> fn : callbacks) {
			fn(new_enemy);
		}

		if (registry.motions.has(new_enemy)) {
			registry.motions.get(new_enemy).scale = vec2(64.f, 64.f);
		}
    }
    // display the enum number that corresponds to the current state of the game
//    Game& game = registry.game.get(player_doll);
//    std::cout << static_cast<int>(game.state) << std::endl;

	for (int i = 0; i < registry.animatedSprites.entities.size(); i++) {
		setSpriteAnimationPauseState(registry.animatedSprites.entities[i]);
		setSpriteAnimationFrame(elapsed_ms_since_last_update, registry.animatedSprites.entities[i]);
	}

    return true;
}

void WorldSystem::progressTutorial(float elapsed_ms_since_last_update) {
    Background& bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 1;
    registry.helpScreens.remove(helpScreen);
    registry.helpScreens.remove(menuButton);
    RenderRequest& rr = registry.renderRequests.get(tutorialScreen);
    TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
    float& time = tutorialTimer.timePerSprite;
    int& index = tutorialTimer.tutorialIndex;
    bool& isComplete = tutorialTimer.tutorialCompleted;
    time -= elapsed_ms_since_last_update;
    if (time <= 0) {
        if (index == 4) {
            escapeTutorial(isComplete);
        } else {
            index++;
            rr.used_texture = tutorialScreenOptions[index];
            tutorialTimer.timePerSprite = 7000.f;
        }
    }
}

void WorldSystem::findInitialFrameBufferSize(){
    if (fbWidth == 0 && fbHeight == 0) {
//        printf("fBWidth needs update\n");
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        fbWidth = w;
        fbHeight = h;
    }
}

void WorldSystem::escapeTutorial(bool isComplete) {
    isComplete = true;
    registry.helpScreens.emplace(helpScreen);
    registry.helpScreens.emplace(menuButton);
    registry.renderRequests.remove(tutorialScreen);
    Game& game = registry.game.get(player_doll);
    game.state = GameState::PLAYING;
    Background& bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 0;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

    findInitialFrameBufferSize();

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
//	glfwGetFramebufferSize(window, &screen_width, &screen_height);
    screen_height = window_height_px;
    screen_width = window_width_px;
	background = createBackground(renderer, { 600, 400});

    // create a help screen

    helpScreen = createHelpWindow(renderer, { 600, 150 });
    Motion& help_motion = registry.motions.get(helpScreen);
    help_motion.scale = help_motion.scale * float(200);

	// Create a new doll
	player_doll = createDoll(renderer, { 300, 300 }, {fbWidth, fbHeight});
	Motion& motion = registry.motions.get(player_doll);
//	motion.scale = motion.scale * float(screen_width / 8);
	motion.scale = motion.scale * float(150);
    player_speed = 200.f;
	registry.colors.insert(player_doll, {1, 0.8f, 0.8f});

	// create a clickable menu button
	menuButton = createMenuButton(renderer, { 1150, 25 });
    Motion& menuButton_motion = registry.motions.get(menuButton);
    menuButton_motion.scale = menuButton_motion.scale * float(75);

    // create a menu overlay
    menuOverlay = createMenuOverlay(renderer, {600, 400});
    Motion& overlayMotion = registry.motions.get(menuOverlay);
    overlayMotion.scale = overlayMotion.scale * float(300);

    // create tutorial screen
    tutorialScreen = createTutorial(renderer, {600, 400});
    Motion& tutorialMotion = registry.motions.get(tutorialScreen);
    tutorialMotion.scale = tutorialMotion.scale * float(600);
    // set state to tutorial
    if (!registry.tutorialTimer.get(tutorialScreen).tutorialCompleted) {
        Game& game = registry.game.get(player_doll);
        game.state = GameState::TUTORIAL;
    }

	//hardcoded for now while we figure out save/load
	createWallBlock({ 50.f, 150.f });
	createWallBlock({ 150.f, 150.f });
	createWallBlock({ 250.f, 150.f });
	createWallBlock({ 350.f, 150.f });
	createWallBlock({ 450.f, 150.f });
	createWallBlock({ 550.f, 150.f });
	createWallBlock({ 650.f, 150.f });
	createWallBlock({ 750.f, 150.f });
	createWallBlock({ 850.f, 150.f });
	createWallBlock({ 950.f, 150.f });
	createWallBlock({ 1050.f, 150.f });
	createWallBlock({ 1150.f, 150.f });
	createWallBlock({ 1250.f, 150.f });

	// create clickable area
}


void WorldSystem::drawBattleWindow() {
	selected_move_menu = BattleMenuItemType::NONE;
	battle_screen = createBattleWindow(renderer, { 600.f, 400.f });
    battle_doll = createBattleDoll(renderer, { 400, 400 });
    battle_enemy = createBattleEnemy(renderer, { 800, 400 });

	float SCALE = 800 / 160; // 160 px is the height of the Aseprite drawing of the battle screen

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
                Game& game = registry.game.get(player_doll);
                game.state = GameState::BATTLE;
				if (!registry.currentEnemies.has(entity_other)) {
					registry.currentEnemies.emplace(entity_other);
                    debugging.in_debug_mode = false;
                    drawBattleWindow();
				}

			}

			// bounce off walls
			if (registry.walls.has(entity_other)) {
				Motion& motion = registry.motions.get(entity);
				motion.velocity = vec2(0.f, 0.f);

				if (motion.dir == Direction::UP) {
					motion.position = vec2(motion.position[0], motion.position[1] + 0.07f);
				} else if (motion.dir == Direction::RIGHT) {
					motion.position = vec2(motion.position[0] - 0.07f, motion.position[1]);

				} else if (motion.dir == Direction::DOWN) {
					motion.position = vec2(motion.position[0], motion.position[1] - 0.07f);

				} else if (motion.dir == Direction::LEFT) {
					motion.position = vec2(motion.position[0] + 0.07f, motion.position[1]);

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
    // press spacebar to close tutorial screen
    Game& game = registry.game.get(player_doll);
    // press slash to activate debugging mode, release to deactivate
    if (game.state == GameState::PLAYING && key == GLFW_KEY_SLASH) {
        if (action == GLFW_PRESS) {
            debugging.in_debug_mode = !debugging.in_debug_mode;
        }
    }
    // press spacebar to close tutorial screen
    if (game.state == GameState::TUTORIAL && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
        escapeTutorial(tutorialTimer.tutorialCompleted);
    }
    // press return key to progress tutorial faster
    if (game.state == GameState::TUTORIAL && key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        RenderRequest& rr = registry.renderRequests.get(tutorialScreen);
        TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
        float& time = tutorialTimer.timePerSprite;
        int& index = tutorialTimer.tutorialIndex;
        bool& isComplete = tutorialTimer.tutorialCompleted;
        if (index == 4) {
            escapeTutorial(isComplete);
        } else {
            index++;
            rr.used_texture = tutorialScreenOptions[index];
            tutorialTimer.timePerSprite = 7000.f;
            }
        }

    // change the helpscreen display, n to move left and m to move right
    if (registry.helpScreens.has(helpScreen)) {
        HelpScreen& hs = registry.helpScreens.get(helpScreen);
        RenderRequest& hs_rr = registry.renderRequests.get(helpScreen);
        if (key == GLFW_KEY_N && action == GLFW_PRESS && hs.order != 0) {
            hs_rr.used_texture = helpScreenOptions[hs.order - 1];
            hs.order--;
        } else if (key == GLFW_KEY_M && action == GLFW_PRESS && hs.order !=4) {
            hs_rr.used_texture = helpScreenOptions[hs.order + 1];
            hs.order++;
        }
    }

    // Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
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
    Direction prev = motion.dir;
 if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
			motion.velocity = vec2{ 0, -player_speed };
            motion.dir = Direction::UP;
        } else if (key == GLFW_KEY_S) {
			motion.velocity = vec2{ 0, +player_speed };
            motion.dir = Direction::DOWN;
        } else if (key == GLFW_KEY_A) {
			motion.velocity = vec2{ -player_speed, 0 };
            motion.dir = Direction::LEFT;
        } else if (key == GLFW_KEY_D) {
			motion.velocity = vec2{ +player_speed, 0 };
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
    if (prev != motion.dir) {
        this->setRenderRequests();
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

void WorldSystem::on_mouse_click(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        Game& game = registry.game.get(player_doll);
        // clickevent happening on menu box
        if (isClickInRegion(xPos, yPos, {1115, 10}, 40, 75)) {
            if (game.state == GameState::MENUOVERLAY) {
                closeMenuOverlayScreen();
            } else if (game.state == GameState::PLAYING) {
                openMenuOverlayScreen();
            }
        }
        if (game.state == GameState::MENUOVERLAY) {
            if (isClickInRegion(xPos, yPos, {460, 320}, 40, 300)) {
                closeMenuOverlayScreen();
                restart_game();
            }
            if (isClickInRegion(xPos, yPos, {460, 430}, 40, 300)) {
                closeMenuOverlayScreen();
                registry.renderRequests.insert(
                        tutorialScreen,
                        { TEXTURE_ASSET_ID::TUTORIAL_ONE,
                          EFFECT_ASSET_ID::HELP_SCREEN,
                          GEOMETRY_BUFFER_ID::HELP_SCREEN});
                TutorialTimer& tutorial = registry.tutorialTimer.get(tutorialScreen);
                tutorial.tutorialCompleted = false;
                tutorial.tutorialIndex = 0;
                Game& game = registry.game.get(player_doll);
                game.state = GameState::TUTORIAL;
            }
        }
		if (game.state == GameState::BATTLE) {
			BattleMenuItemType button_clicked = getBattleScreenButtonClicked(xPos, yPos);
			if (button_clicked != BattleMenuItemType::NONE) {
				handleBattleScreenButtonClick(button_clicked);
			}
		}
    }

}

void WorldSystem::openMenuOverlayScreen() {
    Game& game = registry.game.get(player_doll);
    game.state = GameState::MENUOVERLAY;
    Background& bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 1;
    registry.renderRequests.insert(
            menuOverlay,
            { TEXTURE_ASSET_ID::MENU_OVERLAY,
              EFFECT_ASSET_ID::HELP_SCREEN,
              GEOMETRY_BUFFER_ID::SPRITE});
    registry.helpScreens.remove(helpScreen);
}

void WorldSystem::closeMenuOverlayScreen() {
    Background& bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 0;
    registry.renderRequests.remove(menuOverlay);
    registry.helpScreens.emplace(helpScreen);
    Game& game = registry.game.get(player_doll);
    game.state = GameState::PLAYING;
}

void WorldSystem::attach(std::function<void(Entity)> fn) {
	callbacks.push_back(fn);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::setSpriteAnimationFrame(float elapsed_time_ms, Entity entity) {
	assert(registry.animatedSprites.has(entity));
	AnimatedSprite& animated_sprite = registry.animatedSprites.get(entity);
	animated_sprite.ms_since_last_update += elapsed_time_ms;
	if (animated_sprite.paused == true) {
		animated_sprite.current_frame = 0;
	}
	else if (animated_sprite.ms_since_last_update > animated_sprite.animation_speed_ms) {
		int num_frames = animated_sprite.num_frames_per_type[animated_sprite.current_type];
		animated_sprite.current_frame = (animated_sprite.current_frame + 1) % num_frames;
		animated_sprite.ms_since_last_update = 0;
	}
}

void WorldSystem::setSpriteAnimationPauseState(Entity entity) {
	assert(registry.animatedSprites.has(entity));
	AnimatedSprite& animated_sprite = registry.animatedSprites.get(entity);

	// pause/go logic for player
	if (registry.players.has(entity)) {
		Motion& player_motion = registry.motions.get(entity);
		animated_sprite.paused = player_motion.velocity == vec2{ 0, 0 };
	}
}

void WorldSystem::setRenderRequests() {
	// TODO: This function name isn't really relevant anymore, maybe we can change it -Naoreen
    if (registry.game.get(player_doll).state == GameState::PLAYING) {
        Direction dir = registry.motions.get(player_doll).dir;
		AnimatedSprite& animated_sprite = registry.animatedSprites.get(player_doll);
        if (dir == Direction::UP) {
			animated_sprite.current_type = 2;
        } else if (dir == Direction::RIGHT) {
			animated_sprite.current_type = 1;
        } else if (dir == Direction::DOWN) {
			animated_sprite.current_type = 0;
        } else if (dir == Direction::LEFT) {
			animated_sprite.current_type = 3;
        }
    }
}

void WorldSystem::handleBattleScreenButtonClick(BattleMenuItemType button_clicked) {
	assert(registry.game.get(player_doll).state == GameState::BATTLE);

	if (button_clicked == BattleMenuItemType::ATTACK_BUTTON) {
		selected_move_menu = button_clicked;
		float SCALE = SCREEN_HEIGHT / 160;

		AttackList& player_attacks = registry.attackLists.get(player_doll);
		if (player_attacks.hasAttack(AttackName::PUNCH) && !registry.battleMenuPlayerMoves.has(punch_button)) {
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

		// Register attack turn - in future we may want to add an additional step (using the "Go" button)
		if (registry.game.get(player_doll).battle_state == BattleState::PLAYER_TURN && !registry.turns.has(player_doll)) {
			registry.turns.insert(player_doll, Turn(AttackName::PUNCH));
		}
		else {
			printf("Wait for your turn!\n");
		}
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
	int w, h;
	int originalHeight = registry.players.get(player_doll).fBHeight;
    int originalWidth = registry.players.get(player_doll).fBWidth;
    glfwGetFramebufferSize(window, &w, &h);
    int scaledX = x * originalWidth / w;
    int scaledY = y * originalHeight / h;
    return
		scaledX >= top_left_coords[0] &&
		scaledX <= top_left_coords[0] + width &&
		scaledY >= top_left_coords[1] &&
		scaledY <= top_left_coords[1] + height;
}