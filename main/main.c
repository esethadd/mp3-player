#include <stdio.h>
#include "scroll_wheel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void){

    // gui init
    input_manager_init();
    //sd card init
    scroll_wheel_init();
    //usbc init

    // delay for startup
    while (1) {
        // do something
    }
}


