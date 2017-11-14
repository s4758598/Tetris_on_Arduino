#pragma once
#include <stdint.h>

#define SEGMENT_PIN_01 0
#define SEGMENT_PIN_02 1
#define SEGMENT_PIN_04 2
#define SEGMENT_PIN_05 3
#define SEGMENT_PIN_06 4
#define SEGMENT_PIN_07 5
#define SEGMENT_PIN_08 6
#define SEGMENT_PIN_09 7
#define SEGMENT_PIN_10 8
#define SEGMENT_PIN_11 9
#define SEGMENT_PIN_12 10

#define SEGMENT_LETTER_A SEGMENT_PIN_11
#define SEGMENT_LETTER_B SEGMENT_PIN_07
#define SEGMENT_LETTER_C SEGMENT_PIN_04
#define SEGMENT_LETTER_D SEGMENT_PIN_02
#define SEGMENT_LETTER_E SEGMENT_PIN_01
#define SEGMENT_LETTER_F SEGMENT_PIN_10
#define SEGMENT_LETTER_G SEGMENT_PIN_05

#define SEGMENT_DIGIT_PIN_1 SEGMENT_PIN_12
#define SEGMENT_DIGIT_PIN_2 SEGMENT_PIN_09
#define SEGMENT_DIGIT_PIN_3 SEGMENT_PIN_08
#define SEGMENT_DIGIT_PIN_4 SEGMENT_PIN_06

void setup_segment_display();

void configure_digital_pin_as_output(uint8_t pin);

void set_digital_pin_output_value(uint8_t pin, uint8_t value);

void write_digit_to_segment_display_position(uint8_t digit, uint8_t position);

void write_number_to_segment_display(uint16_t n);
