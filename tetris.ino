#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 13

#define ROWS 8
#define COLUMNS 5

typedef struct{
  bool stone[3][3];
} Stone_3x3;

typedef struct{
  bool stone[2][2];
} Stone_2x2;

typedef union
{
  Stone_3x3 s3;
  Stone_2x2 s2;
} Stone_Matrix;

typedef struct{
  Stone_Matrix matrix;
  uint8_t order;
  int8_t x_offset;
  uint8_t y_offset;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Current_Stone;

Adafruit_NeoPixel led_matrix = Adafruit_NeoPixel(40,PIN ,NEO_GRB +
NEO_KHZ800);

bool game_state[ROWS][COLUMNS] = {false};

Current_Stone current_stone;// tetorid, that moves

Stone_Matrix replacement;   // memory region for rotation

uint8_t selected_stone = 0;

void setup()
{
    led_matrix.begin();
    led_matrix.setBrightness(5);
    led_matrix.show();

    select_random_stone();
    current_stone.x_offset = 1;
    current_stone.y_offset = 7;
    render_stone_matrix();
    led_matrix.show();
}

// the loop routine runs over and over again forever:
void loop()
{
    for(;;)
    {
      delay(200);
      led_matrix.clear();
      render_stone_matrix();
      render_game_state();
      //rotate_right();
      led_matrix.show();
      if (current_stone.y_offset > 3)
        current_stone.y_offset--;
      else
        break;
    }

    for(uint8_t i=0; i < 4; i++)
    {
      led_matrix.clear();
      render_stone_matrix();
      render_game_state();
      rotate_right();
      led_matrix.show();
      delay(200);
    }

    for(;;)
    {
      led_matrix.clear();
      render_stone_matrix();
      render_game_state();
      //rotate_right();
      led_matrix.show();
      if (current_stone.y_offset > 0)
        current_stone.y_offset--;
      else
        break;
      delay(200);
    }

    selected_stone = (selected_stone + 1) % 7;
    select_random_stone();
    current_stone.y_offset = 7;
}

void select_random_stone()
{
    // select random stone
    //uint8_t selection = random(7);
    uint8_t selection = selected_stone; // XXX

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
                  false,true,false,
                  false,true,false,
                  false,true,false,
                };
                break;
            }

        case 1:
            {
                current_stone.order = 3;
                current_stone.matrix.s3 = {
                  false,true ,false,
                  false,true ,false,
                  true ,true ,false
                };
                break;
            }

        case 2:
            {
                current_stone.order = 3;
                current_stone.matrix.s3 = {
                  false,true ,false,
                  false,true ,false,
                  false,true ,true
                };
                break;
            }

        case 3:
            {
                current_stone.order = 2;
                current_stone.matrix.s2 = {
                  true ,true ,
                  true ,true
                };
                break;
            }

        case 4:
            {
                current_stone.order = 3;
                current_stone.matrix.s3 = {
                  false,false,false,
                  false,true ,true ,
                  true ,true ,false
                };
                break;
            }

        case 5:
            {
                current_stone.order = 3;
                current_stone.matrix.s3 = {
                  false,false,false,
                  true ,true ,true ,
                  false,true ,false
                };
                break;
            }

        case 6:
            {
                current_stone.order = 3;
                current_stone.matrix.s3 = {
                  false,false,false,
                  true ,true ,false,
                  false,true ,true
                };
                break;
            }

        default:
            {
                //TODO
                break;
            }
    }
}

void render_game_state()
{
    uint8_t led_num = 0;
    for(uint8_t i=0; i < ROWS; i++)
    {
        for(uint8_t j=0; j < COLUMNS; j++)
        {
            if (game_state[i][j] == true)
            {
                led_num = j * 8 + i - 1;
                led_matrix.setPixelColor(led_num, 255, 0, 0); //TODO
            }
            led_num++;
        }
    }
}

void render_stone_matrix()
{
    if (current_stone.y_offset > 7)
        return;

    // was muss gezeichnet werden?
    uint8_t led_num;

    led_matrix.setPixelColor(32 + current_stone.y_offset, 0, 0, 255); //TODO

    if (current_stone.order == 2)
    {
      for(uint8_t i=0; i < current_stone.order; i++)
      {
          if (!(current_stone.y_offset == 7 && i == 0))
          {
              for(uint8_t j=0; j < current_stone.order; j++)
              {
                if(current_stone.matrix.s2.stone[i][j] == true)// &&
current_stone.y_offset != 0)
                {
                  led_num = ((current_stone.x_offset + j) * 8) +
current_stone.y_offset + current_stone.order - i - 1;
                  led_matrix.setPixelColor(led_num, current_stone.r,
current_stone.g, current_stone.b); //TODO
                }
/*
                // versuch, den stein in die
                else if(current_stone.matrix.s2.stone[i][j] == true &&
current_stone.y_offset == 0)
                {
                    game_state[current_stone.x_offset + i][j] = true;
                }
            */
              }
          }
      }
    }

    else
    {
      for(uint8_t i=0; i < current_stone.order; i++)
      {
          if (!(current_stone.y_offset == 7 && i == 0) &&
!(current_stone.y_offset == 7 && i == 1) && !(current_stone.y_offset ==
6 && i == 0))
          {
              for(uint8_t j=0; j < current_stone.order; j++)
              {
                if(current_stone.matrix.s3.stone[i][j] == true)
                {
                  led_num = ((current_stone.x_offset + j) * 8) +
current_stone.y_offset + current_stone.order - i - 1;
                  led_matrix.setPixelColor(led_num, current_stone.r,
current_stone.g, current_stone.b); //TODO
                }
              }
          }
      }
    }

}

void rotate_right()
{
    if(current_stone.order == 3)
    {
        for(uint8_t i = 0; i < current_stone.order; i++ )
        {
            for(uint8_t j = 0; j < current_stone.order; j++)
            {
              replacement.s3.stone[i][j] =
current_stone.matrix.s3.stone[j][current_stone.order - 1 - i];
            }
        }

        current_stone.matrix = replacement;
      }
}

void rotate_left()
{
    if(current_stone.order == 3)
    {
        for(uint8_t i = 0; i < current_stone.order; i++ )
        {
            for(uint8_t j = 0; j < current_stone.order; j++)
            {
              replacement.s3.stone[j][current_stone.order - 1 - i] =
current_stone.matrix.s3.stone[i][j];
            }
        }

        current_stone.matrix = replacement;
      }
}