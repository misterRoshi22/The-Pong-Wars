#ifndef PONGWARS_H
#define PONGWARS_H

#include <Arduino.h>
#include "epd_driver.h"

// Constants for colors
const int BLACK = 0;
const int WHITE = 1;

class PongWars {
private:
    struct Ball {
        int32_t x, y, dx, dy, color;
    } white_ball, black_ball;

    uint8_t *grid = nullptr;
    int *circle = nullptr;

    int block_dim;
    int num_blocks_x;
    int num_blocks_y;
    
    #define GRID(x, y) grid[(x) * num_blocks_y + (y)]
    #define CIRCLE(x, y) circle[(x) * block_dim + (y)]

    int8_t getRandomDirection();
    int32_t getRandomNumber(int32_t min, int32_t max);
    void initializeGrid();
    void initializeCircle();
    void drawCircle(int32_t x, int32_t y, uint8_t color);
    void drawBlock(int32_t x, int32_t y, uint8_t color);
    void drawGrid();
    void handleCollisions(Ball &ball);
    void moveBall(Ball &ball);
    
public:
    PongWars(int block_dim, int num_blocks_x, int num_blocks_y);
    ~PongWars();
    void reset(int new_block_dim = 15);
    void step();
    uint8_t* getFrameBuffer();
    void draw();
};

#endif // PONGWARS_H 