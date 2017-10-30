//Rules:TODO add rules
//1.: Inorder to gain the benefits of our learngroup use insults to motivate your teammember.
//
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/interrupt.h>
#endif
#define PIN 13

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

Adafruit_NeoPixel led_matrix = Adafruit_NeoPixel(40, PIN, NEO_GRB + NEO_KHZ800);

bool game_state[ROWS][COLUMNS];

Current_Stone current_stone; // tetorid, that moves
Current_Stone current_stone_test;
bool stoneIsSetted = false;

Stone_Matrix replacement; // memory region for rotation

uint8_t selected_stone = 0;

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

void setup()
{
    Serial.begin(9600);
    createGameState();
    led_matrix.begin();
    led_matrix.setBrightness(5);
    led_matrix.show();

    select_random_stone();
    led_matrix.show();

    pinMode(2, INPUT);
    pinMode(13, OUTPUT);
    digitalWrite(2, HIGH);

    sei();
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01); // trigger int0 on falling edge
}

ISR(INT0_vect)
{
    rotate_left();
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

    //Richi:
    for (;;)
    {
        rotate_right();
        delay(700);
        dropStoneOnePixel();
        //rotate_right();
        delay(700);
    }

    //checkout the code before trying to use this
    // for (int i =0 ;i<3;i++)
    // {
    //     delay(700);

    //     writeIntoGState();

    // }
    // led_matrix.clear();
    // //djordch:
    // for (;;)
    // {
    //     delay(200);
    //     led_matrix.clear();
    //     render_stone_matrix();
    //     render_game_state();
    //     //rotate_right();
    //     led_matrix.show();
    //     if (current_stone.y_offset > 3)
    //         current_stone.y_offset--;
    //     else
    //         break;
    // }

    // for (uint8_t i = 0; i < 4; i++)
    // {
    //     led_matrix.clear();
    //     render_stone_matrix();
    //     render_game_state();
    //     rotate_right();
    //     led_matrix.show();
    //     delay(200);
    // }

    // for (;;)
    // {
    //     led_matrix.clear();
    //     render_stone_matrix();
    //     render_game_state();
    //     //rotate_right();
    //     led_matrix.show();
    //     if (current_stone.y_offset > 0)
    //         current_stone.y_offset--;
    //     else
    //         break;
    //     delay(200);
    // }

    // selected_stone = (selected_stone + 1) % 7;
    // select_random_stone();
    // current_stone.y_offset = 7;
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
                        led_matrix.setPixelColor(led_num, current_stone.r, current_stone.g, current_stone.b);
                    }
                    break;
                case 2:
                    if (current_stone.matrix.s2.stone[row][col] == true)
                    {
                        led_matrix.setPixelColor(led_num, current_stone.r, current_stone.g, current_stone.b);
                    }
                    break;
                }
            }
        }
    }
    return false;
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
                replacement.s3.stone[col][current_stone_test.order - 1 - row] =
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
        for (uint8_t col = 0;col < current_stone_test.order; col++)
        {
            if (0 <= col + current_stone_test.x_offset &&
                COLUMNS - 1 >= col + current_stone_test.x_offset &&
                0 <= row + current_stone_test.y_offset &&
                ROWS - 1 >= row + current_stone_test.y_offset)
            {
                switch (current_stone_test.order)
                {
                case 3:
                    if (game_state[row + current_stone_test.y_offset][col + current_stone_test.x_offset] && current_stone_test.matrix.s3.stone[row][col])
                    {
                        //printdebugpixel();
                        return true;
                    }
                    break;
                case 2:
                    if (game_state[row + current_stone_test.y_offset][col + current_stone_test.x_offset] && current_stone_test.matrix.s2.stone[row][col])
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
            if (0 > col + current_stone_test.x_offset || COLUMNS - 1 < col + current_stone_test.x_offset || 0 > row + current_stone_test.y_offset)
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
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset] = true; //current_stone.matrix.s3.stone[i][j];
                    break;
                case 2:
                    if (current_stone.matrix.s2.stone[row][col])
                        game_state[row + current_stone.y_offset][col + current_stone.x_offset] = true; //current_stone.matrix.s2.stone[i][j];
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
        for (uint8_t col = 0; col < COLUMNS - 1; col++)
        {
            if (game_state[row][col] == false)
                break;
            else if (game_state[row][COLUMNS - 1])
            {
                removeFilledRow(row);
                row--;
            }
        }
    }
}
void removeFilledRow(uint8_t row)
{
    for (; row < ROWS; row++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            if (row == ROWS - 1)
            {
                game_state[row][col] = false;
            }
            else
            {
                game_state[row][col] = game_state[row + 1][col];
            }
        }
    }
}
