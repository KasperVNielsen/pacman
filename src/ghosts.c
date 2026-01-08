//each of the ghosts were programmed to have their own distinct traits — the red ghost would directly chase Pac-Man, the pink and blue ghosts would position themselves in front of him, and the orange ghost would be random.

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// Constants
#define NUM_GHOSTS 4
#define GHOST_SPEED_NORMAL 0.8f
#define GHOST_SPEED_FRIGHTENED 0.5f
#define GHOST_SPEED_EATEN 1.5f

typedef enum{
   Blinky,//red
   Pinky,//pink 
   Inky,//cyan
   Clyde,//orange
   NUM_GHOST_TYPES
}GhostType;

typedef enum{
    scatter,
    chase,
    frightened,
    eaten,
    idle
}GhostState;

// Ghost structure to hold individual ghost data
typedef struct{
    GhostType type;
    GhostState state;

    float x , y;
    float targetX, targetY;

    int direction;
    float speed;

    int scatterTimer;
    int chaseTimer;
    int frightenedTimer;

    bool inGhostHouse;
    bool isReleased;

    float colorR,colorG, colorB;
    float scaleX,scaleY;
}Ghost;

// Function prototypes
void initGhosts(Ghost ghosts[]);
void updateGhost(Ghost* ghost, float pacmanX, float pacmanY, int pacmanDir);
void drawGhost(const Ghost* ghost);
void setGhostTarget(Ghost* ghost, float pacmanX, float pacmanY, int pacmanDir);
void updateGhostState(Ghost* ghost, int gameTimer, bool powerPelletActive);
void moveGhost(Ghost* ghost);

// Individual behavior functions (implement these based on ghost type)


int main(){



}