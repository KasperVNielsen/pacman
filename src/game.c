// game.c
#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/* collision: use visual half-extents directly */
int rects_overlap(float ax, float ay, float aHalfX, float aHalfY,
                  float bx, float by, float bHalfX, float bHalfY)
{
   
    float aVisHalfX = aHalfX * 0.5f;
    float aVisHalfY = aHalfY * 0.5f;
    float bVisHalfX = bHalfX * 0.5f;
    float bVisHalfY = bHalfY * 0.5f;

    return (fabsf(ax - bx) < (aVisHalfX + bVisHalfX)) &&
           (fabsf(ay - by) < (aVisHalfY + bVisHalfY));
}

int game_init(Game *g, GLuint program, GLuint vao)
{
    if (!g) return 0;
    g->program = program;
    g->vao = vao;

    // allocate & fill arrays using your original wall definitions
    static Rect static_walls[] = {
        { -0.95f,  0.0f, 0.05f, 0.95f },  // left outer
        {  0.95f,  0.0f, 0.05f, 0.95f },  // right outer
        {  0.00f,  0.95f, 0.90f, 0.05f },  // top outer
        {  0.00f, -0.95f, 0.90f, 0.05f },  // bottom outer
        { -0.50f,  0.72f, 0.30f, 0.04f },
        {  0.50f,  0.72f, 0.30f, 0.04f },
        { -0.50f, -0.72f, 0.30f, 0.04f },
        {  0.50f, -0.72f, 0.30f, 0.04f },
        { -0.65f,  0.20f, 0.04f, 0.40f },
        { -0.65f, -0.20f, 0.04f, 0.40f },
        {  0.65f,  0.20f, 0.04f, 0.40f },
        {  0.65f, -0.20f, 0.04f, 0.40f },
        {  0.00f,  0.00f, 0.16f, 0.12f },
        { -0.28f,  0.12f, 0.10f, 0.03f },
        { -0.28f, -0.12f, 0.10f, 0.03f },
        {  0.28f,  0.12f, 0.10f, 0.03f },
        {  0.28f, -0.12f, 0.10f, 0.03f },
        { -0.80f,  0.80f, 0.12f, 0.12f },
        {  0.80f,  0.80f, 0.12f, 0.12f },
        { -0.80f, -0.80f, 0.12f, 0.12f },
        {  0.80f, -0.80f, 0.12f, 0.12f }
    };
    g->wall_count = sizeof(static_walls)/sizeof(static_walls[0]);
    g->walls = malloc(sizeof(Rect) * g->wall_count);
    for (int i=0;i<g->wall_count;++i) g->walls[i] = static_walls[i];

    static Pellet static_pellets[] = {
        { -0.7f,  0.7f, 0.02f },
        { -0.5f,  0.7f, 0.02f },
        { -0.3f,  0.7f, 0.02f }
    };
    g->pellet_count = sizeof(static_pellets)/sizeof(static_pellets[0]);
    g->pellets = malloc(sizeof(Pellet) * g->pellet_count);
    for (int i=0;i<g->pellet_count;++i) g->pellets[i] = static_pellets[i];

    // player setup 
    g->half = 0.05f;
    g->posX = 0.3f; g->posY = 0.3f;
    g->speed = 1.2f;
    return 1;
}

void game_update(Game *g, float dt, int up, int down, int left, int right)
{
    float dx = 0.0f, dy = 0.0f;
    if (left)  dx -= g->speed * dt;
    if (right) dx += g->speed * dt;
    if (up)     dy += g->speed * dt;
    if (down)   dy -= g->speed * dt;

    // attempt X
    float newX = g->posX + dx;
    int collideX = 0;
    for (int i=0;i<g->wall_count;++i) {
        if (rects_overlap(newX, g->posY, g->half, g->half,
                          g->walls[i].x, g->walls[i].y, g->walls[i].halfW, g->walls[i].halfH)) {
            collideX = 1; break;
        }
    }
    if (!collideX) g->posX = newX;

    // attempt Y
    float newY = g->posY + dy;
    int collideY = 0;
    for (int i=0;i<g->wall_count;++i) {
        if (rects_overlap(g->posX, newY, g->half, g->half,
                          g->walls[i].x, g->walls[i].y, g->walls[i].halfW, g->walls[i].halfH)) {
            collideY = 1; break;
        }
    }
    if (!collideY) g->posY = newY;

    // clamp
    float limit = 1.0f - g->half;
    if (g->posX > limit) g->posX = limit;
    if (g->posX < -limit) g->posX = -limit;
    if (g->posY > limit) g->posY = limit;
    if (g->posY < -limit) g->posY = -limit;
}

void game_render(Game *g, GLint loc_uOffset, GLint loc_uScale, GLint loc_uColor)
{
    // draw walls (blue)
    for (int i=0;i<g->wall_count;++i) {
        glUniform2f(loc_uOffset, g->walls[i].x, g->walls[i].y);
        // IMPORTANT: unit quad ±0.5 -> to get half-extent H, scale by H*2
        glUniform2f(loc_uScale, g->walls[i].halfW * 0.5f, g->walls[i].halfH * 1.0f);
        glUniform3f(loc_uColor, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(g->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    // pellets
    for (int i=0;i<g->pellet_count;++i){
        glUniform2f(loc_uOffset, g->pellets[i].x, g->pellets[i].y);
        glUniform2f(loc_uScale, g->pellets[i].r * 0.6f, g->pellets[i].r * 1.0f);
        glUniform3f(loc_uColor, 1.0f, 1.0f, 0.0f);
        glBindVertexArray(g->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    // player (white)
    glUniform2f(loc_uOffset, g->posX, g->posY);
    glUniform2f(loc_uScale, g->half * 0.8f, g->half * 1.0f);
    glUniform3f(loc_uColor, 1.0f, 1.0f, 1.0f);
    glBindVertexArray(g->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void game_shutdown(Game *g)
{
    if (!g) return;
    free(g->walls); g->walls = NULL;
    free(g->pellets); g->pellets = NULL;
}
