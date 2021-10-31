// internal
#include <iostream>
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

void checkFakeCollision(Entity e) {
	ComponentContainer<Motion>& motion_container = registry.motions;
	Motion m = registry.motions.get(e);
	std::vector<Entity> entities = registry.motions.entities;

	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		if (motion_i.collision_proof == 1 || (unsigned int)entities[i] == (unsigned int)e) {
			continue;
		}

		if (collides(motion_i, m))
		{
;			registry.remove_all_components_of(e);
			return;
		}
	}

}

std::function<void(Entity)> PhysicsSystem::getCollisionFunction() {
	return checkFakeCollision;
}


void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	// Move entity based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	ComponentContainer<Motion>& motion_container = registry.motions;

	if (registry.game.get(registry.players.entities[0]).state == GameState::PLAYING) {
		for (uint i = 0; i < motion_registry.size(); i++)
		{
			Motion& motion = motion_registry.components[i];
			Entity entity = motion_registry.entities[i];
			float step_seconds = 1.0f * (elapsed_ms / 1000.f);
			// Check that step does not take player out of bounds
			vec2 potential_pos = motion.position + (motion.velocity * step_seconds);
			if (potential_pos[0] > 40 && potential_pos[0] < window_width_px  - 40 && potential_pos[1] > 40 && potential_pos[1] < window_height_px - 40) {
				motion.position = motion.position + (motion.velocity * step_seconds);
			}
		}

		// Check for collisions between all moving entities
		for (uint i = 0; i < motion_container.components.size(); i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];
			for (uint j = 0; j < motion_container.components.size(); j++) // i+1
			{
				if (i == j)
					continue;

				Motion& motion_j = motion_container.components[j];
				if (collides(motion_i, motion_j))
				{
					Entity entity_j = motion_container.entities[j];
					// Create a collisions event
					// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
					registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				}
			}
		}
	}



	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
        vec2 verticalLineScale = {5, 100};
        vec2 horizontalLineScale = {100, 5};

        printf("in debug mode \n");
        // create a red line around the wall boxes
        for (Entity wallBox: registry.walls.entities) {
            printf("looping through wall \n");
            Motion& wallMotion = registry.motions.get(wallBox);
	        vec2 position = wallMotion.position;
            printf("trying to create wall box \n");
            std::cout << position.x << ", " << position.y << std::endl;
//            createBox(position, verticalLineScale, horizontalLineScale);
        }
        Entity playerDoll = registry.players.entities[0];
        Motion& dollMotion = registry.motions.get(playerDoll);
        vec2 dollPosition = dollMotion.position;
//        createBox(dollPosition, verticalLineScale, horizontalLineScale);

        for (Entity enemy: registry.currentEnemies.entities) {
            Motion& enemyMotion = registry.motions.get(enemy);
            vec2 position = enemyMotion.position;
//            createBox(position, verticalLineScale, horizontalLineScale);
        }
	}
}