#include "PongWars.h"

extern uint8_t *framebuffer;

PongWars::PongWars(int block_dim, int seed) {
    srand(seed);
    this->block_dim = block_dim;
    num_blocks_x = EPD_WIDTH / block_dim;
    num_blocks_y = EPD_HEIGHT / block_dim;
    
    // Allocate grid in PSRAM
    grid = (uint8_t *)ps_malloc(num_blocks_x * num_blocks_y);
    if (!grid) {
        Serial.println("Failed to allocate grid memory in PSRAM!");
        while (1);
    }
    // Initialize grid to BLACK
    memset(grid, BLACK, num_blocks_x * num_blocks_y);
    
    // Allocate circle pattern in PSRAM
    circle = (int *)ps_malloc(block_dim * block_dim * sizeof(int));
    if (!circle) {
        Serial.println("Failed to allocate circle memory in PSRAM!");
        while (1);
    }
    // Initialize circle to 0
    memset(circle, 0, block_dim * block_dim * sizeof(int));
    
    initializeCircle();
    initializeGrid();
}

PongWars::~PongWars() {
    if (grid) {
        free(grid);
        grid = nullptr;
    }
    if (circle) {
        free(circle);
        circle = nullptr;
    }
}

int8_t PongWars::getRandomDirection() {
    return random(0, 2) ? 1 : -1;
}

int32_t PongWars::getRandomNumber(int32_t min, int32_t max) {
    return random(min, max + 1);
}

void PongWars::initializeGrid() {
    int side = seed & 1;
    if (side) {
            for (int i = 0; i < num_blocks_x/2; i++) {
        for (int j = 0; j < num_blocks_y; j++) {
            getGrid(i, j) = WHITE;
        }
    }
    for (int i = num_blocks_x/2; i < num_blocks_x; i++) {
        for (int j = 0; j < num_blocks_y; j++) {
            getGrid(i, j) = BLACK;
        }
    }
        black_ball = {getRandomNumber(1, num_blocks_x / 2 - 2), getRandomNumber(1, num_blocks_y - 2), getRandomDirection(), getRandomDirection(), BLACK};
        white_ball = {getRandomNumber(num_blocks_x / 2 + 1, num_blocks_x - 2), getRandomNumber(1, num_blocks_y - 2), getRandomDirection(), getRandomDirection(), WHITE};
    } else {
        for (int i = 0; i < num_blocks_x/2; i++) {
            for (int j = 0; j < num_blocks_y; j++) {
                getGrid(i, j) = BLACK;
            }
        }
        for (int i = num_blocks_x/2; i < num_blocks_x; i++) {
            for (int j = 0; j < num_blocks_y; j++) {
                getGrid(i, j) = WHITE;
            }
        }
        white_ball = {getRandomNumber(1, num_blocks_x / 2 - 2), getRandomNumber(1, num_blocks_y - 2), getRandomDirection(), getRandomDirection(), WHITE};
        black_ball = {getRandomNumber(num_blocks_x / 2 + 1, num_blocks_x - 2), getRandomNumber(1, num_blocks_y - 2), getRandomDirection(), getRandomDirection(), BLACK};
    }
}

void PongWars::initializeCircle() {
    int center = block_dim / 2;
    int radius = block_dim / 2;

    for (int i = 0; i < block_dim; ++i) {
        for (int j = 0; j < block_dim; ++j) {
            int dx = i - center;
            int dy = j - center;
            if (dx * dx + dy * dy <= radius * radius) {
                getCircle(i, j) = 1;
            }
        }
    }
}

void PongWars::drawCircle(int32_t x, int32_t y, uint8_t color) {
    int raw_x = x * block_dim;
    int raw_y = y * block_dim;

    if(color == WHITE) {
        for(int32_t i = 0; i < block_dim; i++) {
            for(int32_t j = 0; j < block_dim; j++) {
                epd_fill_rect(raw_x + i, raw_y + j, 1, 1, getCircle(i, j) * 0xFF, framebuffer);
            }
        }
    } 
    else {
        for(int32_t i = 0; i < block_dim; i++) 
            for(int32_t j = 0; j < block_dim; j++) {
                epd_fill_rect(raw_x + i, raw_y + j, 1, 1, !getCircle(i, j) * 0xFF, framebuffer);
            }
    }
}

void PongWars::drawBlock(int32_t x, int32_t y, uint8_t color) {
    int32_t raw_x = x * block_dim;
    int32_t raw_y = y * block_dim;
    epd_fill_rect(raw_x, raw_y, block_dim, block_dim, color * 0xFF, framebuffer);
}

void PongWars::drawGrid() {
    for (int32_t i = 0; i < num_blocks_x; i++) {
        for (int32_t j = 0; j < num_blocks_y; j++) {
            if (black_ball.x == i && black_ball.y == j) {
                drawCircle(i, j, BLACK);
            } else if (white_ball.x == i && white_ball.y == j) {
                drawCircle(i, j, WHITE);
            } else if (getGrid(i, j) == BLACK) {
                drawBlock(i, j, BLACK);
            } else if (getGrid(i, j) == WHITE) {
                drawBlock(i, j, WHITE);
            }
        }
    }
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
}

void PongWars::handleCollisions(Ball &ball) {
    int current_color = ball.color;
    while (true) {
        int new_x = ball.x + ball.dx;
        int new_y = ball.y + ball.dy;
        bool flag = false;
        bool directionChanged = false;

        if ((new_x >= 0 && new_x < num_blocks_x && getGrid(new_x, ball.y) == current_color) || (new_x < 0 || new_x >= num_blocks_x)) {
            ball.dx = -ball.dx;
            if (new_x >= 0 && new_x < num_blocks_x && getGrid(new_x, ball.y) == current_color) {
                getGrid(new_x, ball.y) = !current_color;
            }
            flag = true;
            directionChanged = true;
        }
        if ((new_y >= 0 && new_y < num_blocks_y && getGrid(ball.x, new_y) == current_color) || (new_y < 0 || new_y >= num_blocks_y)) {
            ball.dy = -ball.dy;
            if (new_y >= 0 && new_y < num_blocks_y && getGrid(ball.x, new_y) == current_color) {
                getGrid(ball.x, new_y) = !current_color;
            }
            flag = true;
            directionChanged = true;
        }
        if (!flag && new_x >= 0 && new_x < num_blocks_x && new_y >= 0 && new_y < num_blocks_y && getGrid(new_x, new_y) == current_color) {
            ball.dx = -ball.dx;
            ball.dy = -ball.dy;
            getGrid(new_x, new_y) = !current_color;
            directionChanged = true;
        }
        if (!directionChanged) {
            break;
        }
    }
}

void PongWars::moveBall(Ball &ball) {
    ball.x += ball.dx;
    ball.y += ball.dy;
}

void PongWars::reset(int new_block_dim) {
    // Free old memory
    if (grid) {
        free(grid);
        grid = nullptr;
    }
    if (circle) {
        free(circle);
        circle = nullptr;
    }
    
    // Update dimensions
    block_dim = new_block_dim;
    num_blocks_x = EPD_WIDTH / block_dim;
    num_blocks_y = EPD_HEIGHT / block_dim;
    
    // Allocate new grid in PSRAM
    grid = (uint8_t *)ps_malloc(num_blocks_x * num_blocks_y);
    if (!grid) {
        Serial.println("Failed to allocate grid memory in PSRAM!");
        while (1);
    }
    // Initialize grid to BLACK
    memset(grid, BLACK, num_blocks_x * num_blocks_y);
    
    // Allocate new circle pattern in PSRAM
    circle = (int *)ps_malloc(block_dim * block_dim * sizeof(int));
    if (!circle) {
        Serial.println("Failed to allocate circle memory in PSRAM!");
        while (1);
    }
    // Initialize circle to 0
    memset(circle, 0, block_dim * block_dim * sizeof(int));
    
    // Clear framebuffer and screen
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    epd_clear();
    
    // Reinitialize
    initializeCircle();
    initializeGrid();
    
    // Run simulation steps
    for (int i = 0; i < 1e6; i++) {
        step();
    }
}

void PongWars::step() {
    handleCollisions(black_ball);
    handleCollisions(white_ball);

    moveBall(black_ball);
    moveBall(white_ball);
}

uint8_t* PongWars::getFrameBuffer() {
    return framebuffer;
}

void PongWars::draw() {
    drawGrid();
} 