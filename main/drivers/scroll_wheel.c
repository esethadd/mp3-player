#include "scroll_wheel.h"
#include <stdio.h>
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


static pcnt_unit_handle_t scroll_unit = NULL;

static pcnt_channel_handle_t scroll_channel_a = NULL;
static pcnt_channel_handle_t scroll_channel_b = NULL;

static void button_isr_handler(void *arg);
static void scroll_wheel_task(void *arg);
static bool scroll_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, 
    void *user_ctx);


void scroll_wheel_init() {
    /* -- Buttons Configuration -- */

    buttons_init();

    /* -- Scroll Wheel Configuration -- */

    scrolls_init();
}


void scrolls_init() {
    /* -- Encoder COM is held high -- */
    gpio_config_t encoder_gpio_config = {
    .pin_bit_mask = (1ULL << ENCA_PIN) |
                    (1ULL << ENCB_PIN),

    .mode = GPIO_MODE_INPUT,

    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_ENABLE,

    .intr_type = GPIO_INTR_DISABLE
};

ESP_ERROR_CHECK(gpio_config(&encoder_gpio_config));

    pcnt_unit_config_t unit_config = {
        .low_limit =  PCNT_LOW_LIMIT,            
        .high_limit = PCNT_HIGH_LIMIT,            
    };

    
    ESP_ERROR_CHECK( 
        pcnt_new_unit(&unit_config, &scroll_unit)
    );

    /* -- Channel A Configuration -> Clockwise Turns -- */
    pcnt_chan_config_t channel_a_config = {
        .edge_gpio_num = ENCA_PIN,
        .level_gpio_num = ENCB_PIN,
    };

    ESP_ERROR_CHECK(
        pcnt_new_channel(
            scroll_unit,
            &channel_a_config,
            &scroll_channel_a
        )
    );

    // Clockwise Increases Count
    ESP_ERROR_CHECK(
        pcnt_channel_set_edge_action(
            scroll_channel_a,
            PCNT_CHANNEL_EDGE_ACTION_DECREASE,
            PCNT_CHANNEL_EDGE_ACTION_INCREASE
        )
    );

    ESP_ERROR_CHECK(
        pcnt_channel_set_level_action(
            scroll_channel_a,
            PCNT_CHANNEL_LEVEL_ACTION_KEEP,
            PCNT_CHANNEL_LEVEL_ACTION_INVERSE
        )
    );

    /* -- Channel B Configuration -> Counter-Clockwise Turns -- */
    pcnt_chan_config_t channel_b_config = {
    .edge_gpio_num  = ENCB_PIN,
    .level_gpio_num = ENCA_PIN,
    };


    ESP_ERROR_CHECK(
        pcnt_new_channel(
            scroll_unit,
            &channel_b_config,
            &scroll_channel_b
        )
    );

    // Counter Clockwise Decreases Count
    ESP_ERROR_CHECK(
        pcnt_channel_set_edge_action(
            scroll_channel_b,
            PCNT_CHANNEL_EDGE_ACTION_INCREASE,
            PCNT_CHANNEL_EDGE_ACTION_DECREASE
        )
    );

    ESP_ERROR_CHECK(
        pcnt_channel_set_level_action(
            scroll_channel_b,
            PCNT_CHANNEL_LEVEL_ACTION_KEEP,
            PCNT_CHANNEL_LEVEL_ACTION_INVERSE
        )
    );

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };

    ESP_ERROR_CHECK(
        pcnt_unit_set_glitch_filter(
            scroll_unit,
            &filter_config
        )
    );

    // One Click Forward/Back is +-2 | Send Input to input manager at +-4.
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(scroll_unit, 4));
    ESP_ERROR_CHECK (pcnt_unit_add_watch_point(scroll_unit, -4));

    pcnt_event_callbacks_t callbacks = {
        .on_reach = scroll_callback
    };

    ESP_ERROR_CHECK (pcnt_unit_register_event_callbacks(scroll_unit, &callbacks, NULL));

    ESP_ERROR_CHECK(pcnt_unit_enable(scroll_unit));

    ESP_ERROR_CHECK(pcnt_unit_clear_count(scroll_unit));

    ESP_ERROR_CHECK(pcnt_unit_start(scroll_unit));

}


void buttons_init() {

    /* -- Configure 5 Buttons GPIOs -- */
    gpio_config_t button_conf = {
        .intr_type =    GPIO_INTR_NEGEDGE,          // Falling Edge
        .mode =         GPIO_MODE_INPUT,                   
        .pin_bit_mask = (1ULL << SW1_BUTTON_PIN) |  // Center Button
                        (1ULL << SW2_BUTTON_PIN) |  // Down Button
                        (1ULL << SW3_BUTTON_PIN) |  // Right Button
                        (1ULL << SW4_BUTTON_PIN) |  // Up Button
                        (1ULL << SW5_BUTTON_PIN),   // Left Button
        .pull_down_en = GPIO_PULLDOWN_ENABLE,     
        .pull_up_en =   GPIO_PULLUP_DISABLE,          
    };

    ESP_ERROR_CHECK(
        gpio_config(&button_conf)
    );


    ESP_ERROR_CHECK(
        gpio_install_isr_service(0)
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            SW1_BUTTON_PIN,
            button_isr_handler,
            (void *)CONFIRM
        )
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            SW2_BUTTON_PIN,
            button_isr_handler,
            (void *)DOWN
        )
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            SW3_BUTTON_PIN,
            button_isr_handler,
            (void *)FORWARD
        )
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            SW4_BUTTON_PIN,
            button_isr_handler,
            (void *)UP
        )
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            SW5_BUTTON_PIN,
            button_isr_handler,
            (void *)BACK
        )
    );

    
    xTaskCreate (
        scroll_wheel_task,
        "scroll_wheel_task",
        2048,
        NULL,       
        5,
        NULL
    );
}



/* -- Testing Input Manager -- */
static void scroll_wheel_task(void *arg)
{
    Inputs_t button_input;

    while (1)
    {
        if (input_manager_receieve(&button_input, portMAX_DELAY)) {
            switch (button_input) {
                case UP:
                    printf("UP\n");
                    break;

                case DOWN:
                    printf("DOWN\n");
                    break;

                case BACK:
                    printf("BACK\n");
                    break;

                case FORWARD:
                    printf("FORWARD\n");
                    break;

                case CONFIRM:
                    printf("CONFIRM\n");
                    break;
                
                case SCROLL_CW:
                    printf("SCROLL CLOCKWISE\n");
                    break;
                
                case SCROLL_CCW:
                    printf("SCROLL COUNTER CLOCKWISE\n");
                    break;

                default:
                    break;
            } // switch
        } // if
    } // while
}


static void IRAM_ATTR button_isr_handler(void *arg) {
    Inputs_t input = (Inputs_t)(uintptr_t)arg;
    input_manager_send_isr(input);
}

static bool scroll_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata,
     void *user_ctx) {
    if (edata->watch_point_value == 4) {
        input_manager_send_isr(SCROLL_CW);
    }
    else if (edata->watch_point_value == -4) {
        input_manager_send_isr(SCROLL_CCW);
    }

    pcnt_unit_clear_count(unit);

    return false;
}

// Testing encoder steps
int get_encoder_count() {
    int count = 0;
    pcnt_unit_get_count(scroll_unit, &count);

    return count;
}

