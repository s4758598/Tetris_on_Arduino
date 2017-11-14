#include "Arduino.h"
#include "music.h"
#include <avr/interrupt.h>

static const uint16_t MUSIC_PART_A_FREQUENCIES[]= {659, 494, 523, 587, 659, 587, 523, 494, 440, 440, 523, 659, 587, 523, 494, 494, 523, 587, 659, 523, 440, 440, 0, 587, 698, 880, 784, 698, 659, 659, 523, 659, 587, 523, 494, 494, 523, 587, 659, 523, 440, 440, 0};
static const uint16_t MUSIC_PART_A_DURATIONS[]  = {4, 8, 8, 8, 16, 16, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 4, 4, 4, 2, 8, 4, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 4, 4, 4, 4, 4};

static const uint16_t MUSIC_PART_B_FREQUENCIES[] = {330, 262, 294, 247, 262, 220, 208, 247, 330, 262, 294, 247, 262, 330, 440, 415};
static const uint16_t MUSIC_PART_B_DURATIONS[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 1};

static uint8_t music_counter = 0;
static uint8_t music_part = 0;
float music_speedup = 1.0;

void setup_music()
{
    cli();

    // Enable CTC interrupt on timer1 with 1024 prescaler
    TCCR1A = 0;
    TCCR1B = 13; // set CS10 and CS12 bits for 1024 prescaler and WGM12 for CTC mode (see p. 173, 172)
    TIMSK1 = 2;  // set OCIEA and enable "Output Compare A Match Interrupt" for timer 1 (see p. 184)
    OCR1A = MUSIC_CLK_SPEED; // Output Compare 1 A (16bit register see p.180)

    sei();
}

ISR(TIMER1_COMPA_vect)
{
    if (music_part == 0)
    {
        tone(MUSIC_PIN, MUSIC_PART_A_FREQUENCIES[music_counter]);
        OCR1A = (MUSIC_CLK_SPEED / music_speedup) / MUSIC_PART_A_DURATIONS[music_counter];
        music_counter++;

        if (music_counter >= MUSIC_PART_A_LENGTH)
        {
            music_counter = 0;
            music_part = 1;
        }
    }

    else
    {
        tone(MUSIC_PIN, MUSIC_PART_B_FREQUENCIES[music_counter]);
        OCR1A = (MUSIC_CLK_SPEED / music_speedup) / MUSIC_PART_B_DURATIONS[music_counter];
        music_counter++;

        if (music_counter >= MUSIC_PART_B_LENGTH)
        {
            music_counter = 0;
            music_part = 0;
        }
    }
}
