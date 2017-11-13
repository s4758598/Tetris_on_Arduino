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

static void configure_digital_pin_as_output(uint8_t pin);

static void set_digital_pin(uint8_t pin, uint8_t value);

static inline void set_digital_pin_high(uint8_t pin);

static inline void set_digital_pin_low(uint8_t pin);

void write_number_to_segment_display(uint16_t n);

static void write_position_vector_to_segment_display(uint8_t position);

static void write_digit_vector_to_segment_display(uint8_t digit);

static void write_position_to_segment_display(uint8_t position);

static void write_digit_to_segment_display(uint8_t digit);

void write_digit_to_segment_display_position(uint8_t digit, uint8_t position);

void display_number_on_segment_display(uint16_t n);
