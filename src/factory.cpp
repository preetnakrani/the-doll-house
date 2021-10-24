#include <../ext/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;

#include "world_init.hpp"
#include "world_system.hpp"
#include <iostream>
#include <filesystem>

using namespace std;
class JsonReader {
public:
    json readFile(const string& fileName) {
        std::string dataPath = data_path() + fileName;
        json obj;
        std::ifstream file(dataPath);
        file >> obj;
        return obj;
    }

    json readGameProgress() {
        string fileName = "/gameData/gameProgress.json";
        return readFile(fileName);
    }

    json readLevel(int level) {
        std::string levelName = "/levels/level_" +  to_string(level) + ".json";
        return readFile(levelName);
    }
};

class Factory {
    public:
         virtual void create(RenderSystem* renderer, json components) = 0;
         static void test() {
             json m;
             string b = "{\"hello\": 5.6555}";
             json test = json::parse(b);
             std::cout << test << std::endl;
             JsonReader j;
             json obj = j.readLevel(1);
             std::cout << obj << endl;
         }
};

class BedFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createBed(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                          components["pos"]["y"].get<float>()),
                          components["dir"].get<Direction>(),
                          components["angle"].get<float>());
    }

};

class LampFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createLamp(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                       components["pos"]["y"].get<float>()),
                  components["dir"].get<Direction>(),
                  components["angle"].get<float>());
    }
};

class TableFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createTable(renderer,
                   vec2(components["pos"]["x"].get<float>(),
                        components["pos"]["y"].get<float>()),
                   components["dir"].get<Direction>(),
                   components["angle"].get<float>());
    }

};

class EnemyFactory {
public:
    void create(RenderSystem* render, WorldSystem* world, json components) {
        world->clearEnemy();
        for(auto& enemy: components) {
            world->addEnemy(enemy["type"].get<std::string>(), enemy["frequency"].get<int>());
        }
    }

};

class MagicFactory {
public:
    static void getMagicAttacks(json components, vector<MagicAttack> * magic) {
        for(auto& spell: components) {
            magic->push_back(createMagicAttack(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["attackType"].get<AttackType>(), spell["damage"].get<int>()));
        }
    }

    static void getMagicEffects(json components, vector<MagicEffect> * magic) {
        for(auto& spell: components) {
            magic->push_back(createMagicEffect(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["isTemporary"].get<bool>(), spell["timer"].get<float>()));
        }
    }

    static void getMagicDefense(json components, vector<MagicDefense> * magic) {
        for(auto& spell: components) {
            magic->push_back(createMagicDefense(spell["name"].get<std::string>(), spell["magicType"].get<MagicType>(), spell["physical_defense_boost"].get<int>(), spell["magic_defense_boost"].get<int>()));
        }
    }

    static void getAttacks(json components, vector<Attack> * attacks) {
        for(auto& attack: components) {
            attacks->push_back(createAttack(attack["name"].get<std::string>(), attack["type"].get<AttackType>(), attack["damage"].get<int>()));
        }
    }
};



class PlayerFactory {
public:
    Entity createDollFactory(RenderSystem* renderer, json components) {
        std::vector<Attack> attacks;
        json attackList = components["attacks"];
        MagicFactory::getAttacks(attackList, &attacks);

//        std::vector<MagicAttack> magic;
//        json magicLists = components["magic"];
//
//        std::vector<MagicAttack> magicAttack;
//        MagicFactory::getMagicAttacks(magicLists["maggicAttack"], &magicAttack);
//
//        std::vector<MagicEffect> magicEffect;
//        MagicFactory::getMagicEffects(magicLists["magicEffect"], &magicEffect);
//
//        std::vector<MagicDefense> magicDefense;
//        MagicFactory::getMagicDefense(magicLists["magicDefense"], &magicDefense);

        return createDoll(renderer, vec2(components["pos"]["x"].get<float>(), components["pos"]["y"].get<float>()), attacks, components["hp"], components["dp"]);

    }

};

class WallFactory : public Factory {
public:
    void create(RenderSystem * renderer, json components) override {
        createWallBlock(
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()),
            vec2(components["scale"]["x"].get<float>(),
                components["scale"]["y"].get<float>())
        );
    }
};

class BackgroundFactory : public Factory {
public:
    void create(RenderSystem* render, json components) override {
        createBackground(
            render,
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()),
            (TEXTURE_ASSET_ID)components["asset_id"].get<int>()
        );
    }
};

class GameProgressFactory {
public:
    GameProgress create(RenderSystem* renderer, json components) {
        Health h = createHealth(components["health"]["health"].get<int>(), components["health"]["healthDecrement"].get<int>());
        std::vector<Attack> attack;
        MagicFactory::getAttacks(components["attacks"], &attack);
        json motion = components["motion"];
        Motion m = createMotion(vec2(motion["scale"]["x"].get<float>(), motion["scale"]["y"].get<float>()),
                                vec2(motion["position"]["x"].get<float>(), motion["position"]["y"].get<float>()),
                                vec2(motion["velocity"]["x"].get<float>(), motion["velocity"]["y"].get<float>()),
                                motion["angle"].get<float>(),
                                motion["dir"].get<Direction>(),
                                motion["collision_proof"].get<bool>());

        return createGameProgress(components["level"].get<int>(), h, m, attack);
    }
};

class MotionFactory {
public:
    Motion create(json motion) {
        Motion m = createMotion(vec2(motion["scale"]["x"].get<float>(), motion["scale"]["y"].get<float>()),
                                vec2(motion["position"]["x"].get<float>(), motion["position"]["y"].get<float>()),
                                vec2(motion["velocity"]["x"].get<float>(), motion["velocity"]["y"].get<float>()),
                                motion["angle"].get<float>(),
                                motion["dir"].get<Direction>(),
                                motion["collision_proof"].get<bool>());
        return m;
    }
};