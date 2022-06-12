// ############################################################################
// ball.c: functions to handle each ball and to compute their position,
// speed and direction at each period. 
// ############################################################################

#include "init.h"

// Assigns the correct index to the balls
int getFreeIndex(){
	int	i = 0;
    for (i = 0; i < BALLSNUMBER - 1; i++){
        if (ballsArray[i].id == -1)
            return i;
    }
    return -1;
}

// Check if the ball task misses the deadline
void check_deadline_ball (int num) {
  if (ptask_deadline_miss()) {
    sem_wait(&semmain);
    printf("Ball Task num. %d has missed its deadline\n", num);
    sem_post(&semmain);
  }
}

// Check if any ball is moving
bool isBallsMoving() {
	int	i = 0;
    for (i = 0; i < BALLSNUMBER; i++) {
        if (ballsArray[i].vx != 0 || ballsArray[i].vy != 0) {
            return true;
        }
    }
    return false;
}

// ############################################################################
// If the white ball is pocketed it is moved to a random position
// which is not occupied from another ball
// ############################################################################
void checkWhiteBallPocketed() {
    if(!isBallsMoving() && ballsArray[WHITEBALLINDEX].pocketed == true) {
        bool condition = false;
        while (condition != true) {
            int	x = rand() % (BOTTOMBORDER + 1 - TOPBORDER) + TOPBORDER;
            int y = rand() % (RIGHTBORDER + 1 - LEFTBOTDER) + LEFTBOTDER;
			int	i = 0;
            for (i=0; i<BALLSNUMBER-1; i++){
                int dx = ballsArray[i].x - x;
                int dy = ballsArray[i].y - y;
                float distance = sqrt(dx*dx + dy*dy);
                if (distance > HOLERADIUS + 20) {
                    condition = true;
                    ballsArray[WHITEBALLINDEX].x = x;
                    ballsArray[WHITEBALLINDEX].y = y;
                    ballsArray[WHITEBALLINDEX].pocketed = false;
                }
            }
        } 
    }
}

// Check if a ball is pocketed 
void handleHoles(int index){
	int	i = 0;
    for (i=0; i < 6; i++) {
        int dx = ballsArray[index].x + BALLRADIUS/2 - holePositionx[i];
        int dy = ballsArray[index].y + BALLRADIUS/2 - holePositiony[i];
        float distance = sqrt(dx*dx + dy*dy);
        if (distance < HOLERADIUS) {
            ballsArray[index].pocketed = true;                
            ballsArray[index].x = 0 + index * (BALLRADIUS + 10);
            ballsArray[index].y = 0;
            ballsArray[index].vx = 0;
            ballsArray[index].vy = 0;
            if (i == WHITEBALLINDEX) {
                ballsArray[index].x = 0;
                ballsArray[index].y = 2 * BALLRADIUS;
            } else
                nholed++;
        }
    }
}

// Handle the collissions with borders of a ball
void handleBorderCollisions(int index) {
    if (ballsArray[index].pocketed != true) {
        if (ballsArray[index].y > BOTTOMBORDER - BALLRADIUS){
            ballsArray[index].y = BOTTOMBORDER - BALLRADIUS; 
            ballsArray[index].vy = ballsArray[index].vy * -1;
			ballsArray[index].angle = atan2(ballsArray[index].vy, ballsArray[index].vx);
        }

        if (ballsArray[index].y < TOPBORDER) {
            ballsArray[index].y = TOPBORDER;
            ballsArray[index].vy = ballsArray[index].vy * -1;
			ballsArray[index].angle = atan2(ballsArray[index].vy, ballsArray[index].vx);
        }

        if (ballsArray[index].x < LEFTBOTDER) {
            ballsArray[index].x = LEFTBOTDER;
            ballsArray[index].vx = ballsArray[index].vx * -1;
			ballsArray[index].angle = atan2(ballsArray[index].vy, ballsArray[index].vx);
        }

        if (ballsArray[index].x > RIGHTBORDER - BALLRADIUS) {
            ballsArray[index].x = RIGHTBORDER - BALLRADIUS;
            ballsArray[index].vx = ballsArray[index].vx * -1;
			ballsArray[index].angle = atan2(ballsArray[index].vy, ballsArray[index].vx);
        }
    }
}

// Update ball state based on its position
void update(int index, float dt) {
    handleBorderCollisions(index);
    handleHoles(index);
    checkWhiteBallPocketed();
        
    if (abs(ballsArray[index].vx) < 0.1 || abs(ballsArray[index].vy) < 0.1) {
        ballsArray[index].vy = 0.0;
        ballsArray[index].vx = 0.0;
    }
    if (ballsArray[index].vx != 0 || ballsArray[index].vy != 0){
		
		float angle = ballsArray[index].angle;

        ballsArray[index].x += round(ballsArray[index].vx * dt - 0.5 * \
		 ATTR * cos(angle) * dt * dt);
        ballsArray[index].y += round(ballsArray[index].vy * dt - 0.5 * \
		 ATTR * sin(angle) * dt * dt);

        float v = sqrt(ballsArray[index].vx * ballsArray[index].vx + \
		 ballsArray[index].vy * ballsArray[index].vy);
		v = v - ATTR * dt;
		ballsArray[index].vx = v * cos(angle);
		ballsArray[index].vy = v * sin(angle);
    }
}

// ############################################################################
// Task function for each ball which update at each period the status
// of the ball. It also computes the collision of the balls with the walls
// and check if the balls are pocketd in an hole.
// ############################################################################
void ball_task(void) {
    sem_wait(&semmain);
    int index = getFreeIndex();
    if (index == -1) {
        index = WHITEBALLINDEX;
    }
    ballsArray[index].id = index;  

    int i = ptask_get_index();
    float dt = ptask_get_period(i, MILLI) / 100.;
    sem_post(&semmain);

    while (1) {
        sem_wait(&ballsSem[index]);
        update(index, dt);
        check_deadline_ball(index);
        ptask_wait_for_period();
    }
}