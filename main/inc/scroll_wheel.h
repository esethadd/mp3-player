#ifndef SCROLL_WHEEL_H
#define SCROLL_WHEEL_H

/* -- Scroll Wheel Pins | PCNT -- */
#define ENCA_PIN 1
#define ENCB_PIN 2

/* -- Button Pins | EXTI -- */
#define SW1_BUTTON_PIN 42
#define SW2_BUTTON_PIN 41
#define SW3_BUTTON_PIN 40
#define SW4_BUTTON_PIN 39
#define SW5_BUTTON_PIN 38

/* -- Scroll Wheel Limits until wraps back to 0 -- */
#define PCNT_HIGH_LIMIT 1500
#define PCNT_LOW_LIMIT -1500

#include "input_manager.h"

/* -- Initialize Peripherals -- */

void scroll_wheel_init();
void buttons_init();
void scrolls_init();


// Testing Encoder Steps
int get_encoder_count();

#endif