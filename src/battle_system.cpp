#include "battle_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"

#include <iostream>
using namespace std;

void BattleSystem::init(WorldSystem* world_system_arg) {
    this->world_system = world_system_arg;
};

void BattleSystem::handle_battle() {
    Entity& player_doll = registry.players.entities[0];
    Game& game = registry.game.get(player_doll);
    if (game.state != GameState::BATTLE || registry.currentEnemies.entities.empty()) {
        return;
    }

    if (game.battle_state == BattleState::NONE) {
        // New battle was initiated so let's initialize relevant variables
        Entity& current_enemies = registry.currentEnemies.entities[0];
        game.battle_state = BattleState::START;
        printf("=====Initialized battle=====\n");
    }

    else if (game.battle_state == BattleState::START) {
        game.battle_state = BattleState::PLAYER_TURN;
    }

    else if (game.battle_state == BattleState::PLAYER_TURN && registry.turns.has(player_doll)) {
      // ai.AISystem::step(500);
        // Process the player's turn, then remove their turn component and update battle state
        process_player_turn(registry.turns.get(player_doll));
        registry.turns.remove(player_doll);
        Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
        game.battle_state = enemy_health.health < 1 ? BattleState::WON : BattleState::ENEMY_TURN;
        
    }

    else if (game.battle_state == BattleState::ENEMY_TURN && registry.turns.has(registry.currentEnemies.entities[0])) {
        //ai.AISystem::step(500);
        process_enemy_turn(registry.turns.get(registry.currentEnemies.entities[0]), registry.currentEnemies.entities[0]);
        registry.turns.remove(registry.currentEnemies.entities[0]);
        game.battle_state = BattleState::PLAYER_TURN;
        Health& player_health = registry.health.get(player_doll);
        game.battle_state = player_health.health < 1 ? BattleState::LOST: BattleState::PLAYER_TURN;
    }

    else if (game.battle_state == BattleState::WON) {
        printf("You win! :D \n");
        // TODO: additional battle winning logic like adding XP
        end_battle(game, registry.currentEnemies.entities);
    }

    else if (game.battle_state == BattleState::LOST) {
        printf("You lose! :( \n");
        // TODO: additional battle losing logic like losing lives or restarting game, for now just reset health
        Health& player_health = registry.health.get(player_doll);
        player_health.health = 100;
        player_health.healthDecrement = 0;
        end_battle(game, registry.currentEnemies.entities);
    }
};

void BattleSystem::process_player_turn(Turn& player_turn) {
    Entity& player_doll = registry.players.entities[0];
    if (player_turn.move_type == MoveType::ATTACK) {
        Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
        AttackList& player_attacks = registry.attackLists.get(player_doll);
        Attack chosen_attack = player_attacks.getAttack(player_turn.attack_name);
        enemy_health.healthDecrement = chosen_attack.damage;

        // Change this later when poison included
        Status& enemy_status = registry.status.get(registry.currentEnemies.entities[0]);
        if (enemy_status.POISONED) {
            if (enemy_status.timer <= 0) {
                enemy_status.POISONED = false;
                //return;
            }
            else {
                enemy_health.healthDecrement += 3;
                enemy_status.timer -= 1;
            }
           
            
        }
        else if (enemy_status.SHIELDED) {
            if (enemy_status.timer <= 0) {
                enemy_status.SHIELDED = false;
                //return;
            }
            else {
                enemy_health.healthDecrement *= 0.4;
                enemy_status.timer -= 1;
            }
            
            
            
            
        }
        if (registry.status.get(player_doll).TAUNTED) {
            Status& player_status = registry.status.get(player_doll);
            
            if (player_status.timer <= 0) {
                player_status.TAUNTED = false;
                player_status.timer = 0;
           
            }
            else {
                player_status.timer -= 1;
            }
        }

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

void BattleSystem::process_enemy_turn(Turn& enemy_turn, Entity& enemy) {
    Entity& player_doll = registry.players.entities[0];
    Health& player_health = registry.health.get(player_doll);
    if (enemy_turn.move_type == MoveType::ATTACK) {
        AttackList& enemy_attacks = registry.attackLists.get(enemy);
        Attack chosen_attack = enemy_attacks.getAttack(enemy_turn.attack_name);
        player_health.healthDecrement = chosen_attack.damage;

        // Change this later when poison included
        Status& player_status = registry.status.get(player_doll);
        if (player_status.POISONED) {
            
            if (player_status.timer <= 0) {
                player_status.POISONED = false;
            }
            else {
                player_health.healthDecrement += 3;
                player_status.timer -= 1;
            }
        }
        else if (player_status.SHIELDED) {
            
            if (player_status.timer <= 0) {
                player_status.SHIELDED = false;
            }
            else {
                player_health.healthDecrement *= 0.4;
                player_status.timer -= 1;
            }
        }

        player_health.health -= player_health.healthDecrement;
        printf("The enemy used the move: %s", chosen_attack.nameAsString().c_str());
        printf("!, Your health : %d\n\n", player_health.health);
    }
    else if (enemy_turn.move_type == MoveType::MAGIC) {
        Entity& player_doll = registry.players.entities[0];
        MagicList& enemy_attacks = registry.magicLists.get(enemy);
        Magic chosen_attack = enemy_attacks.getAttack(enemy_turn.magic_name);
        Status& enemy_status = registry.status.get(enemy);
        Status& player_status = registry.status.get(player_doll);

        if (chosen_attack.magicType == MagicType::DEFENSE) {
            enemy_status.SHIELDED = true;
            player_status.TAUNTED = false;
            player_status.POISONED = false;
            enemy_status.timer = chosen_attack.timer;
          //  chosen_attack.countdown = chosen_attack.timer;
        }
        else if (chosen_attack.magicType == MagicType::STATUS_EFFECT) {
            player_status.TAUNTED = true;
            player_status.POISONED = false;
            player_status.SHIELDED = false;
            player_status.timer = chosen_attack.timer;
            // chosen_attack.countdown = chosen_attack.timer;
        }
        else if (chosen_attack.magicType == MagicType::ATTACK) {
            void; // no magic attacks at the moment
        }
        printf("The enemy used the move: %s", chosen_attack.nameAsString().c_str());
        printf("!, Your health : %d\n\n", player_health.health);
    }
    else if (enemy_turn.move_type == MoveType::ITEM) {
        Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
        GameItem& enemy_item = registry.gameItems.get(enemy);
        if (enemy_item.item_name == GameItemName::HEALING_POTION) {
            if (enemy_health.health + enemy_item.health > enemy_health.maxHP) {
                enemy_health.health = enemy_health.maxHP;
            }
            else {
                enemy_health.health += enemy_item.health;
            }

            registry.gameItems.remove(enemy);
            printf("The enemy used the move: HEALTH POTION ! \n");
            printf("Your health : %d", player_health.health);
            printf("    Enemy health : %d\n\n", enemy_health.health);
        }
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

    world_system->incrementEnemiesKilled();

}
