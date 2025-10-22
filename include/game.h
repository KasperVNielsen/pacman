#ifndef GAME_H
#define GAME_H

/* forward declare InputState to avoid pulling GLFW/glad into headers */
typedef struct InputState InputState;

/* opaque GameState type */
typedef struct GameState GameState;

GameState* game_create(void);
void game_destroy(GameState* g);
void game_update(GameState* g, float dt, const InputState *in);
void game_render(GameState* g);

#endif // GAME_H
