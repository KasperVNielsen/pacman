#define GLFW_INCLUDE_NONE
#include <glad/glad.h>   
#include <GLFW/glfw3.h>

#include "input.h"

static GLFWwindow *g_window = NULL;

void input_set_window(void *window) {
    g_window = (GLFWwindow*)window;
}

void input_poll(InputState *out) {
    if (!out) return;
    if (!g_window) {
        out->up = out->down = out->left = out->right = out->quit = 0;
        return;
    }
    out->up    = glfwGetKey(g_window, GLFW_KEY_UP)    == GLFW_PRESS;
    out->down  = glfwGetKey(g_window, GLFW_KEY_DOWN)  == GLFW_PRESS;
    out->left  = glfwGetKey(g_window, GLFW_KEY_LEFT)  == GLFW_PRESS;
    out->right = glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    out->quit  = glfwGetKey(g_window, GLFW_KEY_ESCAPE)== GLFW_PRESS;
}
