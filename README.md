# team11

# Milestone 2
Here is a summary of what to expect with our Milestone 2 implementation:

**State machine**: The enemy AI determines what action to do depending on the statuses, health, and health difference of both the enemy and the player, as well as whether it has an item to use during battle.

**Simple path finding**:
The enemy finds the fastest route to get to the player based on BFS in a projected grid of available area to move in, accounting for walls and other obstacles. 

**Sprite animation**:
When the player is moving with the WASD keys, a walking motion is displayed. The enemies (bunnies) have the animation where their ears constantly move up and down. 

**Adaptive resolution**:
The window size can be adjusted (either make the window bigger or smaller). 

**External integration**: Used [nlohmann's JSON parsing library](https://github.com/nlohmann/json) to parse pre-saved level configurations.  This will be further utilized to build up a larger collection of levels without hardcoding values.

**Debugging graphics**:
Pressing the '/' key will render the bounding boxes around the entities to allow for easier graphics debugging. It will also display the grid nodes that an enemy can move to, illustrating pathing behaviour.

**Level loading**:
When the player defeats 5 enemies, they will progress to the next level. Currently, once the player beats 5 enemies in the last level (room 2) they will return to the start screen. 

**Story**:
Dialogue boxes are used as a story telling device. These boxes show what the character is saying, the diary entries, and narration. The player can move through the dialogue by pressing the spacebar. Currently the dialogue is triggered when the player enters a new level (and is not in the tutorial). TheDollhouseStoryPlan.pdf contains a detailed description of the general story as well as the story for future rooms. 

**Other information**:
Graphic assets were created for new rooms and dialogue boxes. 
Wall collisions (buggy from M1) have been entirely rewritten and are now bug-free and far more versatile. 
