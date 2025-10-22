#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef struct InputState {
    int up, down, left, right;
    int quit;
} InputState;

void input_init(void);
void input_poll(InputState *in);

#endif // INPUT_H
