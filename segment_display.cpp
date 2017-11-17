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

#include "segment_display.h"
#include "digital_pin_functions.h"

uint8_t SEGMENT_DISPLAY_LETTER_PINS[] = {SEGMENT_DISPLAY_LETTER_A, SEGMENT_DISPLAY_LETTER_B, SEGMENT_DISPLAY_LETTER_C, SEGMENT_DISPLAY_LETTER_D, SEGMENT_DISPLAY_LETTER_E, SEGMENT_DISPLAY_LETTER_F, SEGMENT_DISPLAY_LETTER_G};
uint8_t SEGMENT_DISPLAY_DIGIT_POSITION_PINS[] = {SEGMENT_DISPLAY_DIGIT_POSITION_PIN_1, SEGMENT_DISPLAY_DIGIT_POSITION_PIN_2, SEGMENT_DISPLAY_DIGIT_POSITION_PIN_3, SEGMENT_DISPLAY_DIGIT_POSITION_PIN_4};

void setup_segment_display()
{
    for(uint8_t i=0; i < 7; i++)
    {
        configure_digital_pin_as_output(SEGMENT_DISPLAY_LETTER_PINS[i]);
    }

    for(uint8_t i=0; i < 4; i++)
    {
        configure_digital_pin_as_output(SEGMENT_DISPLAY_DIGIT_POSITION_PINS[i]);
    }
}

/* 
 *  This function enables or disables digit positions of the segment display. 
 *  
 *  It therefore utilizes a position vector, which consists of the lowest 4 bits
 *  of a byte. Each bit encodes, whether a digit pin of the segment display should
 *  be high (1) or low (0). The segment display is low active, thus a low value 
 *  will enable a specific digit.
 *  
 *  The encoding is done as follows:
 *  | Digit Pin 4 | Digit Pin 3 | Digit Pin 2 | Digit Pin 1 |
 *  |      X3     |     X2      |     X1      |     X0      | x[...] || enables
 *  |-------------+-------------+-------------+-------------+--------||--------
 *  |       0     |      0      |      0      |      0      |    0   ||     all
 *  |       1     |      1      |      1      |      0      |   14   || digit 1
 *  |       1     |      1      |      0      |      1      |   13   || digit 2
 *  |       1     |      0      |      1      |      1      |   11   || digit 3
 *  |       0     |      1      |      1      |      1      |    7   || digit 4
 *  |       1     |      1      |      1      |      1      |   15   ||    none
 *  
 */
static void write_position_vector_to_segment_display(uint8_t position)
{
    for(uint8_t i=0; i<4; i++)
    {
        set_digital_pin_output_value(SEGMENT_DISPLAY_DIGIT_POSITION_PINS[i], position & 1);
        position = position >> 1;
    }
}

static void write_position_to_segment_display(uint8_t position)
{
    switch (position) 
    {
        case 0: 
            write_position_vector_to_segment_display(0);
            break;

        case 1:
            write_position_vector_to_segment_display(14);
            break;
                
        case 2:
            write_position_vector_to_segment_display(13);
            break;
                
        case 3:
            write_position_vector_to_segment_display(11);
            break;
                
        case 4:
            write_position_vector_to_segment_display(7);
            break;
    }
}

/* 
 *  This function is processing a byte (digit vector) in order to set all seven 
 *  segments of a segment display. The digit vector utilize the lowest 7 bits of
 *  a byte. Each bit encodes, whether a segment pin of the display should be 
 *  high (1) or low (0). The segments are high active.
 *  
 *  The encoding is done as follows:
 *  |  G  |  F  |  E  |  D  |  C  |  B  |  A  |
 *  | X6  | X5  | X4  | X3  | X2  | X1  | X0  | x[...] || displays
 *  |-----+-----+-----+-----+-----+-----+-----+--------||---------
 *  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |   63   ||   "0"
 *  |  0  |  0  |  0  |  0  |  1  |  1  |  0  |    6   ||   "1"
 *  |  1  |  0  |  1  |  1  |  0  |  1  |  1  |   91   ||   "2"
 *  |  1  |  0  |  0  |  1  |  1  |  1  |  1  |   79   ||   "3"
 *  |  1  |  1  |  0  |  0  |  1  |  1  |  0  |  102   ||   "4"
 *  |  1  |  1  |  0  |  1  |  1  |  0  |  1  |  109   ||   "5"
 *  |  1  |  1  |  1  |  1  |  1  |  0  |  1  |  125   ||   "6"
 *  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |    7   ||   "7"
 *  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  127   ||   "8"
 *  |  1  |  1  |  0  |  1  |  1  |  1  |  1  |  111   ||   "9"
 *  
 */
static void write_digit_vector_to_segment_display(uint8_t digit)
{
    for(uint8_t i=0; i<7; i++)
    {
        set_digital_pin_output_value(SEGMENT_DISPLAY_LETTER_PINS[i], digit & 1);
        digit = digit >> 1;
    }
}

static void write_digit_to_segment_display(uint8_t digit)
{
    switch(digit)
    {
        case 0:
            write_digit_vector_to_segment_display(63);
            break;

        case 1:
            write_digit_vector_to_segment_display(6);
            break;

        case 2:
            write_digit_vector_to_segment_display(91);
            break;

        case 3:
            write_digit_vector_to_segment_display(79);
            break;

        case 4:
            write_digit_vector_to_segment_display(102);
            break;

        case 5:
            write_digit_vector_to_segment_display(109);
            break;

        case 6:
            write_digit_vector_to_segment_display(125);
            break;

        case 7:
            write_digit_vector_to_segment_display(7);
            break;

        case 8:
            write_digit_vector_to_segment_display(127);
            break;

        case 9:
            write_digit_vector_to_segment_display(111);
            break;
    }
}

void write_digit_to_segment_display_position(uint8_t digit, uint8_t position)
{ 
    write_position_to_segment_display(position);
    write_digit_to_segment_display(digit);
}


/* 
 *  The segment display can only show one distinct digit at a time. In order to
 *  display a number with distinct digits, one has to enable all desired digits
 *  and positions repeatedly -- and that is exactly the purpose of this function:
 *  If called multiple times (e.G. within a loop or an timer ISR), it can display
 *  any number up to four digits.
 */
void write_number_to_segment_display(uint16_t n)
{
    static uint16_t divisor = 1;
    static uint8_t next_digit = 0;
  
    write_digit_to_segment_display_position((n / divisor) % 10, 4 - next_digit);
    
    if (n > 10 && n < 100)
    {
      next_digit++;
      next_digit %= 2;
      divisor *= 10;
      
      if (divisor == 100)
      {
          divisor = 1;
      }
    }

    else if (n > 100 && n < 1000)
    {
      next_digit++;
      next_digit %= 3;
      divisor *= 10;
      
      if (divisor == 1000)
      {
          divisor = 1;
      }
    }

    else if (n > 1000)
    {
      n = n % 10000; 
      
      next_digit++;
      next_digit %= 4;
      divisor *= 10;
      
      if (divisor == 10000)
      {
          divisor = 1;
      }
    }
}
