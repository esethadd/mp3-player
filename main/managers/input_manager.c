#include "input_manager.h"

struct InputManager{
    QueueHandle_t queue;
};

static InputManager *input_manager = NULL;

/* -- Initialize a Input Manager -- */
void input_manager_init() {
    input_manager = malloc(sizeof(InputManager));

    if (input_manager == NULL) {
        printf("input manager failed.");
        return;
    }

    input_manager->queue = xQueueCreate(QUEUE_LENGTH, sizeof(Inputs_t));

    if (input_manager->queue == NULL) {
        free(input_manager);
        printf("input manager queue failed.");
        return;
    }
}

bool input_manager_receieve(Inputs_t *input, TickType_t wait_time) {
    return xQueueReceive(input_manager->queue, input, wait_time) == pdTRUE;
}

void input_manager_send_isr(Inputs_t input) {
    BaseType_t higher_priority_task_woken = pdFALSE;

    xQueueSendFromISR(
        input_manager->queue,
        &input,
        &higher_priority_task_woken
    );

    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}