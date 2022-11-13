# Level Editing Tutorial

This is the tutorial to let the players create their own levels by creating waves. You will be able to add to each wave a lot of different mobs such as the “Blop”, the “Crop”, ...

##	Setup

Go to `./assets/levels/` and create a new file. Name it whatever you want and end it with “.level”.
```
Example : BigBossLevel.level
```

Open the file you just created and write these two lines :
-	First line : the level’s name
-	Second line : just write “0”. This line exists to know if you already have finished the level (1 means already finished)
```
Example : BigBossLevel.level
          0
```

##	Create waves


**Step 1:**

To create a wave got to a new line and write “W” followed by the number of the wave.
```
Example : W1 for the wave number 1
```
**Step 2:**

Just after the line create a new line and write the ID of the mobs you want to spawn during the wave spaced by a space. Monsters ID are described at the bottom of this tutorial in the part 4) Mobs.
```
Example : 0 0 0 1 1 1 0 0 1 1 1
```
To create another wave just refollow the step

##	Mobs

Mobs | ID | Attack | Life |
--- | --- | --- | --- |
Blop | 0 | 1 | 5 |
Crop | 1 | 1 | 10 |
Boss | 2 | 1 | 50 |

*Player's attack is 5 and player's life is 1

##	How to play ?

Launch the game and go to the single player mode. Search you game and launch it. If the title is green, it means that you already have finished the level.
