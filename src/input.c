#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "input.h"
#include <stdio.h>

static GLFWwindow *g_window = NULL;

void input_init(void) {
    // window pointer should be set by main after creating the window
}

void input_set_window(GLFWwindow *w) { g_window = w; } // optionally export this

void input_poll(InputState *in) {
    in->up = glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS;
    in->down = glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS;
    in->left = glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS;
    in->right = glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    in->quit = glfwGetKey(g_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}
