// ############################################################################
// main.c: initialize game and create tasks
// ############################################################################

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <allegro.h>
#include "init.h"

// Local variables
int last_proc = 0;				// Last created thread							
int max_proc = 18; 				// Max concurrent thread

// Create tasks for the balls
void createBall(){
	sem_wait(&semmain);
	int i = 0;
    tpars   params;
    params = TASK_SPEC_DFL;
    params.period = tspec_from(PER, MILLI);
    params.rdline = tspec_from(DREL, MILLI);
    params.priority = PRIO - last_proc;
    params.measure_flag = 1;
    params.act_flag = NOW;
    params.processor = last_proc++;
    if (last_proc >= max_proc) {
        last_proc = 0;
    }
	i = ptask_create_param(ball_task, &params);
	if (i != -1) {
        printf("Task %d created and activated\n", i);
    } else {
        allegro_exit();
        printf("Error in creating task ball!\n");
        exit(-1);
    }
	sem_post(&semmain);
}

// Create task for the graphic
void createGraphic(){
	sem_wait(&semmain);
	int i = 0;
    tpars   params;
    params = TASK_SPEC_DFL;
    params.period = tspec_from(PER, MILLI);
    params.rdline = tspec_from(DREL, MILLI);
    params.priority = PRIO;
    params.measure_flag = 1;
    params.act_flag = NOW;
    params.processor = last_proc++;
    if (last_proc >= max_proc) {
        last_proc = 0;
    }
	i = ptask_create_param(graphics_task, &params);
	if (i != -1) {
        printf("Task %d created and activated\n", i);
    } else {
        allegro_exit();
        printf("Error in creating task!\n");
        exit(-1);
    }
	sem_post(&semmain);
}

// Create task for the cue
void createUser(){
	sem_wait(&semmain);
	int i = 0;
    tpars   params;
    params = TASK_SPEC_DFL;
    params.period = tspec_from(PER, MILLI);
    params.rdline = tspec_from(DREL, MILLI);
    params.priority = PRIO;
    params.measure_flag = 1;
    params.act_flag = NOW;
    params.processor = last_proc++;
    if (last_proc >= max_proc) {
        last_proc = 0;
    }
	i = ptask_create_param(cue_task, &params);
	if (i != -1) {
        printf("Task %d created and activated\n", i);
    } else {
        allegro_exit();
        printf("Error in creating task!\n");
        exit(-1);
    }
	sem_post(&semmain);
}

// ############################################################################
// Function called at the beginning of the extecution. Initialize
// all the environment and start the tasks. The tasks can run
// until the user presses the ESC button
// ############################################################################
int main(){
    init();
	int i = 0;
	createGraphic();
	createUser();
	for (i = 0; i < BALLSNUMBER; i++) {
		createBall();
	}	
    while (!key[KEY_ESC]) {
    };
    for (int i = 0; i < BALLSNUMBER; i++) {
        destroy_bitmap(ballsbmp[i]);
    }
    destroy_bitmap(cuebmp);
	destroy_bitmap(table);
    destroy_bitmap(buffer);
	allegro_exit();
	return 0;
}