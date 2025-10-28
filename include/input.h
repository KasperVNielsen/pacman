#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

typedef struct InputState {
    int up;
    int down;
    int left;
    int right;
    int quit;
} InputState;

void input_set_window(GLFWwindow *window);
void input_poll(InputState *input);

#endif
