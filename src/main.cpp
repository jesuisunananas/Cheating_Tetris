#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <string>
#include <random>

// mechanics
// collision detection
// physics
using Piece = int[4][4];

const int BOARD_HEIGHT = 20;
const int BOARD_WIDTH = 10;
const int CELL_SIZE = 20;

int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

struct Tetromino {
    int shape[4][4];
    int x,y;
    int type;
    int rotation;
    Color color;
};

Tetromino current_piece;

int O_PIECE[4][4] = {
    {0,0,0,0},
    {0,2,2,0},
    {0,2,2,0},
    {0,0,0,0}
};

int S_PIECE[4][4] = {
    {0,0,0,0},
    {0,0,3,3},
    {0,3,3,0},
    {0,0,0,0}
};

int Z_PIECE[4][4] = {
    {0,0,0,0},
    {4,4,0,0},
    {0,4,4,0},
    {0,0,0,0}
};

int J_PIECE[4][4] = {
    {0,5,0,0},
    {0,5,5,5},
    {0,0,0,0},
    {0,0,0,0}
};

int L_PIECE[4][4] = {
    {0,0,6,0},
    {6,6,6,0},
    {0,0,0,0},
    {0,0,0,0}
};

int T_PIECE[4][4] = {
    {0,0,0,0},
    {0,7,7,7},
    {0,0,7,0},
    {0,0,0,0}
};

int I_PIECE[4][4] = {
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,0,0,0}
};

const Piece* pieces[] = {
    &I_PIECE,
    &O_PIECE,
    &S_PIECE,
    &Z_PIECE,
    &J_PIECE,
    &L_PIECE,
    &T_PIECE,
};

void rotate_piece(int piece[][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = i+1; j < 4; j++) {
            std::swap(piece[i][j], piece[j][i]);
        }
    }
    for (int i = 0; i < 4; i++) {
        std::reverse(std::begin(piece[i]), std::end(piece[i]));
    }
}

bool is_row_full(int row_idx) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        if (board[row_idx][i] == 0) {
            return false;
        }
    }
    return true;
}

void clear_row(int row_idx) {
    for (int col = 0; col < BOARD_WIDTH; col++) {
        board[row_idx][col] = 0;
    }
}

bool enforce_gravity(int row_idx) {
    for (int i = row_idx; i > 0; i--) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = board[i-1][j];
        }
    }
    for (int i = 0; i < BOARD_WIDTH; i++) {
        board[0][i] = 0;
    }
    return true;
}

bool clear_lines() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        if (is_row_full(i)) {
            clear_row(i);
            enforce_gravity(i);
        }
    }
    return true;
}

Color get_color(int given) {
    switch(given) {
        case 1:
            return SKYBLUE;
        case 2:
            return YELLOW;
        case 3:
            return GREEN;
        case 4:
            return RED;
        case 5:
            return DARKBLUE;
        case 6:
            return ORANGE;
        case 7:
            return PURPLE;
    }
}

void spawn_piece() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,6);
    int chosen = dist(rng);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            current_piece.shape[i][j] = (*pieces[chosen])[i][j];
        }
    }
    // will change to last known location
    current_piece.x = BOARD_WIDTH / 2 - 2;
    current_piece.y = 0;
    current_piece.type = chosen + 1;
    current_piece.color = get_color(current_piece.type);
}

void draw_piece(Tetromino piece) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece.shape[i][j] != 0) {
                int screen_x = (GetScreenWidth() / 2) - (BOARD_WIDTH * CELL_SIZE / 2) + ((piece.x + j) * CELL_SIZE);
                int screen_y = 50 + (piece.y + i) * CELL_SIZE;
                DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, piece.color);
                DrawRectangleLines(screen_x, screen_y, CELL_SIZE, CELL_SIZE, DARKGRAY);
            }
        }
    }
}

void write_piece_to_board(Tetromino piece) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece.shape[i][j] != 0) {
                board[piece.y+i][piece.x+j] = piece.type;
            }
        }
    }
}

bool check_collision(Tetromino piece, int offset_x, int offset_y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece.shape[i][j] != 0) {
                int new_x = piece.x + j + offset_x;
                int new_y = piece.y + i + offset_y;
                
                if (new_x < 0 || new_x >= BOARD_WIDTH || new_y >= BOARD_HEIGHT) {
                    if (new_y >= BOARD_HEIGHT) {
                        write_piece_to_board(piece);
                        spawn_piece();
                    }
                    return true;
                }
                
                if (new_y >= 0 && board[new_y][new_x] != 0) {
                    write_piece_to_board(piece);
                    spawn_piece();
                    return true;
                }
            }
        }
    }
    return false;
}

void try_rotate() {
    int temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i][j] = current_piece.shape[i][j];
        }
    }
    rotate_piece(current_piece.shape);
    if (check_collision(current_piece, 0, 0)) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                current_piece.shape[i][j] = temp[i][j];
            }
        }
    }
}

void draw_board() {
    clear_lines();
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            int screen_x = (GetScreenWidth() / 2) - (BOARD_WIDTH * CELL_SIZE / 2) + (j * CELL_SIZE);
            int screen_y = 50 + i * CELL_SIZE;
            if (board[i][j] > 0) {
                //use color of tetromino
                switch(board[i][j]) {
                    case 1:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, SKYBLUE);
                        break;
                    case 2:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, YELLOW);
                        break;
                    case 3:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, GREEN);
                        break;
                    case 4:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, RED);
                        break;
                    case 5:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, DARKBLUE);
                        break;
                    case 6:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, ORANGE);
                        break;
                    case 7:
                        DrawRectangle(screen_x, screen_y, CELL_SIZE, CELL_SIZE, PURPLE);
                }
            }
            DrawRectangleLines(screen_x, screen_y, CELL_SIZE, CELL_SIZE, DARKGRAY);
        }
    }
}

void gravity() {
    if (!check_collision(current_piece, 0, 1)) {
        current_piece.y++;
    }
}

// physics w/tetromino and how blocks are being generated etc.
// stack
// if stack is empty push a tetromino to stack
// if stack, push it down with gravity
// if it hits bottom we pop it from the stack

// how will tetromino move against board??

//collision detection
// check out of bounds (height width), proposed new location
// for rotation, if rotation pushes out of bounds, kick back by sliding right or left 1

int main() {
    InitWindow(800, 600, "Tetris");
    SetTargetFPS(60);
    int frame_counter = 0;
    int frames_per_action = 30;
    SetTraceLogLevel(LOG_ALL);
    
    spawn_piece();
    while (!WindowShouldClose()) {
        frame_counter++;
        if (IsKeyPressed(KEY_RIGHT) && !check_collision(current_piece, 1, 0)) {
            current_piece.x++;
        }
        if (IsKeyPressed(KEY_LEFT) && !check_collision(current_piece, -1, 0)) {
            current_piece.x--;
        }
        //if (IsKeyPressed(KEY_UP) && ) rotate_piece(current_piece.shape);
        if (IsKeyPressed(KEY_UP)) {
            try_rotate();
        }
        if (IsKeyPressed(KEY_DOWN) && !check_collision(current_piece, 0, -1)) {
            current_piece.y--;
        }
        //gravity();
        if (frame_counter >= frames_per_action) {
            gravity();
            frame_counter = 0;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_board();
        draw_piece(current_piece);
        DrawText("Tetris Board", 10, 10, 20, MAROON);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}