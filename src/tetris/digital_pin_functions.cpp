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

#include "digital_pin_functions.h"
#include <avr/io.h>

void configure_digital_pin_as_output(uint8_t pin)
{    
    if (pin < 8)
    {
        DDRD |= (1 << pin);
    }
    
    else if (pin > 7 && pin < 14)
    {
        DDRB |= (1 << (pin - 8));
    }
}

static inline void set_digital_pin_output_high(uint8_t pin)
{
    if (pin < 8)
    {
        PORTD |= (1 << pin);
    }
    
    else if (pin > 7 && pin < 14)
    {
        PORTB |= (1 << (pin % 8));
    }
}

static inline void set_digital_pin_output_low(uint8_t pin)
{
    if (pin < 8)
    {
        PORTD &= ~(1 << pin);
    }
    
    else if (pin > 7 && pin < 14)
    {
        PORTB &= ~(1 << (pin % 8));
    }
}

void set_digital_pin_output_value(uint8_t pin, uint8_t value)
{
    switch(value)
    {
        case 0:
            set_digital_pin_output_low(pin);
            break;

        case 1:
            set_digital_pin_output_high(pin);
            break;
    }
}
