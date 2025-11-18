// game.h
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct { float x, y; float halfW, halfH; } Rect;
typedef struct { float x, y, r; } Pellet;

typedef struct {
    Rect *walls;
    int wall_count;
    Pellet *pellets;
    int pellet_count;

    // player
    float posX, posY;
    float half;   // half-size of player square
    float speed;  // units per second

    // GL state needed by renderer
    GLuint vao;
    GLuint program;
} Game;

int game_init(Game *g, GLuint program, GLuint vao);
void game_update(Game *g, float dt, int up, int down, int left, int right);
void game_render(Game *g, GLint loc_uOffset, GLint loc_uScale, GLint loc_uColor);
void game_shutdown(Game *g);

// helper
int rects_overlap(float ax, float ay, float aHalfX, float aHalfY,
                  float bx, float by, float bHalfX, float bHalfY);

#endif // GAME_H
