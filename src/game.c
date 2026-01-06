// src/game.c
#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/* --- scale factors you requested --- */
/* Rendering scales (what you asked in your snippet):
   walls: uScale.x = halfW * 0.5f; uScale.y = halfH * 1.0f
   pellets: uScale.x = r * 0.6f; uScale.y = r * 1.0f
   player: uScale.x = half * 0.8f; uScale.y = half * 1.0f
*/
#define WALL_SCALE_X 0.5f
#define WALL_SCALE_Y 1.0f
#define PELLET_SCALE_X 0.6f
#define PELLET_SCALE_Y 1.0f
#define PLAYER_SCALE_X 0.8f
#define PLAYER_SCALE_Y 1.0f

/* axis-aligned box overlap using visual half-extents  */
static int rects_overlap_visual(float ax, float ay, float aHalfX, float aHalfY,
                                float bx, float by, float bHalfX, float bHalfY)
{
    return (fabsf(ax - bx) < (aHalfX + bHalfX)) &&
           (fabsf(ay - by) < (aHalfY + bHalfY));
}

/* wrapper: accepts stored half-values and applies scale factors per object before testing */
static int rects_overlap_with_scales(float ax, float ay, float aStoredHalfX, float aStoredHalfY,
                                     float bx, float by, float bStoredHalfX, float bStoredHalfY,
                                     float aScaleX, float aScaleY, float bScaleX, float bScaleY)
{
    /* compute actual visual half-extents used for rendering */
    float aVisHalfX = aStoredHalfX * aScaleX * 0.5f;
    float aVisHalfY = aStoredHalfY * aScaleY * 0.5f;
    float bVisHalfX = bStoredHalfX * bScaleX * 0.5f;
    float bVisHalfY = bStoredHalfY * bScaleY * 0.5f;

    return (fabsf(ax - bx) < (aVisHalfX + bVisHalfX)) &&
           (fabsf(ay - by) < (aVisHalfY + bVisHalfY));
}

/* dynamic pellet array helpers */
static int append_pellet(Game *g, float x, float y, float r) {
    Pellet *tmp = (Pellet*)realloc(g->pellets, (g->pellet_count + 1) * sizeof(Pellet));
    if (!tmp) return 0;
    g->pellets = tmp;
    g->pellets[g->pellet_count].x = x;
    g->pellets[g->pellet_count].y = y;
    g->pellets[g->pellet_count].r = r;
    g->pellet_count++;
    return 1;
}
static void remove_pellet(Game *g, int idx) {
    if (idx < 0 || idx >= g->pellet_count) return;
    if (idx != g->pellet_count - 1) g->pellets[idx] = g->pellets[g->pellet_count - 1];
    g->pellet_count--;
    if (g->pellet_count == 0) {
        free(g->pellets);
        g->pellets = NULL;
    } else {
        Pellet *tmp = (Pellet*)realloc(g->pellets, g->pellet_count * sizeof(Pellet));
        if (tmp) g->pellets = tmp;
    }
}

/* Generate pellets on a centered grid. For each candidate we check
   overlap using the SCALED visual half-extents so pellets never overlap walls.
   pellet_radius parameter is the stored r.
*/
static void generate_pellets(Game *g,
                             float pellet_radius,
                             float spacing,
                             float margin,
                             float avoid_x, float avoid_y, float avoid_radius)
{
    float minXY = -1.0f + margin;
    float maxXY =  1.0f - margin;

    float startX = minXY + spacing * 0.5f;
    float startY = minXY + spacing * 0.5f;

    for (float y = startY; y <= maxXY + 1e-6f; y += spacing) {
        for (float x = startX; x <= maxXY + 1e-6f; x += spacing) {
            // keep clear around player start
            float dx = x - avoid_x, dy = y - avoid_y;
            float avoidDist = avoid_radius + pellet_radius;
            if (dx*dx + dy*dy < (avoidDist * avoidDist)) continue;

            // check overlap with walls (use scaled visuals)
            int blocked = 0;
            for (int w = 0; w < g->wall_count; ++w) {

                if (rects_overlap_with_scales(x, y,
                                              pellet_radius, pellet_radius,
                                              g->walls[w].x, g->walls[w].y, g->walls[w].halfW, g->walls[w].halfH,
                                              PELLET_SCALE_X, PELLET_SCALE_Y, WALL_SCALE_X, WALL_SCALE_Y)) {
                    blocked = 1; break;
                }
            }
            if (blocked) continue;

            // avoid overlapping other pellets
            int collidePel = 0;
            for (int p = 0; p < g->pellet_count; ++p) {
                float px = g->pellets[p].x, py = g->pellets[p].y;
                float sep = (g->pellets[p].r * PELLET_SCALE_X) + (pellet_radius * PELLET_SCALE_X);
                float ddx = px - x, ddy = py - y;
                if (ddx*ddx + ddy*ddy < (sep*sep)) { collidePel = 1; break; }
            }
            if (collidePel) continue;

            if (!append_pellet(g, x, y, pellet_radius)) {
                fprintf(stderr, "generate_pellets: allocation failed\n");
                return;
            }
        }
    }
}



int game_init(Game *g, GLuint program, GLuint vao)
{
    if (!g) return 0;
    g->program = program;
    g->vao = vao;

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

    g->wall_count = (int)(sizeof(static_walls)/sizeof(static_walls[0]));
    g->walls = malloc(sizeof(Rect) * g->wall_count);
    if (!g->walls) return 0;
    for (int i = 0; i < g->wall_count; ++i) g->walls[i] = static_walls[i];

    /* pellets */
    g->pellets = NULL;
    g->pellet_count = 0;

    /* player (keep stored size identical) */
    g->half = 0.05f;
    g->posX = 0.3f; g->posY = 0.3f;
    g->speed = 1.2f;

    /* auto-generate pellets with parameters tuned to fill corridors */
    float pellet_radius = 0.02f;  /* stored pellet.r unchanged */
    float spacing = 0.05f;        /* dense */
    float margin = 0.03f;
    float avoid_radius = 0.14f;

    generate_pellets(g, pellet_radius, spacing, margin, g->posX, g->posY, avoid_radius);

    return 1;
}

void game_update(Game *g, float dt, int up, int down, int left, int right)
{
    float dx = 0.0f, dy = 0.0f;
    if (left)  dx -= g->speed * dt;
    if (right) dx += g->speed * dt;
    if (up)     dy += g->speed * dt;
    if (down)   dy -= g->speed * dt;

    /* attempt X movement: check against walls using visual scaled halves */
    float newX = g->posX + dx;
    int collideX = 0;
    for (int i = 0; i < g->wall_count; ++i) {
        if (rects_overlap_with_scales(newX, g->posY,
                                      g->half, g->half,
                                      g->walls[i].x, g->walls[i].y, g->walls[i].halfW, g->walls[i].halfH,
                                      PLAYER_SCALE_X, PLAYER_SCALE_Y,
                                      WALL_SCALE_X, WALL_SCALE_Y)) {
            collideX = 1; break;
        }
    }
    if (!collideX) g->posX = newX;

    /* attempt Y movement */
    float newY = g->posY + dy;
    int collideY = 0;
    for (int i = 0; i < g->wall_count; ++i) {
        if (rects_overlap_with_scales(g->posX, newY,
                                      g->half, g->half,
                                      g->walls[i].x, g->walls[i].y, g->walls[i].halfW, g->walls[i].halfH,
                                      PLAYER_SCALE_X, PLAYER_SCALE_Y,
                                      WALL_SCALE_X, WALL_SCALE_Y)) {
            collideY = 1; break;
        }
    }
    if (!collideY) g->posY = newY;

    /* clamp to NDC */
    float limit = 1.0f - g->half;
    if (g->posX > limit) g->posX = limit;
    if (g->posX < -limit) g->posX = -limit;
    if (g->posY > limit) g->posY = limit;
    if (g->posY < -limit) g->posY = -limit;

    /* pellet-eating: remove pellet if overlapping (use scaled visuals for both) */
    for (int i = g->pellet_count - 1; i >= 0; --i) {
        if (rects_overlap_with_scales(g->posX, g->posY,
                                      g->half, g->half,
                                      g->pellets[i].x, g->pellets[i].y, g->pellets[i].r, g->pellets[i].r,
                                      PLAYER_SCALE_X, PLAYER_SCALE_Y,
                                      PELLET_SCALE_X, PELLET_SCALE_Y)) {
            remove_pellet(g, i);
        }
    }
}

void game_render(Game *g, GLint loc_uOffset, GLint loc_uScale, GLint loc_uColor)
{
    /* Draw walls (blue) using requested render scales */
    for (int i = 0; i < g->wall_count; ++i) {
        glUniform2f(loc_uOffset, g->walls[i].x, g->walls[i].y);
        glUniform2f(loc_uScale, g->walls[i].halfW * WALL_SCALE_X, g->walls[i].halfH * WALL_SCALE_Y);
        glUniform3f(loc_uColor, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(g->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    /* Draw pellets (yellow) using requested pellet scales */
    for (int i = 0; i < g->pellet_count; ++i) {
        glUniform2f(loc_uOffset, g->pellets[i].x, g->pellets[i].y);
        glUniform2f(loc_uScale, g->pellets[i].r * PELLET_SCALE_X, g->pellets[i].r * PELLET_SCALE_Y);
        glUniform3f(loc_uColor, 1.0f, 1.0f, 0.0f);
        glBindVertexArray(g->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    /* Draw player (white) using requested player scales */
    glUniform2f(loc_uOffset, g->posX, g->posY);
    glUniform2f(loc_uScale, g->half * PLAYER_SCALE_X, g->half * PLAYER_SCALE_Y);
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
