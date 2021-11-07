# Physics II - Pinball
This is a pinball game created by Pablo, Anna, Miguel for Physics II. This project was created using C++ 
and box2D library.

Author's github links:

Pablo
https://github.com/Xymaru

Anna
https://github.com/aNnAm2606

Miguel
https://github.com/migon25

Controls
Space to start the game, or restart after game is lost.

Left arrow key for left flipper and Right arrow key for right flipper.

Down arrow key launching the ball.


Debug Features

F1 draws colliders

F2 key to lose the game

Github Link to the project can be found here: https://github.com/aNnAm2606/Physics-II---Pinball


Special features:
Originally player starts with 3 balls.
The objective is to hit yellow birds: small birds give 5 points, big bird gives 15 points.
Score is shown in-game. It includes high score, current score, previous score. Previous score renewed after game is lost, so is the high score, if new high
score is achieved.
All the art was drawn from scratch and animated by our group. (some references mau have been used from the internet)
The score numbers are 100% authentic, created by our group members.
When player hits 6 birds, he gets an extra ball to play with.

 
Some of the bugs encountered:
We found a bug in METERS_TO_PIXEL and PIXEL_TO_METERS macros, when we called them sending a value which was an operation, we would get the wrong results, that was because the macro joined a
multiplication to the operation instead of doing the operation first. We fixed this by just adding parenthesis for the value passed.

Everytime we restarted the level, all objects were duplicated, we fixed this by clearing all physics bodies before starting the game level.