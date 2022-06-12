// ############################################################################
// cue.c: contains all the functions to handle the cue and
// to compute its animations
// ############################################################################

#include "init.h"

// Check if the cue task misses the deadline
void check_deadline_cue() {
  if (ptask_deadline_miss()) {
    sem_wait(&semmain);
    printf("Cue Task has missed its deadline\n");
    sem_post(&semmain);
  }
}

// Check if the button for enabling the trajectory is pressed
void trajectoryHandle(){
    if(keypressed()) {
        int c = readkey();
        int k = c >> 8;
        if(k == KEY_T) {
            if(cue.showTracjetory == false)
                cue.showTracjetory = true;
            else
                cue.showTracjetory = false;
        }
    }
}

// Change the state of the cue if the mouse button is pressed
void cueHandle() {
    if (mouse_b & 1 && mouseClicked == false) {
        if(cue.mode == 0) { 
            cue.mode = 1;
        }
        else if (cue.mode == 1){ 
            cue.mode = 2;
        }
        mouseClicked = true;
    }
    else if(mouse_b == 0) {
        mouseClicked = false;
    }
    if (cue.mode == 2) { 
        if(cue.distance <= BALLRADIUS/2) {
            ballsArray[WHITEBALLINDEX].vx = cue.power * cos(cue.angle + 3.14);
            ballsArray[WHITEBALLINDEX].vy = cue.power * sin(cue.angle + 3.14);
			ballsArray[WHITEBALLINDEX].angle = cue.angle + 3.14;
            cue.mode = 0;
            mouseClicked = true;
        }
    } 
}

// ############################################################################
// The function finds at every period the position of the mouse on the screen 
// and computes the angle and the distance from the white ball of the cue.
// The cue has 3 modes:
// - 0: the user chooses the angle
// - 1: the user chooses the power
// - 2: the cue is performing the animation 
// ############################################################################
void cue_task(void) {
    while (1) {
		sem_wait(&semcue);
        trajectoryHandle();
        if(!isBallsMoving()) {
            cueHandle();
            int m_x = mouse_x;
            int m_y = mouse_y;
            float delta_x = m_x - ballsArray[WHITEBALLINDEX].x - BALLRADIUS / 2;
            float delta_y = m_y - ballsArray[WHITEBALLINDEX].y - BALLRADIUS / 2;
            float angle = atan2(delta_y, delta_x);

            if(cue.mode == 0) {
                cue.angle = angle;
                cue.angle0 = fixatan2(itofix(delta_y), itofix(delta_x));
                cue.mousex0 = mouse_x;
                cue.mousey0 = mouse_y;
            }
            if(cue.mode == 1) {
                int dx = mouse_x - ballsArray[WHITEBALLINDEX].x - BALLRADIUS / 2;
                int dy = mouse_y - ballsArray[WHITEBALLINDEX].y - BALLRADIUS / 2;
                cue.distance = sqrt(dx * dx + dy * dy);
                
                if(cue.distance > MAXDISTANCE)
                    cue.distance = MAXDISTANCE;
                if(cue.distance < BALLRADIUS / 2 + 20)
                    cue.distance = BALLRADIUS / 2 + 20;
                cue.power = (MAXPOWER * cue.distance / MAXDISTANCE);
            }
            if(cue.mode == 2){
                cue.distance -= 10;
                if(cue.distance <= BALLRADIUS / 2)
                    cue.distance = BALLRADIUS / 2;
            }
			
        }    
        check_deadline_cue();
        ptask_wait_for_period();
    }    
}