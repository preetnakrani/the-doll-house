# team11

# Milestone 1
Here is a summary of what to expect with our Milestone 1 implementation:

**Wall collisions**

Collision resolution was accomplished by stopping the player's motion when they hit the attic wall, or try to walk off screen.
Note that when you walk into the wall at the top, and then try to walk sideways, you may get stuck. This has to do with how we are currently implementing the wall.
We aim to fix this in a future milestone.

**Battle screen**

When you collide with a dust bunny, you will enter battle mode. Currently, the battle feedback is provided in the console.
So please look there for the relevant print statements.
You will want to click the "punch" attack on the battle screen until the enemy's HP reaches 0 (the enemy's HP is printed to the console).
Once you defeat the enemy, you should automatically be exited from the battle.

**Help screen**

Help is available on the wall of the attic. The player can navigate through the help by either pressing N or M. 

**Menu Button** 

By clicking on the menu button located on the top right of the screen, the user can access the overlay menu, from which they can either restart the game or replay the tutorial. Click the menu button again to close the overlay menu. 

**Tutorial**

The tutorial is activated every time the game restarts. The player can skip through the tutorial by pressing 'enter', or close the tutorial by pressing 'space'. If neither key input is detected, each tutorial instruction displays on the screen for 7 seconds. 
