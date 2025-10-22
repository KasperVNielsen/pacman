#ifndef INPUT_H
#define INPUT_H

typedef struct InputState {
    int up;
    int down;
    int left;
    int right;
    int quit;
} InputState;

/* set the GLFW window (implementation file will include GLFW) */
void input_set_window(void *window);
void input_poll(InputState *out);

#endif // INPUT_H
