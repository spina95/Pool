# POOL GAME
This is a project for the exam of Real-Time Embedded Systems which simulates a pool game.

### How to launch
Open a terminal inside the game directory and launch the command
```
$ make
$ sudo ./main
```

### Instruction to play the game
This is a simulator of a pool game. The user controls the cue and he has to put all the balls in the holes except for the white ball.
If the white ball will be pocketed it will be positioned randomly on the table. 
The user has to control the cue, deciding its angle and its power.

- To choose the angle of the cue move the cursor around the white ball then press the left button of the mouse
- To choose the power of the cue move the cursor. The power will be proportional to the distance of the cursor from the white ball. Press the left button of the mouse to confirm
- Press **t** to show the white ball trajectory
- Press **esc** to quit from the game

### Tasks 
The tasks used are the followings:

- #### Balls
Each ball is a periodic task which updates its position and speed. It also handles the collisions with walls and checks if the ball is pocketed in a hole

- #### Cue
The cue is a periodic task which takes as input the mouse position and sets the coordinates and the angle of the cue if none of the balls is moving. It also sets the power and the angle of the shot and check if the 't' button is pressed to show the trajectory

- #### Draw
The task is used to render all the objects on the screen. It also handles the collisions between the balls and computes the trajectory of the white ball if enabled

### Tasks diagram

![Alt text](pool.jpg?raw)

### Screenshot

![Alt text](screenshot.png?raw)
