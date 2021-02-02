#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cstdio>
#include<cstring>
#include<cassert>
#include<SDL2/SDL.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef float f16;
typedef double f32;

#define WIDTH 10
#define HEIGHT 22
#define VISIBLE_LENGHT 20

struct Tetrino{
    const u8 *data;
    const s32 side;
};

inline Tetrino tetrino(const u8 *data, s32 side){
    return {data, side};
}

const u8 TETRINO_1[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0 ,0 ,0 ,0,
};

const u8 TETRINO_2[] = {
    2, 2,
    2, 2
};

const u8 TETRINO_3[] = {
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

const Tetrino TETRINOS[] = {
    tetrino(TETRINO_1, 4),
    tetrino(TETRINO_2, 2),
    tetrino(TETRINO_3, 3)
};

enum Game_Phase{
    GAME_PHASE_PLAY
};

struct Piece_State{
    u8 tetrino_index;
    s32 offset_row;
    s32 offset_col;
    s32 rotation;
};

struct Game_State{
    u8 board[WIDTH * HEIGHT];
    Piece_State piece;

    Game_Phase phase;
};

struct Input_State{
    s8 dleft;
    s8 dright;
    s8 dup;
};

inline u8 matrix_get(const u8 *values, s32 width, s32 row, s32 col){
    s32 index = row*width + col;
    return values[index];
}

inline void matrix_set(u8 *values, s32 width, s32 row, s32 col, u8 value){
    s32 index = row*width + col;
    values[index] = value;
}

inline u8 tetrino_get(const Tetrino *tetrino, s32 row, s32 col, s32 rotation){
    s32 side = tetrino->side;
    switch(rotation){
        case 0:
            return tetrino->data[row*side + col];
        case 1:
            return tetrino->data[(side-col-1)*side + row];
        case 2:
            return tetrino->data[(side-row-1)*side + (side-col-1)];
        case 3:
            return tetrino->data[col*side + (side-row-1)];
    }
    return 0;
}

bool check_piece_valid(const Piece_State *piece, const u8 *board, s32 width, s32 height){
    const Tetrino *tetrino = TETRINOS + piece->tetrino_index;
    assert(tetrino);

    for(s32 row=0; row<height; row++){
        for(s32 col=0; col<width; col++){
            u8 value = tetrino_get(tetrino, row, col, piece->rotation);
            if(value>0){
                s32 board_row = piece->offset_row + row;
                s32 board_col = piece->offset_col + col;
                if(board_row<0 || board_row >=height){
                    return false;
                }
                if(board_col<0 || board_col>=width){
                    return false;
                }
                if(matrix_get(board, width, board_row, board_col)){
                    return false;
                }
            }
        }
    }
    return true;
}

void update_game_play(Game_State *game, const Input_State *input){
    Piece_State piece = game->piece;
    if(input->dleft > 0){
        piece.offset_col--;
    }
    if(input->dright > 0){
        piece.offset_col++;
    }
    if(input->dup > 0){
        piece.rotation = (piece.rotation +1)%4;
    }

    if(check_piece_valid(&piece, game->board, WIDTH, HEIGHT)){
        game->piece = piece;
    }
}

void update_game(Game_State *game, const Input_State *input){
    switch(game->phase){
        case GAME_PHASE_PLAY:
            return update_game_play(game, input);
            break;
    }
}

int main(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool quit = false;
    while(!quit){
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}