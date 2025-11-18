// input.c
#include "input.h"

void input_poll(GLFWwindow *window, InputState *state)
{
    state->up    = glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS;
    state->down  = glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS;
    state->left  = glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS;
    state->right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    state->quit  = glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS;
}
