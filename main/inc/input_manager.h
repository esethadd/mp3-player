#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#define QUEUE_LENGTH 256

#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"

typedef struct InputManager InputManager;

typedef enum {
    FORWARD,
    BACK,
    UP,
    DOWN,
    SCROLL_CW,
    SCROLL_CCW,
    CONFIRM
} Inputs_t;


/* -- Initialize a Input Manager -- */
void input_manager_init();

/* -- Enqueues an Input into the Input Manager -- */
void input_manager_send(Inputs_t input);

/* -- Dequeues blah blah blah -- */
bool input_manager_receieve(Inputs_t *input, TickType_t wait_time);
/* -- Enqueues an Input into the Input Manager from an ISR -- */
void input_manager_send_isr(Inputs_t input);

#endif