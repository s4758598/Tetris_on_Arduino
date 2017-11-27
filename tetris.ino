/*
 * Copyright (c) 2017 Richard Arnold, Georg Alexander Murzik
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
 

/*
 * This Project is inspired by tetris, a well known game created by Alexey Pajitnov. 
 */

#include <stdint.h>
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>

#include "music.h"
#include "segment_display.h"

#define LED_DISPLAY_PIN 13
#define ROWS 8
#define COLUMNS 5
#define GAME_SPEEDUP_FACTOR 1.25
#define GAME_LEVEL_UP_MODULO 1
#define INVISIBLE_GAME_STATE_LEVEL_MODULO 3 

typedef struct
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
} Color;

typedef struct
{
    bool active = false;
    Color color;
} Pixel;

typedef struct
{
    bool stone[3][3];
} Stone_3x3;

typedef struct
{
    bool stone[2][2];
} Stone_2x2;

typedef union {
    Stone_3x3 s3;
    Stone_2x2 s2;
} Stone_Matrix;

typedef struct
{
    Stone_Matrix matrix;
    uint8_t order;
    int8_t x_offset;
    int8_t y_offset;
    bool visible;
    Color color;
} Current_Stone;

Color color_default;
Pixel pixel_default;

/* Struct representing the tetroid, which can be moved and rotated by the player */
Current_Stone current_stone;

/* Struct containing collided tetorids */
Pixel game_state[ROWS][COLUMNS];

/* Collision test object */
Current_Stone current_stone_test;     

uint8_t level = 1;
uint16_t score = 0;
float game_speedup = 2.0;
bool game_over_reached = false;

/* Determines, whether the game_state will be visible or not */
bool invisible_game_state = false;

Adafruit_NeoPixel led_matrix = Adafruit_NeoPixel(40, LED_DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

/* Arduino method, which is called once at start of the mc to setup the system */
void setup()
{
    setup_music();
    
    led_matrix.begin();
    led_matrix.setBrightness(5);

    setup_segment_display();
    setup_game();
}

/* Initializes tetris logic */
void setup_game()
{
    led_matrix.clear();
    clear_game_state();
    create_new_stone();
    led_matrix.show();
}

void create_next_level()
{
    setup_game();
    render();
}

void level_up()
{
    level++;
    animate_level_up();

    invisible_game_state = (level % INVISIBLE_GAME_STATE_LEVEL_MODULO == 0) ? true : false;    
    music_speedup += MUSIC_SPEEDUP_FACTOR;
    game_speedup *= GAME_SPEEDUP_FACTOR;
    create_next_level();
}

void animate_level_up()
{
    uint8_t led_num = 0;
    for (uint8_t row = 0; row < ROWS; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            led_matrix.setPixelColor(led_num++, 51, 255, 255);
            led_matrix.show();
            write_number_to_segment_display(score);
        }
    }
}

void game_over()
{
    for(;;)
    {
        write_number_to_segment_display(score);
        delay(5);
    }
}

/* 
 * All rotations and move actions (left, right, down) are tested on curent_stone_test.
 * If a modification is valid, it will be commited, otherwise the action will be discarded.
 */
void process_move()
{
  if (collides())
      reset_move();
  else
      commit_move();
}
    
void reset_move()
{
    current_stone_test = current_stone;
}

void commit_move()
{
    current_stone = current_stone_test;
    render();
}

void move_right()
{
    current_stone_test.x_offset++;
    process_move();
}

void move_left()
{
    current_stone_test.x_offset--;
    process_move();
}

/* Standard arduino function called after setup(). This is the main routine of the game. */
void loop()
{ 
    uint8_t pressed_button_old = 0;
    uint8_t pressed_button_new = 0;
    uint32_t time;
  
    for (;;)
    {
        drop_stone_one_pixel();
        for (uint8_t i = 0; i < 7; i++)
        {
            for (uint8_t j = 0; j < 25 / game_speedup; j++)
            {
                time = micros();
                pressed_button_new = check_input_buttons();
                delay(10);
                if(!(pressed_button_new == check_input_buttons())){
                    pressed_button_new =0;
                }
                if (pressed_button_old != pressed_button_new )//&& micros() - time > 100)
                {
                    switch(pressed_button_new)
                    {
                        case 0:
                            // do nothing, button debouncing produces unintended behavior
                            break;
                        case 1:
                            rotate_left();
                            break;

                        case 2:
                            move_left();
                            break;

                        case 3:
                            move_right();
                            break;

                        case 4:
                            rotate_right();
                            break;
                    }
                    pressed_button_old = pressed_button_new;
                }
                
                delay(5);
                write_number_to_segment_display(score);

                if (game_over_reached)
                {
                    game_over();
                }
            }
            render();
        }
    }
}

/* Assigns a random tetroid and other related values to current_stone. */
void create_new_stone()
{
    switch (random(7))
    {
    case 0:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, true, false,
            false, true, false,
            false, true, false,
        };
        break;
    }

    case 1:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, true, false,
            false, true, false,
            true, true, false};
        break;
    }

    case 2:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, true, false,
            false, true, false,
            false, true, true};
        break;
    }

    case 3:
    {
        current_stone.order = 2;
        current_stone.matrix.s2 = {
            true, true,
            true, true};
        break;
    }

    case 4:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, false, false,
            false, true, true,
            true, true, false};
        break;
    }

    case 5:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, false, false,
            true, true, true,
            false, true, false};
        break;
    }

    case 6:
    {
        current_stone.order = 3;
        current_stone.matrix.s3 = {
            false, false, false,
            true, true, false,
            false, true, true};
        break;
    }
    }

    current_stone.color.r = random(256);
    current_stone.color.g = random(256);
    current_stone.color.b = random(256);
    
    current_stone.y_offset = ROWS;
    current_stone.x_offset = 1;
    current_stone.visible = true;
    current_stone_test = current_stone;
}

/* Renders game_state and current_stone */
void render()
{
    led_matrix.clear();
    render_game_state();
    
    if (current_stone.visible)
    {
        render_current_stone();
    }
    
    led_matrix.show();
}

void render_game_state()
{
    uint8_t led_num = 0;
    for (uint8_t col = 0; col < COLUMNS; col++)
    {
        for (uint8_t row = 0; row < ROWS; row++)
        {

            if (game_state[row][col].active == true)
            {
                led_matrix.setPixelColor(
                        led_num,
                        game_state[row][col].color.r,
                        game_state[row][col].color.g,
                        game_state[row][col].color.b
                    );
            }
            
            led_num++;
        }
    }
}

void render_current_stone()
{
    uint8_t led_num;
    for (uint8_t col = 0; col < current_stone.order; col++)
    {
        for (uint8_t row = 0; row < current_stone.order; row++)
        {
            if (col + current_stone.x_offset < COLUMNS && row + current_stone.y_offset < ROWS)
            {
                led_num = ((current_stone.x_offset + col) * 8) + current_stone.y_offset + row;
                switch (current_stone.order)
                {
                case 3:
                    if (current_stone.matrix.s3.stone[row][col] == true)
                    {
                        led_matrix.setPixelColor(led_num,current_stone.color.r, current_stone.color.g, current_stone.color.b);
                    }
                    break;

                case 2:
                    if (current_stone.matrix.s2.stone[row][col] == true)
                    {
                        led_matrix.setPixelColor(led_num,current_stone.color.r, current_stone.color.g, current_stone.color.b);
                    }
                    break;

                }
            }
        }
    }
}

void rotate_right()
{
    if (current_stone.order == 3)
    {
        for (uint8_t row = 0; row < 3; row++)
        {
            for (uint8_t col = 0; col < 3; col++)
            {
                current_stone_test.matrix.s3.stone[row][col] = current_stone.matrix.s3.stone[col][2 - row];
            }
        }
        
        process_move();
    }
}

void rotate_left()
{
    if (current_stone.order == 3)
    {
        for (uint8_t row = 0; row < 3; row++)
        {
            for (uint8_t col = 0; col < 3; col++)
            {
                current_stone_test.matrix.s3.stone[col][2 - row] = current_stone.matrix.s3.stone[row][col];
            }
        }
        
        process_move();
    }
}

/* Returns true if current_stone_test collides with game_state */
bool collides()
{
    for (uint8_t row = 0; row < current_stone_test.order; row++)
    {
        for (uint8_t col = 0; col < current_stone_test.order; col++)
        {
            if (0 <= col + current_stone_test.x_offset &&
                COLUMNS - 1 >= col + current_stone_test.x_offset &&
                0 <= row + current_stone_test.y_offset &&
                ROWS - 1 >= row + current_stone_test.y_offset)
            {
                switch (current_stone_test.order)
                {
                case 3:
                    if (game_state[row +current_stone_test.y_offset][col + current_stone_test.x_offset].active &&
                        current_stone_test.matrix.s3.stone[row][col])
                    {
                        return true;
                    }
                    break;

                case 2:
                    if (game_state[row +current_stone_test.y_offset][col + current_stone_test.x_offset].active &&
                        current_stone_test.matrix.s2.stone[row][col])
                    {
                        return true;
                    }
                    break;
                }
            }
        }
    }
    
    return is_out_of_bounds();
}

bool is_out_of_bounds()
{
    for (uint8_t row = 0; row < current_stone_test.order; row++)
    {
        for (uint8_t col = 0; col < current_stone_test.order; col++)
        {
            if (0 > col + current_stone_test.x_offset ||
                COLUMNS - 1 < col + current_stone_test.x_offset ||
                0 > row + current_stone_test.y_offset)
            {
                switch (current_stone_test.order)
                {
                    case 3:
                        if (current_stone_test.matrix.s3.stone[row][col])
                        {
                            return true;
                        }                       
                        break;
                    case 2:
                        if (current_stone_test.matrix.s2.stone[row][col])
                        {
                            return true;
                        }
                        break;
                }
            }
        }
    }

    return false;
}

void write_current_stone_into_game_state()
{   
    for (uint8_t row = 0; row < current_stone.order; row++)
    {
        for (uint8_t col = 0; col < current_stone.order; col++)
        {
            if (row + current_stone.y_offset < ROWS && col + current_stone.x_offset < COLUMNS)
            {
                switch (current_stone.order)
                {
                case 3:
                    if (current_stone.matrix.s3.stone[row][col])
                    {
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset].active = true;
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset].color = (invisible_game_state) ? color_default : current_stone.color;

                        if(current_stone.y_offset + row == ROWS - 1)
                        {
                            game_over_reached = true;
                        }
                    }
                    break;

                case 2:
                    if (current_stone.matrix.s2.stone[row][col])
                    {
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset].active = true;
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset].color = (invisible_game_state) ? color_default : current_stone.color;

                        if(current_stone.y_offset + row == ROWS - 1)
                        {
                            game_over_reached = true;
                        }
                    }
                    break;
                }                
            }
        }
    }
    
    current_stone.visible = (game_over_reached) ? true : false;
    remove_filled_rows();
    render();
    create_new_stone();
}

void drop_stone_one_pixel()
{
    current_stone_test.y_offset--;
    if (collides())
    {
        write_current_stone_into_game_state();
    }

    else
    {
        commit_move();
    }
}

void clear_game_state()
{
    for (uint8_t col = 0; col < COLUMNS; col++)
    {
        for (uint8_t row = 0; row < ROWS; row++)
        {
            game_state[row][col].active = false;
            game_state[row][col].color = color_default;
        }
    }
}

/* Checks game_state for full rows and removes them */
void remove_filled_rows()
{
    bool level_up_reached = false;
    static uint8_t rows_cleared = 0;
  
    for (uint8_t row = 0; row < ROWS; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            if (game_state[row][col].active == false)
            {
                break;
            }

            if (col == COLUMNS - 1)
            {
                shift_rows_down_starting_from(row);
                row--;
                rows_cleared++;
                score += level;

                if (rows_cleared % GAME_LEVEL_UP_MODULO == 0)
                {
                    level_up_reached = true;
                }
            }
        }
    }

    if (level_up_reached)
    {
        level_up();
    }
}

void shift_rows_down_starting_from(uint8_t row)
{
    for (; row < ROWS - 1; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            if (row == ROWS - 1)
            {
                // clear topmost row
                game_state[row][col] = pixel_default;
            }

            else
            {
                game_state[row][col]= game_state[row + 1][col];                
            }
        }
    }
}

uint8_t check_input_buttons()
{
    int keyVal1 = analogRead(A0);

    if (keyVal1 == 1023)
    {
        return 1;
    }

    else if (keyVal1 >= 990 && keyVal1 <= 1010)
    {
        return 2;
    }

    else if (keyVal1 >= 505 && keyVal1 <= 515)
    {
        return 3;
    }

    else if (keyVal1 >= 5 && keyVal1 <= 10)
    {
        return 4;
    }

    else
    {
        return 0;
    }
}

/* Handy debugging function used for visible feedback */
void debugPixel(){
    led_matrix.setPixelColor(32, 0, 255, 0);
    led_matrix.show();
    delay(500);
}
