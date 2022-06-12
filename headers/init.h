// ############################################################################
// init.h: contains constant variables, structures definitions, bitmaps,
// global variables and function declarations.
// ############################################################################

// ############################################################################
// Libraries
// ############################################################################
#include "ptask.h"
#include <allegro.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdbool.h>
#include <math.h>

// ############################################################################
// Window and pool table constants
// ############################################################################
#define XWIN                1200        // Window width         
#define YWIN                700         // Window height
#define TABLEH              600         // Table height
#define TABLEW              1200        // Table width
#define BOTTOMBORDER        640         // Bottom border position Y
#define LEFTBOTDER          64          // Left border position X
#define TOPBORDER           157         // Top border position Y
#define RIGHTBORDER         1135        // Right border position X
#define HOLERADIUS			37			// Holes radius
#define ATTR 				0.1			// Table friction

// ############################################################################
// Balls contants
// ############################################################################
#define BALLSNUMBER         16          // Balls number
#define WHITEBALLINDEX      15          // White ball index in array
#define BALLRADIUS          35          // Ball radius

// ############################################################################
// Cue contants
// ############################################################################
#define MAXPOWER            180			// Max cue power
#define MAXDISTANCE         200			// Max cue distance from white ball

// ############################################################################
// Tasks contants
// ############################################################################
#define PER                 15          // Task period in ms
#define DREL                30          // Relative deadline in ms
#define PRIO 				80          // task priority

// ############################################################################
// Structure which contains all the informations about a ball
// ############################################################################
struct ballStruct {
    int		id;
    int		x, y;                      	// Ball coordinates
    int		vx, vy;                    	// Ball speed
    bool	pocketed;                  	// 0 ball is on the table, 1 ball is pocketed
	float 	angle;
};

// ############################################################################
// General structure to represent a vector
// ############################################################################
struct pointStruct {		
	float x, y;
	float angle;
};

// ############################################################################
// Structure which contains all the informations about the cue
// ############################################################################
struct cueStruct {
    int		x, y;                       // Cue coordinates
    float	angle;                     	// Cue angle
    int		mode;                       // 0 select angle, 1 select power, 2 hitting animation
    float	power;                      // Cue power
    int		mousex0, mousey0;			// Mouse position
    fixed	angle0;						// Fixed angle of the cue
    int		distance;                   // Cue distance from white ball
    bool	showTracjetory;             // 0 don't show trajectory, 1 show trajectory
};

// ############################################################################
// Global variables
// ############################################################################
int				white;                          			// Color white
int				black;                          			// Color black
int				transparent;                    			// Color transparent

int				nholed;                         			// Number of pocketed balls

bool			mouseClicked;								// False if mouse is not clicked, true if mouse is clicked

extern int		holePositionx[];							// Coordinates of the holes on the x axis
extern int		holePositiony[];							// Coordinates of the holes on the y axis

extern struct	ballStruct ballsArray[BALLSNUMBER];			// Array with balls data
extern struct	cueStruct cue;								// Cue data

extern BITMAP	*buffer;                   					// Buffer image
extern BITMAP	*background;               					// Background image
extern BITMAP	*table;                    					// Table image
extern BITMAP	*ballsbmp[BALLSNUMBER];    					// Balls images
extern BITMAP	*cuebmp;                   					// Cue image
extern BITMAP	*trajectorybuffer;							// Bitmap used to draw trajectory
extern BITMAP	*cuebuffer;									// Bitmap used to draw cue

extern sem_t	ballsSem[BALLSNUMBER];     					// Semaphores for balls
extern sem_t	semmain;                   					// Main semaphore
extern sem_t	semcue;                    					// Semaphore for cue

// ############################################################################
// Functions declaration
// ############################################################################
extern void		init();

extern void		graphics_task(void);
extern void		ball_task(void);
extern void		cue_task(void);
extern bool		isBallsMoving();

extern void		createBall();
extern void		createGraphic();
extern void		createUser();