#include "battle_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include <iostream>
using namespace std;

Entity player_doll;
vector<Entity> current_enemies;

void BattleSystem::init(WorldSystem* world_system_arg) {
    this->world_system = world_system_arg;
};

void BattleSystem::handle_battle() {
    player_doll = registry.players.entities[0];
    Game& game = registry.game.get(player_doll);
    if (!(game.state == GameState::BATTLE) || registry.currentEnemies.entities.empty()) {
        return;
    }

    if (game.battle_state == BattleState::NONE) {
        // New battle was initiated so let's initialize relevant variables
        current_enemies = registry.currentEnemies.entities;
        game.battle_state = BattleState::START;
        printf("=====Initialized battle=====\n");
    }

    else if (game.battle_state == BattleState::START) {
        game.battle_state = BattleState::PLAYER_TURN;
    }

    else if (game.battle_state == BattleState::PLAYER_TURN && registry.turns.has(player_doll)) {
        // Process the player's turn, then remove their turn component and update battle state
        process_player_turn(registry.turns.get(player_doll));
        registry.turns.remove(player_doll);
        Health& enemy_health = registry.health.get(current_enemies[0]);
        game.battle_state = enemy_health.health < 1 ? BattleState::WON : BattleState::ENEMY_TURN;
    }

    else if (game.battle_state == BattleState::ENEMY_TURN && registry.turns.has(current_enemies[0])) {
        process_enemy_turn(registry.turns.get(current_enemies[0]), current_enemies[0]);
        game.battle_state = BattleState::PLAYER_TURN;
        Health& player_health = registry.health.get(player_doll);
        game.battle_state = player_health.health < 1 ? BattleState::LOST: BattleState::PLAYER_TURN;
    }

    else if (game.battle_state == BattleState::WON) {
        printf("You win! :D \n");
        // TODO: additional battle winning logic like adding XP
        end_battle(game, current_enemies);
    }

    else if (game.battle_state == BattleState::LOST) {
        printf("You lose! :( \n");
        // TODO: additional battle losing logic like losing lives or restarting game, for now just reset health
        Health& player_health = registry.health.get(player_doll);
        player_health.health = 100;
        player_health.healthDecrement = 0;
        end_battle(game, current_enemies);
    }
};

void BattleSystem::process_player_turn(Turn& player_turn) {
    if (player_turn.move_type == MoveType::ATTACK) {
        Health& enemy_health = registry.health.get(current_enemies[0]);
        AttackList& player_attacks = registry.attackLists.get(player_doll);
        Attack chosen_attack = player_attacks.getAttack(player_turn.attack_name);
        enemy_health.healthDecrement = chosen_attack.damage;
        enemy_health.health -= enemy_health.healthDecrement;
        printf("You used the move: %s", chosen_attack.nameAsString().c_str());
        printf("!, Enemy health : %d\n", enemy_health.health);
    }
    else if (player_turn.move_type == MoveType::MAGIC) {
        // TODO
    }
    else if (player_turn.move_type == MoveType::ITEM) {
        // TODO
    }
}

void BattleSystem::process_enemy_turn(Turn& enemy_turn, Entity enemy) {
    if (enemy_turn.move_type == MoveType::ATTACK) {
        Health& player_health = registry.health.get(player_doll);
        AttackList& enemy_attacks = registry.attackLists.get(enemy);
        Attack chosen_attack = enemy_attacks.getAttack(enemy_turn.attack_name);
        player_health.healthDecrement = chosen_attack.damage;
        player_health.health -= player_health.healthDecrement;
        printf("The enemy used the move: %s", chosen_attack.nameAsString().c_str());
        printf("!, Your health : %d\n\n", player_health.health);
    }
    else if (enemy_turn.move_type == MoveType::MAGIC) {
        // TODO
    }
}

void BattleSystem::end_battle(Game& game, std::vector<Entity> current_enemies) {
    for (int i = 0; i < current_enemies.size(); i++) {
        Entity enemy = current_enemies[0];
        registry.remove_all_components_of(enemy);
        world_system->destroyBattleWindow();
    }
    game.battle_state = BattleState::NONE;
    game.state = GameState::PLAYING;
    printf("=====Battle ended=====\n");
}
