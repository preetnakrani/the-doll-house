#include <../ext/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;

#include "world_init.hpp"


class Factory {
    public:
         virtual void create(json components) {}
};

class BedFactory : public Factory {
public:
    void create(json components) {

    }

};

class LampFactory : public Factory {
public:
    void create(json components) {

    }

};

class TableFactory : public Factory {
public:
    void create(json components) {

    }

};

class EnemyFactory : public Factory {
public:
    void create(json components) {

    }

};

class PlayerFactory : public Factory {
public:
    void create(json components) {

    }

};

class WallFactory : public Factory {
public:
    void create(json components) {
    }

};