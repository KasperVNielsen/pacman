#include "input.h"
#include <string.h>

static GLFWwindow *g_window = NULL;

void input_set_window(GLFWwindow *window) {
    g_window = window;
}

void input_poll(InputState *input) {
    if (!g_window || !input) return;
    memset(input, 0, sizeof(*input));

    if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS)    input->up = 1;
    if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS)  input->down = 1;
    if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS)  input->left = 1;
    if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) input->right = 1;
    if (glfwGetKey(g_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) input->quit = 1;
}
