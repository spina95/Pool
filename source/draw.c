// ############################################################################
// draw.c: render task and function which draw cue, balls
// and table every period. Contains also the function to
// compute the trajectory if enabled and to draw it
// ############################################################################

#include "init.h"

// Check if the drawer task misses the deadline
void check_deadline_drawer() {
  if (ptask_deadline_miss()) {
    sem_wait(&semmain);
    printf("Drawer Task has missed its deadline\n");
    sem_post(&semmain);
  }
}

// Convert degrees to radians
float to_radians(float angle) { 
    double pi = 3.14159265359; 
	float radians = angle * pi / 180;
	return radians;
} 

// Convert radians to degrees
float to_degrees(float radians) { 
    double pi = 3.14159265359; 
	float angle = radians*(180/pi);
	if (angle < 0)
		angle += 360;
	return angle;
} 

// Check if all the balls are pocketed
void checkWin() {
    bool win = true;
	int	i = 0;
    for(i = 0; i < BALLSNUMBER - 1; i++){
        if(ballsArray[i].pocketed == false && i != WHITEBALLINDEX)
            win = false;
    }
    if(win == true)
        textout_ex(buffer, font, "You won", RIGHTBORDER - 20, 80, makecol(255, 255 , 255), -1);
}

// Draw the table
void drawTable() {
    draw_sprite(buffer, background, 0, 0);
    draw_sprite(buffer, table, 0, 100);
}

// Draw all the balls
void drawBalls() {
	int i = 0;
    for(i = 0; i < BALLSNUMBER; i++) {
        draw_sprite(buffer, ballsbmp[i], ballsArray[i].x, ballsArray[i].y);
    }
}

// Find the wall which the trajectory hits at the next step
int findTrajectoryWall(int x0, int y0, float angle){
	float angletl, angletr, anglebl, anglebr;
	angletl = to_degrees(atan2(holePositiony[0] - y0, holePositionx[0] - x0));
	angletr = to_degrees(atan2(holePositiony[2] - y0, holePositionx[2] - x0));
	anglebl = to_degrees(atan2(holePositiony[3] - y0, holePositionx[3] - x0));
	anglebr = to_degrees(atan2(holePositiony[5] - y0, holePositionx[5] - x0));

	if(to_degrees(angle) > anglebr && to_degrees(angle) <= anglebl)		// Bottom
		return 1;
	if(to_degrees(angle) > anglebl && to_degrees(angle) <= angletl)		// Left
		return 2;
	if(to_degrees(angle) > angletl && to_degrees(angle) <= angletr)		// Top
		return 3;
	return 0;															// Right
}

// Compute the trajectory point of collision. Return the point and the future angle
struct pointStruct handleTrajectoryWallCollision(int x0, int y0, float angle){
	float m = tan(angle);
	float q = y0 - m * x0;
	float cosx, sinx;
	struct pointStruct nextPoint;
	int wall = findTrajectoryWall(x0, y0, angle);

	switch (wall) { 
	case 0:
		nextPoint.x = holePositionx[2];
		nextPoint.y = y0 + m * (nextPoint.x - x0);
		cosx = -cos(angle);
        sinx = sin(angle);
        nextPoint.angle = atan2(sinx, cosx);
		break;
	case 1:
		nextPoint.y = holePositiony[3];
		nextPoint.x = (nextPoint.y - q) / m;
		cosx = cos(angle);
        sinx = -sin(angle);
        nextPoint.angle = atan2(sinx, cosx);
		break;
	case 2:
		nextPoint.x = holePositionx[0];
		nextPoint.y = y0 + m * (nextPoint.x - x0);
		cosx = -cos(angle);
        sinx = sin(angle);
        nextPoint.angle = atan2(sinx, cosx);
		break;
	case 3:
		nextPoint.y = holePositiony[0];
		nextPoint.x = (nextPoint.y - q) / m;
		cosx = cos(angle);
        sinx = -sin(angle);
        nextPoint.angle = atan2(sinx, cosx);
		break;
	default:
		break;
	}
	return nextPoint;
}

// Compute the collision of the trajectory with the balls. Return the point of collision
struct pointStruct handleTrajectoryBallCollision(int x0, int y0, float angle){
	float m = tan(angle);
	float q = y0 - m * x0;
	int i = 0;
	struct pointStruct nextPoint;
	nextPoint.x = ballsArray[WHITEBALLINDEX].x;
	nextPoint.y = ballsArray[WHITEBALLINDEX].y;
	float minDist = 0;
	for (i=0; i<BALLSNUMBER; i++){
		if (i != WHITEBALLINDEX && ballsArray[i].pocketed == false){
			float ballsAngle = atan2(ballsArray[i].y - y0, ballsArray[i].x - x0);
			float bx, by;
			bx = ballsArray[i].x + BALLRADIUS / 2;
			by = ballsArray[i].y + BALLRADIUS / 2;
			float dist = abs(by - (m * bx + q)) / sqrt(1 + pow(m, 2));
			
			float anglediff = to_degrees(ballsAngle) - to_degrees(angle);
			if (dist < BALLRADIUS / 2 && anglediff > -30 && anglediff < 30) {
				float m1;
				m1 = -1/m;
				float x1 = (y0 - m * x0 - by + m1 * bx) / (m1 - m);
				float y1 = m * x1 + y0 - m * x0;
				float d = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
				if (minDist == 0) {
					minDist = d;
					nextPoint.x = x1;
					nextPoint.y = y1;
				}
				if (d < minDist) {
					nextPoint.x = x1;
					nextPoint.y = y1;
					minDist = d;
				}
			}
		}
	}
	return nextPoint;
}

// ############################################################################
// If the trajectory is enabled the function computes its collisions
// with the walls and the balls and draws it on the screen
// ############################################################################
void drawTrajectory() {
	rectfill(trajectorybuffer, 0, 0, XWIN, YWIN, transparent);
    if(!isBallsMoving() && cue.showTracjetory == true) {
        int	x0, y0;
		float angle = cue.angle;
        bool check = false;
        int count = 0;
        x0 = ballsArray[WHITEBALLINDEX].x + BALLRADIUS / 2;
        y0 = ballsArray[WHITEBALLINDEX].y + BALLRADIUS / 2;
		struct pointStruct nextPoint;
		float sinx = sin(angle);
		float cosx = cos(angle);
		angle = atan2(-sinx, -cosx);
		rectfill(trajectorybuffer, 0, 0, XWIN, YWIN, transparent);
		while (check == false) {
			nextPoint = handleTrajectoryBallCollision(x0, y0, angle);
			if(nextPoint.x != ballsArray[WHITEBALLINDEX].x && \
			 nextPoint.y != ballsArray[WHITEBALLINDEX].y){
				line(buffer, x0, y0, nextPoint.x, nextPoint.y, makecol(255,255,255));
				check = true;
			} else {
				nextPoint = handleTrajectoryWallCollision(x0, y0, angle);
				line(trajectorybuffer, x0, y0, nextPoint.x, nextPoint.y, makecol(255,255,255));
				x0 = nextPoint.x;
				y0 = nextPoint.y;
				angle = nextPoint.angle;
				count++;
				if(count == 3)
					check = true;
			}
		}
    }
}

// Draw the cue
void drawCue() {
	rectfill(cuebuffer, 0, 0, XWIN, YWIN, transparent);
    if(!isBallsMoving()) {
		if (buffer != NULL && cuebmp != NULL){
			if(cue.mode == 0)
				pivot_scaled_sprite(cuebuffer, cuebmp, ballsArray[WHITEBALLINDEX].x + BALLRADIUS / 2,\
				ballsArray[WHITEBALLINDEX].y + BALLRADIUS / 2, -50, 20, cue.angle0, ftofix(0.7));
			else if(cue.mode == 1)
				pivot_scaled_sprite(cuebuffer, cuebmp, ballsArray[WHITEBALLINDEX].x + BALLRADIUS / 2,\
				ballsArray[WHITEBALLINDEX].y + BALLRADIUS / 2, -cue.distance, 20, cue.angle0, ftofix(0.7));
			else if(cue.mode == 2) {
				pivot_scaled_sprite(cuebuffer, cuebmp, ballsArray[WHITEBALLINDEX].x + BALLRADIUS / 2,\
				ballsArray[WHITEBALLINDEX].y + BALLRADIUS / 2, -cue.distance, 20, cue.angle0, ftofix(0.7));
			}
		}
    }
}

// Update the balls speed if they collide
void handleBallsCollisions(){
	int	i = 0, j = 0;
    for (i = 0; i < BALLSNUMBER; i++) {
        for (j = 0; j < BALLSNUMBER; j++) {
            if (i!=j && ballsArray[i].pocketed != true && ballsArray[j].pocketed != true) {
                int	dx = ballsArray[j].x - ballsArray[i].x;
                int	dy = ballsArray[j].y - ballsArray[i].y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist <= BALLRADIUS-1) {
                    float normalX = dx / dist;
                    float normalY = dy / dist;
                    float midpointX = (ballsArray[i].x + ballsArray[j].x) / 2;
                    float midpointY = (ballsArray[i].y + ballsArray[j].y) / 2;
                    ballsArray[i].x = midpointX - normalX * BALLRADIUS;
                    ballsArray[i].y = midpointY - normalY * BALLRADIUS;
                    ballsArray[j].x = midpointX + normalX * BALLRADIUS;
                    ballsArray[j].y = midpointY + normalY * BALLRADIUS;
                    float dVector = (ballsArray[i].vx - ballsArray[j].vx) * normalX;
                    dVector += (ballsArray[i].vy - ballsArray[j].vy) * normalY;
                    float dvx = dVector * normalX;
                    float dvy = dVector * normalY;
                    ballsArray[i].vx -= dvx;
                    ballsArray[i].vy -= dvy;
					ballsArray[i].angle = atan2(ballsArray[i].vy, ballsArray[i].vx);
                    ballsArray[j].vx += dvx;
                    ballsArray[j].vy += dvy;
					ballsArray[j].angle = atan2(ballsArray[j].vy, ballsArray[j].vx);
                }
            }
        }
    }
}

// ############################################################################
// Graphic task function which at every period computes the collisions 
// of the balls, render all the bitmaps on the screen and draws
// the trajectory of the white ball if enabled
// ############################################################################
void graphics_task(void) {
    while(1) {   
		int	i = 0;  
        handleBallsCollisions();
        drawTable();
        drawBalls();
		drawTrajectory();
        drawCue();
        draw_sprite(buffer, trajectorybuffer, 0, 0);
		draw_sprite(buffer, cuebuffer, 0, 0);
        textout_ex(buffer, font, "Press T to show/remove the trajectory", \
		 10, 80, makecol(255, 255 , 255), -1);
		checkWin();
        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, XWIN, YWIN);
		for (i = 0; i < BALLSNUMBER; i++) {
            sem_post(&ballsSem[i]);
        }
		sem_post(&semcue);
		check_deadline_drawer();
        ptask_wait_for_period();
    }
}