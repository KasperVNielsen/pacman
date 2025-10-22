#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Called whenever a key is pressed or released
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;

    if (action == GLFW_PRESS && key >= 32 && key <= 126) {
        printf("You pressed: %c\n", (char)key);
    } else if (action == GLFW_PRESS) {
        printf("You pressed special key: %d\n", key);
    }
    fflush(stdout);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Key Press Printer", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);

    // black background
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
