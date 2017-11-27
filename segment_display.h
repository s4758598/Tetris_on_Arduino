/*
 * Copyright (c) 2017 Georg Alexander Murzik
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include <stdint.h>

/* Segment display pin to arduino digital pin mapping */
#define SEGMENT_DISPLAY_PIN_01 0
#define SEGMENT_DISPLAY_PIN_02 1
#define SEGMENT_DISPLAY_PIN_04 2
#define SEGMENT_DISPLAY_PIN_05 3
#define SEGMENT_DISPLAY_PIN_06 4
#define SEGMENT_DISPLAY_PIN_07 5
#define SEGMENT_DISPLAY_PIN_08 6
#define SEGMENT_DISPLAY_PIN_09 7
#define SEGMENT_DISPLAY_PIN_10 8
#define SEGMENT_DISPLAY_PIN_11 9
#define SEGMENT_DISPLAY_PIN_12 10

/* Segment display letter to segment display pin mapping */
#define SEGMENT_DISPLAY_LETTER_PIN_A SEGMENT_DISPLAY_PIN_11
#define SEGMENT_DISPLAY_LETTER_PIN_B SEGMENT_DISPLAY_PIN_07
#define SEGMENT_DISPLAY_LETTER_PIN_C SEGMENT_DISPLAY_PIN_04
#define SEGMENT_DISPLAY_LETTER_PIN_D SEGMENT_DISPLAY_PIN_02
#define SEGMENT_DISPLAY_LETTER_PIN_E SEGMENT_DISPLAY_PIN_01
#define SEGMENT_DISPLAY_LETTER_PIN_F SEGMENT_DISPLAY_PIN_10
#define SEGMENT_DISPLAY_LETTER_PIN_G SEGMENT_DISPLAY_PIN_05

/* Segment display digit to segment display pin mapping */
#define SEGMENT_DISPLAY_DIGIT_POSITION_PIN_1 SEGMENT_DISPLAY_PIN_12
#define SEGMENT_DISPLAY_DIGIT_POSITION_PIN_2 SEGMENT_DISPLAY_PIN_09
#define SEGMENT_DISPLAY_DIGIT_POSITION_PIN_3 SEGMENT_DISPLAY_PIN_08
#define SEGMENT_DISPLAY_DIGIT_POSITION_PIN_4 SEGMENT_DISPLAY_PIN_06

/* Configures all assigned segment display pins as output */
void setup_segment_display();

void write_digit_to_segment_display_position(uint8_t digit, uint8_t position);

void write_number_to_segment_display(uint16_t n);
