#include <../ext/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;

#include "world_init.hpp"
#include "world_system.hpp"
#include <iostream>
#include <fstream>
using namespace std;

class JsonReader {
public:
    json readFile(int level) {
        std::string levelName = "../data/levels/level_" +  to_string(level) + ".json";
        json obj;
        std::ifstream file(levelName);
        file >> obj;
        return obj;
    }
};

class Factory {
    public:
         virtual void create(json components);
         static void test() {
             JsonReader j;
             json obj = j.readFile(1);
             std::cout << obj << endl;
         }
};

class BedFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) {
        createBed(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                          components["pos"]["y"].get<float>()),
                          components["dir"].get<Direction>(),
                          components["angle"].get<float>());
    }

};

class LampFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) {
        createLamp(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                       components["pos"]["y"].get<float>()),
                  components["dir"].get<Direction>(),
                  components["angle"].get<float>());
    }
};

class TableFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) {
        createTable(renderer,
                   vec2(components["pos"]["x"].get<float>(),
                        components["pos"]["y"].get<float>()),
                   components["dir"].get<Direction>(),
                   components["angle"].get<float>());
    }

};

class EnemyFactory : public Factory {
public:
    void create(RenderSystem* render, WorldSystem* world, json components) {
        world->clearEnemy();
        for(auto& enemy: components) {
            world->addEnemy(enemy["type"].get<std::string>(), enemy["frequency"].get<int>());
        }
    }

};

class PlayerFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) {
        std::vector<Attack> attacks;
        json attackList = components["attacks"];
        for(auto& attack: attackList) {
            attacks.push_back(createAttack(attack["name"].get<std::string>(), attack["type"].get<AttackType>(), attack["damage"].get<int>()));
        }

        std::vector<Magic> magic;
        json magicLists = components["magic"];
        for(auto& spell: magicLists["magicAttack"]) {
            magic.push_back(createMagicAttack(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["attackType"].get<AttackType>(), spell["damage"].get<int>()));
        }
        for(auto& spell: magicLists["magicEffect"]) {
            magic.push_back(createMagicEffect(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["isTemporary"].get<bool>(), spell["timer"].get<float>()));
        }
        for(auto& spell: magicLists["magicDefense"]) {
            magic.push_back(createMagicDefense(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["physical_defense_boost"].get<int>(), spell["magic_defense_boost"].get<int>()));
        }

        createDoll(renderer, vec2(components["pos"]["x"].get<float>(), components["pos"]["y"].get<float>()), attacks, magic, components["hp"], components["dp"]);

    }

};

class WallFactory : public Factory {
public:
    void create(json components) {
    }
};