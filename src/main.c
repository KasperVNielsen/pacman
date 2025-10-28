#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "input.h"

#define WIN_W 800
#define WIN_H 600

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glfwCreateWindow(WIN_W, WIN_H, "Pacman", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    input_set_window(window);
    InputState input = {0};

    GameState *game = game_create();
    if (!game) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    double lastTime = glfwGetTime();
    printf("Game running! Use arrow keys, ESC to quit.\n");

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;

        glfwPollEvents();
        input_poll(&input);

        if (input.quit)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        // Print key input state
        printf("Up:%d Down:%d Left:%d Right:%d Quit:%d\r",
               input.up, input.down, input.left, input.right, input.quit);
        fflush(stdout);

        game_update(game, deltaTime, &input);
        game_render(game);
        glfwSwapBuffers(window);
    }

    printf("\nExiting game...\n");
    game_destroy(game);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
