// ############################################################################
// init.c: initilization of the global variables, loading bitmap images,
// setting allegro and ptask
// ############################################################################

#include "init.h"

// ############################################################################
// Global variables
// ############################################################################

BITMAP    *buffer;															// Bitmap for the buffer
BITMAP    *background;														// Bitmap for the background
BITMAP    *table;															// Bitmap for the table
BITMAP    *ballsbmp[BALLSNUMBER];											// Bitmap for the balls
BITMAP    *cuebmp;															// Bitmap for the cue
BITMAP	  *trajectorybuffer;												// Buffer used fo the trajectory
BITMAP	  *cuebuffer;														// Buffer used for the cue

struct ballStruct    ballsArray[BALLSNUMBER];								// Array for the data of the balls
struct cueStruct     cue;													// Data of the cue

sem_t	ballsSem[BALLSNUMBER];												// Array with the semaphores of the balls
sem_t	semmain;															// Main semaphore
sem_t	semcue;																// Semaphore for the cue

int		ballPositionx[] = {400, 360, 360, 320, 320,\
320, 280, 280, 280, 280, 240, 240, 240, 240, 240, 850};						// Initial balls coordinates x

int		ballPositiony[] = {375, 355, 395, 335, 375,\
415, 315, 355, 395, 435, 295, 335, 375, 415, 455, 375};						// Initial balls coordinates y

int		holePositionx[] = { LEFTBOTDER, (RIGHTBORDER + LEFTBOTDER) / 2,\
 RIGHTBORDER, LEFTBOTDER, (RIGHTBORDER + LEFTBOTDER) / 2, RIGHTBORDER };	// Holes positions x

int 	holePositiony[] = { TOPBORDER, TOPBORDER - 10, TOPBORDER, \
 BOTTOMBORDER, BOTTOMBORDER + 10, BOTTOMBORDER };							// Holes positions y


// Initialize table image
void initTable() {
    BITMAP	*temp;
    temp = load_bmp("asset/table.bmp", NULL);
    if (!temp){
        allegro_message("Error loading table image");
        exit(1);
    }
    table = create_bitmap(TABLEW, TABLEH);
    stretch_blit(temp, table, 0, 0, temp->w, temp->h, 0, 0, table->w, table->h);
    destroy_bitmap(temp); 
}

// Initialize balls images
void initBalls() {
    BITMAP	*whiteBallTemp = load_bmp("asset/ball-white.bmp", NULL);
    if (!whiteBallTemp)
        allegro_message("Error loading white ball image");
    ballsbmp[WHITEBALLINDEX] = create_bitmap(BALLRADIUS, BALLRADIUS);
    stretch_blit(whiteBallTemp, ballsbmp[WHITEBALLINDEX], 0, 0, whiteBallTemp->w, \
	 whiteBallTemp->h, 0, 0, ballsbmp[WHITEBALLINDEX]->w, ballsbmp[WHITEBALLINDEX]->h);
    destroy_bitmap(whiteBallTemp);
    ballsArray[WHITEBALLINDEX].x = ballPositionx[WHITEBALLINDEX];
    ballsArray[WHITEBALLINDEX].y = ballPositiony[WHITEBALLINDEX];

    ballsArray[WHITEBALLINDEX].vy = 0;
    ballsArray[WHITEBALLINDEX].vx = 0;

    int	i = 0;
    char path[] = "asset/ball-";
    char type[] = ".bmp";
    char number[3];
    char string[255];

    for (i = 0; i < BALLSNUMBER-1; i++) {
        memset(string, 0, sizeof(string));
        sprintf(number, "%d", i+1);
        strcat(string, path);
        strcat(string, number);
        strcat(string, type);
        BITMAP *ballTemp = load_bitmap(string, NULL);
        if (ballTemp == NULL) {
            allegro_message("Error loading %s", string);
            exit(1);
        }
        ballsbmp[i] = create_bitmap(BALLRADIUS, BALLRADIUS);
        stretch_blit(ballTemp, ballsbmp[i], 0, 0, ballTemp->w, ballTemp->h, 0, 0,\
		 ballsbmp[i]->w, ballsbmp[i]->h);
        destroy_bitmap(ballTemp);
        ballsArray[i].vx = 0.f;
        ballsArray[i].vy = 0.f;
        ballsArray[i].x = ballPositionx[i];
        ballsArray[i].y = ballPositiony[i];
        ballsArray[i].pocketed = false;
        ballsArray[i].id = -1;
    }
}

// Initialize cue image and buffer
void initCue() {
    cuebmp = load_bmp("asset/cue.bmp", NULL);
    if(!cuebmp) {
        allegro_message("Error loading cue");
        exit(1);
    }
    cue.power = 0;
    cue.angle = 0;
    cue.mode = 0;
	cuebuffer = create_bitmap(XWIN, YWIN);
    rectfill(cuebuffer, 0, 0, XWIN, YWIN, transparent);
	trajectorybuffer = create_bitmap(XWIN, YWIN);
    rectfill(trajectorybuffer, 0, 0, XWIN, YWIN, transparent);
}

// ############################################################################
// Initialization function called at the beginnig of execution. 
// Used to load images, set global variables and initialize
// Allegro and ptask
// ############################################################################
void init() {
    allegro_init();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
	install_keyboard();
    install_mouse();
    srand(time(NULL));
    
    white = makecol(0,0,0);
    black = makecol(255,255,255);
    transparent = makecol(255,0,255);

    clear_to_color(screen, black);

    buffer = create_bitmap(XWIN, YWIN);

    background = create_bitmap(XWIN, YWIN);
    rectfill(background, 0, 0, XWIN, YWIN, white);
    
    initTable();
    initBalls();
    initCue();

    nholed = 0;

    int i = 0;
    for (i = 0; i < BALLSNUMBER; ++i) {
        sem_init(&ballsSem[i], 0, 1);
    }
    sem_init(&semmain, 0, 1);  
    sem_init(&semcue, 0, 1);  

	ptask_init(SCHED_FIFO, GLOBAL, PRIO_INHERITANCE);
}