# team11

# Milestone 2
Here is a summary of what to expect with our Milestone 2 implementation:

**State machine**: The enemy AI determines what action to do depending on the statuses, health, and health difference of both the enemy and the player, as well as whether it has an item to use during battle.

**Simple path finding**:
The enemy finds the fastest route to get to the player based on BFS in a projected grid of available area to move in. 

**Sprite animation**:
When the player is moving with the WASD keys, a walking motion is displayed. The enemies (bunnies) have the animation where their ears constantly move up and down. 

**Adaptive resolution**:
The window size can be adjusted (either make the window bigger or smaller). 

**External integration**: Used an external library.

**Debugging graphics**:
Pressing a key will render the bounding boxes around the entities to allow for easier graphics debugging. 

**Level loading**:
When the player defeats 5 enemies, they will progress to the next level. As of now, once the player beats 5 enemies in the last level (room 2) they will return to the start screen. 

**Story**:
Dialogue boxes are used as a story telling device. These boxes show what the character is saying, the diary entries, and narration. TheDollhouseStoryPlan.pdf contains a detailed description of the general story as well as the story for future rooms. 

**Other information**:
Graphic assets were created for new rooms and dialogue boxes. 
