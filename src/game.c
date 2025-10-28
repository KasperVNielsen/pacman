#include "game.h"
#include <stdlib.h>
#include <stdio.h>

struct GameState {
    float pacman_x;
    float pacman_y;
    int score;
};

GameState* game_create(void) {
    GameState *g = (GameState*)malloc(sizeof(*g));
    if (!g) {
        fprintf(stderr, "game_create: malloc failed\n");
        return NULL;
    }
    g->pacman_x = 100.0f;
    g->pacman_y = 100.0f;
    g->score = 0;
    printf("[game] created at (%.1f, %.1f)\n", g->pacman_x, g->pacman_y);
    fflush(stdout);
    return g;
}

void game_destroy(GameState* g) {
    if (!g) return;
    printf("[game] destroyed\n");
    fflush(stdout);
    free(g);
}

void game_update(GameState* g, float dt, const InputState *in) {
    if (!g || !in) return;
    const float speed = 100.0f;
    if (in->left)  g->pacman_x -= speed * dt;
    if (in->right) g->pacman_x += speed * dt;
    if (in->up)    g->pacman_y -= speed * dt;
    if (in->down)  g->pacman_y += speed * dt;
}

void game_render(GameState* g) {
    (void)g;
}
