#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "input.h"


int main(void) {
    if (!glfwInit()) return -1;
    GLFWwindow *window = glfwCreateWindow(800,600,"Pacman",NULL,NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr,"Failed to init GL\n"); return -1;
    }

    // input_set_window(window);

    GameState *game = game_create();
    InputState in = {0};

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)(now - last);
        last = now;

        // poll events and read input
        glfwPollEvents();
        input_poll(&in);

        // update + render
        game_update(game, dt, &in);
        game_render(game);

        glfwSwapBuffers(window);

        if (in.quit) glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    game_destroy(game);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
