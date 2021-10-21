// internal
#include "ai_system.hpp"

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
	Attack& chosen_attack = attack_list.available_attacks[0];
	if (!registry.turns.has(current_enemies[0])) {
		registry.turns.insert(current_enemies[0], Turn(chosen_attack.name));
	}
}
