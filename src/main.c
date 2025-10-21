

#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed: glfwInit()\n");
        return EXIT_FAILURE;
    }

    // Request an OpenGL 3.3 core context (change if needed)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "pman (glad+glfw)", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed: glfwCreateWindow()\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLAD after making the context current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed: gladLoadGLLoader()\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    printf("GL Version: %s\n", glGetString(GL_VERSION));
    fflush(stdout);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
