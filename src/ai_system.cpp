// internal
#include "ai_system.hpp"
#include <iostream>
#include <random>

#include <vector>
#include <chrono>

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
	Game& game = registry.game.get(player_doll);

	if (game.battle_state == BattleState::ENEMY_TURN) {
		do_enemy_turn();
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
