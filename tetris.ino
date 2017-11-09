#include <Adafruit_NeoPixel.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#define MUSIC_PIN 8
#define MUSIC_CLK_SPEED 23438
#define MUSIC_PART_A_LENGTH 43
#define MUSIC_PART_B_LENGTH 16

#define DISPLAY_PIN 13
#define FF_REST_PIN 4

#define ROWS 8
#define COLUMNS 5

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
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Current_Stone;

Adafruit_NeoPixel led_matrix = Adafruit_NeoPixel(40, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

bool game_state[ROWS][COLUMNS];

Current_Stone current_stone; // tetorid, that moves
Current_Stone current_stone_test;
bool stoneIsSetted = false;

Stone_Matrix replacement; // memory region for rotation

uint8_t selected_stone = 0;

const int MUSIC_PART_A_FREQUENCIES[]= {659, 494, 523, 587, 659, 587, 523, 494, 440, 440, 523, 659, 587, 523, 494, 494, 523, 587, 659, 523, 440, 440, 0, 587, 698, 880, 784, 698, 659, 659, 523, 659, 587, 523, 494, 494, 523, 587, 659, 523, 440, 440, 0};
const int MUSIC_PART_A_DURATIONS[]  = {4, 8, 8, 8, 16, 16, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 4, 4, 4, 2, 8, 4, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 4, 4, 4, 4, 4};

const int MUSIC_PART_B_FREQUENCIES[] = {330, 262, 294, 247, 262, 220, 208, 247, 330, 262, 294, 247, 262, 330, 440, 415};
const int MUSIC_PART_B_DURATIONS[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 1};

volatile uint8_t music_counter = 0;
volatile uint8_t music_part = 0;

void setup()
{
    Serial.begin(9600);
    cli();

    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);

    OCR1A = 23438; // start playing right after the interrupts are defined
    TCCR1B |= (1 << WGM12); // turn on CTC mode:

    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);

    // Setup external interrupt INT0 on rising edge
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);
    sei(); // enable global interrupts

    createGameState();
    led_matrix.begin();
    led_matrix.setBrightness(5);
    led_matrix.show();

    select_random_stone();
    led_matrix.show();

    pinMode(2, INPUT);
    pinMode(FF_REST_PIN, OUTPUT);
    pinMode(DISPLAY_PIN, OUTPUT);

    digitalWrite(FF_REST_PIN, LOW);
    

    // periodisch soll der game state gezeichnet werden -> loop?
    // auf inputs soll schnell reagiert werden -> external interrupt
    // -> modizifiert current_stone (bewegung, drehung)
    // -> neu zeichnen?
}

ISR(TIMER1_COMPA_vect)
{
    if (music_part == 0)
    {
        tone(MUSIC_PIN, MUSIC_PART_A_FREQUENCIES[music_counter]);
        OCR1A = MUSIC_CLK_SPEED / MUSIC_PART_A_DURATIONS[music_counter];
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
        OCR1A = MUSIC_CLK_SPEED / MUSIC_PART_B_DURATIONS[music_counter];
        music_counter++;

        if (music_counter >= MUSIC_PART_B_LENGTH)
        {
            music_counter = 0;
            music_part = 0;
        }
    }
}

void move_right()
{
    current_stone_test.x_offset++;

    if (collides())
        reset_move();
    else
        commit_move();
}

void move_left()
{
    current_stone_test.x_offset--;

    if (collides())
        reset_move();
    else
        commit_move();
}

void printGameState(void)
{
    // spaltenkopf
    Serial.print("  ");
    for (uint8_t i = 0; i < COLUMNS; i++)
    {
        Serial.print("|" + i);
    }
    Serial.println("|");

    // game sate
    for (int i = ROWS - 1; i >= 0; i--)
    {
        Serial.print(i);
        Serial.print("|");
        for (int j = 0; j < COLUMNS; j++)
        {
            Serial.print(game_state[i][j] ? "X" : "O");
        }
        Serial.println();
    }
}

void restart()
{
    setup();
    led_matrix.clear();
    render();
}

// the loop routine runs over and over again forever:
void loop()
{
    for (;;)
    {
        dropStoneOnePixel();
        for (uint8_t i = 0; i < 7; i++)
        {
            for (uint8_t j = 0; j < 10; j++)
            {
                check_keys();
                delay(10);
            }
            render();
        }
    }

}

void select_random_stone()
{
    // select random stone
    uint8_t selection = random(7);
    //uint8_t selection = selected_stone; // XXX

    // select random color
    current_stone.r = random(256);
    current_stone.g = random(256);
    current_stone.b = random(256);
    switch (selection)
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

    default:
    {
        //TODO
        break;
    }
    }
    current_stone.y_offset = ROWS;
    current_stone.x_offset = 1;
    current_stone_test = current_stone;
    stoneIsSetted = true;

    //dropStoneOnePixel();
}
void render()
{
    led_matrix.clear();
    render_game_state();
    if (stoneIsSetted)
    {
        render_stone_matrix();
    }
    led_matrix.show();
    printGameState();
}
void render_game_state()
{
    uint8_t led_num = 0;
    for (uint8_t col = 0; col < COLUMNS; col++)
    {
        for (uint8_t row = 0; row < ROWS; row++)
        {

            if (game_state[row][col] == true)
            {
                led_matrix.setPixelColor(led_num, 255, 0, 0); //TODO
            }
            led_num++;
        }
    }
}
void render_stone_matrix()
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
                        led_matrix.setPixelColor(led_num,current_stone.r, current_stone.g, current_stone.b);
                    }
                    break;
                case 2:
                    if (current_stone.matrix.s2.stone[row][col] == true)
                    {
                        led_matrix.setPixelColor(led_num,current_stone.r, current_stone.g, current_stone.b);
                    }
                    break;
                }
            }
        }
    }
}

void rotate_right()
{
    if (current_stone_test.order == 3)
    {
        for (uint8_t row = 0; row < current_stone_test.order; row++)
        {
            for (uint8_t col = 0; col < current_stone_test.order; col++)
            {
                replacement.s3.stone[row][col] =
                current_stone_test.matrix.s3.stone[col][current_stone_test.order - 1 - row];
            }
        }
        current_stone_test.matrix = replacement;
    }
    if (collides())
        reset_move();
    else
        commit_move();
}

void rotate_left()
{
    if (current_stone_test.order == 3)
    {
        for (uint8_t row = 0; row < current_stone_test.order; row++)
        {
            for (uint8_t col = 0; col < current_stone_test.order; col++)
            {
                replacement.s3.stone[col][current_stone_test.order - 1 -row] =
                    current_stone_test.matrix.s3.stone[row][col];
            }
        }

        current_stone_test.matrix = replacement;
    }
    if (collides())
        reset_move();
    else
        commit_move();
}

//returns true if stone would collide
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
                    if (game_state[row +current_stone_test.y_offset][col + current_stone_test.x_offset] &&
                        current_stone_test.matrix.s3.stone[row][col])
                    {
                        //printdebugpixel();
                        return true;
                    }
                    break;
                case 2:
                    if (game_state[row +current_stone_test.y_offset][col + current_stone_test.x_offset] &&
                        current_stone_test.matrix.s2.stone[row][col])
                    {
                        //printdebugpixel();
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return isOutOFBounds();
}
bool isOutOFBounds()
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
                        //printdebugpixel();
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
void writeIntoGState()
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
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset] = true;
                    //current_stone.matrix.s3.stone[i][j];
                    break;
                case 2:
                    if (current_stone.matrix.s2.stone[row][col])
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset] = true;
                    //current_stone.matrix.s2.stone[i][j];
                    break;
                }
            }
        }
    }
    stoneIsSetted = false;
    removeFilledRows();
    render();
    select_random_stone();
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
void dropStoneOnePixel()
{
    current_stone_test.y_offset--;
    if (collides())
    {
        writeIntoGState();
    }
    else
    {
        //printdebugpixel();
        commit_move();
    }
}
void printdebugpixel()
{
    led_matrix.setPixelColor(32, 0, 255, 0); //TODO
    led_matrix.show();
    delay(500);
}
void createGameState()
{
    for (uint8_t col = 0; col < COLUMNS; col++)
    {
        for (uint8_t row = 0; row < ROWS; row++)
        {
            game_state[row][col] = false;
        }
    }
}
void removeFilledRows()
{
    for (uint8_t row = 0; row < ROWS; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            if (game_state[row][col] == false)
            {
                break;
            }

            if (col == COLUMNS - 1)
            {
                // hier kanns gelöscht werden
                removeFilledRow(row);
                row--;
            }
        }
    }
}
void removeFilledRow(uint8_t row)
{
    for (; row < ROWS - 1; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            if (row == ROWS - 1)
            {
                // letzte Zeile
                game_state[row][col] = false;
            }

            else
            {
                game_state[row][col] = game_state[row + 1][col];
            }
        }
    }
}

ISR(INT0_vect)
{
    int keyVal1 = analogRead(A0); //TODO durch hardware-nahen Code ersetzen

    if (keyVal1 == 1023)
    {
        rotate_left();
    }

    else if (keyVal1 >= 990 && keyVal1 <= 1010)
    {
        move_left();
    }

    else if (keyVal1 >= 505 && keyVal1 <= 515)
    {
        move_right();
    }

    else if (keyVal1 >= 5 && keyVal1 <= 10)
    {
      
    }

    // reset FF
    digitalWrite(FF_REST_PIN, HIGH);
    digitalWrite(FF_REST_PIN, LOW);
}

void check_keys()
{
    int keyVal1 = analogRead(A0); //TODO durch hardware-nahen Code ersetzen

    if (keyVal1 == 1023)
    {
        rotate_left();
    }

    else if (keyVal1 >= 990 && keyVal1 <= 1010)
    {
        move_left();
    }

    else if (keyVal1 >= 505 && keyVal1 <= 515)
    {
        move_right();
    }

    else if (keyVal1 >= 5 && keyVal1 <= 10)
    {
        rotate_right();
    }

    else
    {
        ;
    }

    // reset FF
    digitalWrite(FF_REST_PIN, HIGH);
    digitalWrite(FF_REST_PIN, LOW);
}