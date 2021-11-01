// internal
#include "ai_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"

#include <iostream>
#include <random>

#include <vector>
#include <chrono>
#include <queue>

float ms_since_recompute = 0;
float enemy_speed = 80;
std::vector<std::vector<int>> grid;

struct Choices
{
	float ATTACK = 25;
	float DEFEND = 25;
	float DEBUFF = 25;
	float HEAL = 25;

};

// Update enemy state
void updateEnemyState(Entity& enemy, Entity& player) {
	EnemyState& enemy_state = registry.currentEnemies.get(enemy).enemyState;
	int enemy_health = registry.health.get(enemy).health;
	int player_health = registry.health.get(player).health;


	if (enemy_health > 80) {
		enemy_state = EnemyState::HEALTHY;
	}
	else if (enemy_health > 50) {
		enemy_state = EnemyState::DAMAGED;
	}
	else if (enemy_health > player_health + 20) {
		enemy_state = EnemyState::BOLD;
	}
	else if (enemy_health < player_health - 20) {
		enemy_state = EnemyState::THREATENED;
	}
	else if (enemy_health < 20) {
		enemy_state = EnemyState::NEAR_DEATH;
	}
	else if (registry.status.get(enemy).TAUNTED) {
		enemy_state = EnemyState::TAUNTED;
	}
}

void decideMoveType(Entity& enemy, Choices& choices, EnemyState state, Status player_status, Status enemy_status) {
	
	if (state == EnemyState::BOLD) {
		choices.DEFEND = 0;
		choices.ATTACK = 33;
		choices.HEAL = 10;
	}
	if (state == EnemyState::THREATENED) {
		choices.ATTACK = 10;
		choices.DEFEND = 40;
	}
	if (state == EnemyState::NEAR_DEATH) {
		choices.HEAL = 50;
		choices.DEFEND = 30;
	}
	// Check if Enemy has moves to fulfill conditions
	//This should not ever happen, but just in case there's an enemy that does not have attacks.
	if (!registry.attackLists.has(enemy)) {
		if (registry.attackLists.get(enemy).available_attacks.size() == 0) {
			choices.ATTACK = 0;
		}
	}
	if (!registry.magicLists.has(enemy)) {
		if (registry.magicLists.get(enemy).available_magic.size() == 0) {
			choices.DEBUFF = 0;
			choices.DEFEND = 0;
		}
	}
	// Situations that will override the probabilities
	if (enemy_status.SHIELDED) {
		choices.DEFEND = 0;
	}
	if (player_status.TAUNTED) {
		choices.DEBUFF = 0;
	}
	if (!registry.gameItems.has(enemy) || state == EnemyState::HEALTHY) {
		choices.HEAL = 0;
	}

}

void normalizeChoices(Choices& choices) {
	float total = choices.ATTACK + choices.DEBUFF + choices.DEFEND + choices.HEAL;

	if (choices.ATTACK > 0) {
		choices.ATTACK = (choices.ATTACK / total) * 100;
	}
	if (choices.DEBUFF > 0) {
		choices.DEBUFF = (choices.DEBUFF / total) * 100;
	}
	if (choices.DEFEND > 0) {
		choices.DEFEND = (choices.DEFEND / total) *  100;
	}
	if (choices.HEAL > 0) {
		choices.HEAL = (choices.HEAL / total) * 100;
	}
}


// Decide on a move based on state and statuses
Turn AISystem::decideMove(Entity& enemy, Entity& player){
	EnemyState enemy_state = registry.currentEnemies.components[0].enemyState;
	Status enemy_status = registry.status.get(enemy);
	Status player_status = registry.status.get(player);
	GameItem item = GameItem();
	if (registry.gameItems.has(enemy)) {
		GameItem& item = registry.gameItems.get(enemy);
	}
	
	// Chance roll to see what to do

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand0 generator(seed);  // minstd_rand0 is a standard linear_congruential_engine
	float chance_roll = generator() % 100;
	
	// Choices and their probabilities
	Choices choices;
	// Update move probability list based on Player and Enemy States and statuses.
	decideMoveType(enemy, choices, enemy_state, player_status, enemy_status);
	// Normalize probability
	normalizeChoices(choices);
	

	// If taunted, no choice of moves, must do an attack
	if (enemy_state == EnemyState::TAUNTED) {
		if(registry.attackLists.get(enemy).hasAttack(AttackName::SNEEZE))
			return Turn(AttackName::SNEEZE);
	}
	// Attack
	if ((chance_roll >= 0) && (chance_roll < choices.ATTACK)) {
		if (registry.attackLists.get(enemy).hasAttack(AttackName::SNEEZE))
			return Turn(AttackName::SNEEZE);
	}
	// Defend/ Self Buff
	else if ((chance_roll >= choices.ATTACK) && (chance_roll < (choices.ATTACK + choices.DEFEND))) {
		if (registry.magicLists.get(enemy).hasMagic(MagicName::SHIELD))
			return Turn(MagicName::SHIELD);
	}
	// Debuff Player
	else if ((chance_roll >= (choices.ATTACK + choices.DEFEND)) && (chance_roll < (choices.ATTACK + choices.DEFEND + choices.DEBUFF))) {
		if (registry.magicLists.get(enemy).hasMagic(MagicName::TAUNT))
			return Turn(MagicName::TAUNT);
	}
	// Heal
	else if ((chance_roll >= (choices.ATTACK + choices.DEFEND + choices.DEBUFF)) && (chance_roll < (choices.ATTACK + choices.DEFEND + choices.DEBUFF + choices.HEAL))) {
		return Turn(GameItemName::HEALING_POTION);
	}
	else {
		return Turn(registry.attackLists.get(enemy).available_attacks[0].name);
	}

	return Turn(AttackName::NONE);
}


void AISystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	Entity player_doll = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player_doll);
	Game& game = registry.game.get(player_doll);

	if (game.battle_state == BattleState::ENEMY_TURN) {
		do_enemy_turn();
	}

	if (debugging.in_debug_mode) {
		for (int i = 0; i < grid.size(); i++) {
			for (int j = 0; j < grid[0].size(); j++) {
				if (grid[i][j]) {
					createLine({ 50.f * i, 50.f * j }, { 5.f, 5.f });
				}
			}
		}
	}

	ms_since_recompute -= elapsed_ms;
	if (ms_since_recompute <= 0) {
		ms_since_recompute = 500;


		for (int i = 0; i < registry.AIEntities.entities.size(); i++) {
			registry.AIEntities.get(registry.AIEntities.entities[i]).coveredPositions = {};
		}
		grid = {};
		PhysicsSystem physics;
		std::function<void(Entity, bool)> callback = physics.getCollisionFunction();
		for (int i = 0; i < window_width_px / 50.f; i++) {
			std::vector<int> col;
			for (int j = 0; j < window_height_px / 50.f; j++) {
				Entity testEntity = createTestEnemy({ 50.f * i, 50.f * j });

				callback(testEntity, true);

				if (registry.motions.has(testEntity)) {
					col.push_back(1);
					registry.remove_all_components_of(testEntity);
				}
				else {
					col.push_back(0);
				}
			}
			grid.push_back(col);
		}


		std::pair<int, int> dollPos = std::make_pair(
			round((player_motion.position.x - fmod(player_motion.position.x, 50)) / 50.f),
			round((player_motion.position.y - fmod(player_motion.position.y, 50)) / 50.f)
		);


		// Do a BFS
		for (int i = 0; i < registry.enemies.size(); i++) {
			std::vector<std::vector<int>> copyGrid = grid;
			Motion& enemy_motion = registry.motions.get(registry.enemies.entities[i]);
			std::pair<int, int> enemyPos = std::make_pair(
				round((enemy_motion.position.x - fmod(enemy_motion.position.x, 50)) / 50.f),
				round((enemy_motion.position.y - fmod(enemy_motion.position.y, 50)) / 50.f)
			);

			for (int j = 0; j < registry.AIEntities.entities.size(); j++) {
				std::vector<std::pair<int, int>> coveredPositions = registry.AIEntities.get(registry.AIEntities.entities[j]).coveredPositions;
				for (int k = 0; k < coveredPositions.size(); k++) {
					copyGrid[coveredPositions[k].first][coveredPositions[k].second] = 1;
				}
			}
			std::vector<std::pair<int, int>> path = bfs(copyGrid, enemyPos, dollPos);
			if (path.size() > 1) {
				enemy_motion.velocity = { (path[1].first - enemyPos.first) * enemy_speed, (path[1].second - enemyPos.second) * enemy_speed };
				if (enemy_motion.velocity.x + enemy_motion.velocity.y > enemy_speed) {
					enemy_motion.velocity = {
						(enemy_motion.velocity.x / (enemy_motion.velocity.x + enemy_motion.velocity.y)) * enemy_speed,
						(enemy_motion.velocity.y / (enemy_motion.velocity.x + enemy_motion.velocity.y)) * enemy_speed
					};
				}
			}
			else {
				enemy_motion.velocity = { 0.f, 0.f };
			}
		}

	}
}

// Wrote this just to make sure enemy attacking is working, please feel free to completely delete/change it - Naoreen
void AISystem::do_enemy_turn() {
	Entity player_doll = registry.players.entities[0];
	Game& game = registry.game.get(player_doll);

	assert(game.state == GameState::BATTLE);
	assert(!registry.currentEnemies.entities.empty());

	// Get the current enemies in battle
	std::vector<Entity> current_enemies = registry.currentEnemies.entities;

	// Just have one enemy attack with whatever its first attack is for now
	AttackList& attack_list = registry.attackLists.get(current_enemies[0]);
	assert(!attack_list.available_attacks.empty());

	// Decide what attack to do based on state enemy is in
	if (!registry.turns.has(current_enemies[0])) {
		Entity enemy = current_enemies[0];
		updateEnemyState(enemy, player_doll);
		Turn chosen_move = decideMove(enemy, player_doll);
		registry.turns.insert(current_enemies[0], chosen_move);
	}
}

std::vector<std::pair<int, int>> getNeighbours(std::pair<int, int > center) {
	std::vector<std::pair<int, int>> neighbours = {};
	neighbours.push_back(std::make_pair(center.first, center.second - 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second));
	neighbours.push_back(std::make_pair(center.first + 1, center.second));
	neighbours.push_back(std::make_pair(center.first, center.second + 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second - 1));
	neighbours.push_back(std::make_pair(center.first + 1, center.second - 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second + 1));
	neighbours.push_back(std::make_pair(center.first + 1, center.second + 1));

	return neighbours;
}

// do bfs to find shortest path from curr to goal and return the position of the first node in that path
std::vector<std::pair<int, int>> AISystem::bfs(std::vector<std::vector<int>> grid, std::pair<int, int> curr, std::pair<int, int> goal) {
	std::queue<std::vector<std::pair<int, int>>> queue;
	std::vector<std::pair<int, int>> currVec;
	currVec.push_back(curr);
	queue.push(currVec);
	grid[curr.first][curr.second] = 0;

	while (!queue.empty()) {
		std::vector<std::pair<int, int>> curr_path = queue.front();
		std::pair<int, int> last_node = curr_path.back();
		if (last_node.first == goal.first && last_node.second == goal.second) {
			return curr_path;
		}

		std::vector<std::pair<int, int>> neighbours = getNeighbours(last_node);

		for (int i = 0; i < neighbours.size(); i++) {
			if (neighbours[i].first >= 0 && neighbours[i].first < grid.size() && neighbours[i].second >= 0 && neighbours[i].second < grid[0].size() && grid[neighbours[i].first][neighbours[i].second] == 1) {
				grid[neighbours[i].first][neighbours[i].second] = 0;
				std::vector<std::pair<int, int>> new_path = curr_path;
				new_path.push_back(std::make_pair(neighbours[i].first, neighbours[i].second));
				queue.push(new_path);
			}
		}
		queue.pop();
	}
	return {};
}


