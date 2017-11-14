#include "segment_display.h"
#include "digital_pin_functions.h"

uint8_t SEGMENT_LETTER_PINS[] = {SEGMENT_LETTER_A, SEGMENT_LETTER_B, SEGMENT_LETTER_C, SEGMENT_LETTER_D, SEGMENT_LETTER_E, SEGMENT_LETTER_F, SEGMENT_LETTER_G};
uint8_t SEGMENT_DIGIT_PINS[] = {SEGMENT_DIGIT_PIN_1, SEGMENT_DIGIT_PIN_2, SEGMENT_DIGIT_PIN_3, SEGMENT_DIGIT_PIN_4};

void setup_segment_display()
{
    for(uint8_t i=0; i < 7; i++)
    {
        configure_digital_pin_as_output(SEGMENT_LETTER_PINS[i]);
    }

    for(uint8_t i=0; i < 4; i++)
    {
        configure_digital_pin_as_output(SEGMENT_DIGIT_PINS[i]);
    }
}

static void write_position_vector_to_segment_display(uint8_t position)
{
    for(uint8_t i=0; i<4; i++)
    {
        set_digital_pin_output_value(SEGMENT_DIGIT_PINS[i], position & 1);
        position = position >> 1;
    }
}

static void write_digit_vector_to_segment_display(uint8_t digit)
{
    for(uint8_t i=0; i<7; i++)
    {
        set_digital_pin_output_value(SEGMENT_LETTER_PINS[i], digit & 1);
        digit = digit >> 1;
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
