#include "battle_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include <iostream>
using namespace std;

Entity player_doll;
vector<Entity> current_enemies;

// true when battle is ongoing, false when there is no battle happening right now
bool is_active_battle = false;

void BattleSystem::init(WorldSystem* world_system_arg) {
    this->world_system = world_system_arg;
};

void BattleSystem::handle_battle() {
    player_doll = registry.players.entities[0];
    Game& game = registry.game.get(player_doll);
    if (game.state != GameState::BATTLE || registry.currentEnemies.entities.empty()) {
        return;
    }

    if (!is_active_battle) {
        // New battle was initiated so let's initialize relevant variables
        current_enemies = registry.currentEnemies.entities;
        is_active_battle = true;
        printf("=====Initialized battle=====\n");
    }

    // TODO: Modify this to account for multiple enemies
    Health& enemy_health = registry.health.get(current_enemies[0]);
    if (!registry.turns.entities.empty()) {
        Turn& player_turn = registry.turns.get(player_doll);
        AttackList& player_attacks = registry.attackLists.get(player_doll);
        Attack chosen_attack = player_attacks.getAttack(player_turn.move);
        enemy_health.healthDecrement = chosen_attack.damage;
        enemy_health.health -= enemy_health.healthDecrement;
        printf("You used the attack: %s", chosen_attack.name.c_str());
        printf("! The enemy now has health : %d\n",enemy_health.health);

        // Done processing the turn, so remove component from the player
        registry.turns.remove(player_doll);
    }

    if (enemy_health.health < 1) {
        for (int i = 0; i < current_enemies.size(); i++) {
            Entity enemy = current_enemies[0];
            registry.remove_all_components_of(enemy);
            world_system->destroyBattleWindow();
        }
        is_active_battle = false;
        game.state = GameState::PLAYING;
        printf("=====Battle ended=====\n");
    }
};

// TODO: It might be nice to put the battle ending logic into a helper
// Currently it gives me an error - Naoreen
//void BattleSystem::end_battle(Game& game, vector<Entity> current_enemies) {
    
//}