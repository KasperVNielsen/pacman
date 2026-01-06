//each of the ghosts were programmed to have their own distinct traits — the red ghost would directly chase Pac-Man, the pink and blue ghosts would position themselves in front of him, and the orange ghost would be random.

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define GHOST_SCALE_X 0.8f
#define GHOST_SCALE_Y 1.0f

typedef enum{
   Blinky,//red
   Pinky,//pink 
   Inky,//cyan
   Clyde//orange
}GhostType;

typedef enum{
    scatter,
    chase,
    frightened,
    eaten,
    idle
}GhostStates;

void drawGhosts(){}

int main(){



}