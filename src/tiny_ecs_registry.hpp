#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has

	ComponentContainer<Health> health;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
    ComponentContainer<Enemy> enemies;
	ComponentContainer<Player> players;
    ComponentContainer<Momentum> momentums;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
    ComponentContainer<Attack> attacks;
	ComponentContainer<AttackList> attackLists;
	ComponentContainer<MagicList> magicLists;
	ComponentContainer<GameItem> gameItems;
	ComponentContainer<Game> game;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<CurrentEnemy> currentEnemies;
	ComponentContainer<Turn> turns;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<HelpScreen> helpScreens;
	ComponentContainer<BattleScreen> battleScreens;
    ComponentContainer<BattleDoll> battleDolls;
    ComponentContainer<BattleEnemy> battleEnemies;
	ComponentContainer<BattleMenu> battleMenus;
	ComponentContainer<BattleMenuButton> battleMenuButtons;
	ComponentContainer<BattleMenuPlayerMove> battleMenuPlayerMoves;
	ComponentContainer<TutorialTimer> tutorialTimer;
	ComponentContainer<Wall> walls;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{

		registry_list.push_back(&health);
        registry_list.push_back(&attacks);
		registry_list.push_back(&attackLists);
		registry_list.push_back(&magicLists);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&enemies);
		registry_list.push_back(&momentums);
		registry_list.push_back(&gameItems);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
        registry_list.push_back(&game);
		registry_list.push_back(&currentEnemies);
		registry_list.push_back(&turns);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&helpScreens);
		registry_list.push_back(&battleScreens);
		registry_list.push_back(&battleDolls);
		registry_list.push_back(&battleEnemies);
		registry_list.push_back(&battleMenus);
		registry_list.push_back(&battleMenuButtons);
		registry_list.push_back(&battleMenuPlayerMoves);
		registry_list.push_back(&tutorialTimer);
		registry_list.push_back(&walls);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;