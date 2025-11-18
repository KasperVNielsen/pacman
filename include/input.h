// input.h
#ifndef INPUT_H
#define INPUT_H
#include <GLFW/glfw3.h>

typedef struct {
    int up, down, left, right, quit;
} InputState;

void input_poll(GLFWwindow *window, InputState *state);

#endif // INPUT_H
