/*******************************************************************************************
*
*   raylib [core] example - basic window
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/


#include "raylib.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

char cwd[1024];



const int pieces[7][4][2] = {
    {{0,0},{1,0},{-1,0},{0,1}},// Pirple
    {{0,0},{1,0},{-1,0},{-1,1}},//L Orange
    {{0,0},{-1,0},{0,1},{1,1}},//Z PGren
    {{0,0},{0,1},{-1,0},{1,1}},//J BLUE
    {{0,0},{0,1},{1,0},{-1,1}},//S Red i think
    {{-1,0},{0,0},{0,1},{-1,1}},//O Yellow
    {{-2,0},{-1,0},{0,0},{1,0}},//I TERQUISE
};
const int GRID_HEIGHT = 12;
const int GRID_WIDTH = 6;

int score = 10;
int grid[GRID_WIDTH][GRID_HEIGHT];
int grid_nums[GRID_WIDTH][GRID_HEIGHT];
int level  = 0;
Color color_grid[GRID_WIDTH][GRID_HEIGHT];
Texture numbers[10];
Texture tile;

typedef struct {
    int shape[4][2];
    int type;
    int nums [4];
    int power [4];
    Color tint;
    float x;
    int y;
} piece_struct;

void draw_num(int num,int x,int y, int width, int height,Color color) {
    int len =floor(log10(num))+1;
    int font_size = floor(width/len/8)*8;
    int offset_x =(int)( width-len*font_size)/2;
    int offset_y =(int)( height-font_size)/2;

    for (int i = len-1; i >-1; i--) {
        int index = len-1-i;

        int digit = (num % (int)pow(10,i+1))/(int)pow(10,i);

        DrawTextureEx(numbers[digit],(Vector2){x+font_size*index+offset_x,y+offset_y},0,font_size/8,color);

    }
}

void init_piece(piece_struct *piece) {

    int type = GetRandomValue(0,6);

    for (int i = 0; i<4; i++) {
        piece->power[i] = GetRandomValue(0,2);
        piece->nums[i] = (1<<piece->power[i]);
        piece->shape[i][0] = pieces[type][i][0];
        piece->shape[i][1] = pieces[type][i][1];
    }
    piece->type = type;
    if (type ==0) {piece->tint = (Color) {136, 44, 237,255};}
    else if (type ==1) {piece->tint = (Color) {221, 164, 34,255};}
    else if (type ==2) {piece->tint = (Color) {138, 234, 40,255};}
    else if (type ==3) {piece->tint = (Color) {0, 0, 240,255};}
    else if (type ==4) {piece->tint = (Color) {207, 54, 22,255};}
    else if (type ==5) {piece->tint = (Color) {241, 239, 47,255};}
    else if (type ==6) {piece->tint = (Color) {0, 240, 240,255};}
    piece->x = 3;
    piece->y = 0;
}

void draw_piece(piece_struct *piece) {
    for (int i = 0; i<4; i++) {
        Rectangle rect = {piece->shape[i][0]*60+piece->x*60+2,piece->shape[i][1]*60+piece->y*60+2,56,56};

        DrawTexture(tile,piece->shape[i][0]*60+piece->x*60,piece->shape[i][1]*60+piece->y*60,piece->tint);


        draw_num(piece->nums[i],piece->shape[i][0]*60+piece->x*60+10,piece->shape[i][1]*60+piece->y*60+10,40,40,BLACK);


    }
}



void freeze_piece(piece_struct *piece) {
    for (int i = 0; i<4; i++) {
        grid[piece->shape[i][0]+(int)piece->x][piece->shape[i][1]+piece->y] = 1;
        grid_nums[piece->shape[i][0]+(int)piece->x][piece->shape[i][1]+piece->y] = piece->nums[i];
        color_grid[piece->shape[i][0]+(int)piece->x][piece->shape[i][1]+piece->y] = piece->tint;




    }

}



bool move_piece(piece_struct * piece,int x, int y){
    bool valid = true;

    for (int i = 0; i<4; i++) {
        int x_pos = piece->shape[i][0]+piece->x+x;
        int y_pos = piece->shape[i][1]+piece->y+y;
        if ( x_pos<0 || x_pos>GRID_WIDTH-1 ||y_pos>GRID_HEIGHT-1 || grid[x_pos][y_pos]==1) {
            valid = false;

        }
    }
    if (valid) {
        piece->x += x;
        piece->y += y;
    }
    return !valid;

}
void rotate_grid(int square[][6],int direction) {
    int new_square[6][6] = {0};
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<6; j++) {
            if (direction == 0) {
                new_square[i][j] = square[i][j];
            }
            else if (direction ==90) {
                new_square[j][5-i] = square[i][j];
            } else if (direction ==180) {
                new_square[i][j] = square[5-i][5-j];
            } else if (direction ==270) {
                new_square[5-j][i] = square[i][j];
            }
        }
    }
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<6; j++) {
            square[i][j] = new_square[i][j];
        }
    }

}

void rotate_color_grid(Color square[][6],int direction) {
    Color new_square[6][6] = {0};
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<6; j++) {
            if (direction == 0) {
                new_square[i][j] = square[i][j];
            }
            else if (direction ==90) {
                new_square[j][5-i] = square[i][j];
            } else if (direction ==180) {
                new_square[i][j] = square[5-i][5-j];
            } else if (direction ==270) {
                new_square[5-j][i] = square[i][j];
            }
        }
    }
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<6; j++) {
            square[i][j] = new_square[i][j];
        }
    }

}

void rotate_piece(piece_struct *piece, int dir) {

    if (piece->type <5) {
        for (int i = 0; i<4; i++) {
            int y = piece->shape[i][1];
            int x = piece->shape[i][0];
            piece->shape[i][0] = -dir*y;
            piece->shape[i][1] = dir*x;

        }
        if (move_piece(piece,0,0)) {

            for (int i = 0; i<4; i++) {
                int y = piece->shape[i][1];
                int x = piece->shape[i][0];
                piece->shape[i][0] = dir*y;
                piece->shape[i][1] = -dir*x;
            }
        }
    } else if (piece->type == 6) {
        for (int i = 0; i<4; i++) {
            int y = piece->shape[i][1] ;
            int x = piece->shape[i][0] ;
            piece->shape[i][0] = y;
            piece->shape[i][1] = x ;

        }
        if (move_piece(piece,0,0)) {

            for (int i = 0; i<4; i++) {
                int y = piece->shape[i][1] ;
                int x = piece->shape[i][0] ;
                piece->shape[i][0] = y;
                piece->shape[i][1] = x ;
            }
        }
    } else {
        for (int i = 0; i<4; i++) {
            float y = piece->shape[i][1] -0.5 ;
            float x = piece->shape[i][0] +0.5;
            piece->shape[i][0] = -dir*y -0.5;
            piece->shape[i][1] = dir*x + 0.5;

        }
    }
}
void move_grid(int dir){
    int square[6][6]= {0};
    int square_sorted[6][6] = {0};
    Color colorz [6][6];
    Color colors_sorted[6][6];
    for (int i = 0; i<6; i++) {
        for (int j = 6; j<12; j++) {
            square[i][j-6] = grid_nums[i][j];
            colorz[i][j-6] = color_grid[i][j];
        }
    }
    rotate_grid(square,(360-dir)%360);
    rotate_color_grid(colorz,(360-dir)%360);
    for (int i = 0; i<6; i++) {
        int index = 0;
        for (int j = 0; j<6; j++) {
            if (square[i][j] > 0) {
                square_sorted[i][index] = square[i][j];
                colors_sorted[i][index] = colorz[i][j];
                index++;
            }
        }
    }
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<5; j++) {
            if (square_sorted[i][j] > 0 && square_sorted[i][j] == square_sorted[i][j+1]) {
                score += square_sorted[i][j+1]*2;
                square_sorted[i][j] = square_sorted[i][j+1]*2;
                colors_sorted[i][j] = (Color){(colors_sorted[i][j].r+colors_sorted[i][j+1].r)/2,(colors_sorted[i][j].g+colors_sorted[i][j+1].g)/2,(colors_sorted[i][j].b+colors_sorted[i][j+1].b)/2,255};
                colors_sorted[i][j+1] = (Color){0,0,0,0};
                square_sorted[i][j+1] = 0;

                for (int x = j+1; x<5; x++) {
                    colors_sorted[i][x] = colors_sorted[i][x+1];
                    square_sorted[i][x] = square_sorted[i][x+1];
                }
            }
        }
    }
    rotate_grid(square_sorted,dir);
    rotate_color_grid(colors_sorted,dir);
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<6; j++) {
            grid[i][j+6] = square_sorted[i][j]>0;
            grid_nums[i][j+6] = square_sorted[i][j];
            color_grid[i][j+6] = colors_sorted[i][j];
        }
    }

}

bool is_valid(piece_struct piece) {
    bool valid = true;
    for (int i = 0; i<4; i++) {
        if (piece.shape[i][1] + piece.y > 4) {valid = false;}
    }
    return valid;
}







int main(void) {
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);

    const int SCREEN_WIDTH = 720;
    const int SCREEN_HEIGHT = 720;
    SetRandomSeed(time(NULL));
    piece_struct tetromino;
    piece_struct next_tetromino;
    init_piece(&next_tetromino);
    if (next_tetromino.type>4){
        next_tetromino.x = 9;
    }
    else {
        next_tetromino.x = 8.5;
    }
    next_tetromino.y = 8;
    init_piece(&tetromino);
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetromino");
    for (int i = 0; i<10; i++) {
        char c[2];
        sprintf(c, "%d", i);
        char numer[30] = "Assets/Font/Num";
        strcat(numer, c);
        strcat(numer, ".png");
        numbers[i] = LoadTexture(numer);


    }
    tile = LoadTexture("Assets/Tile.png");

    float dt;
    float tick = 0;
    while (!WindowShouldClose()) {
        dt = GetFrameTime();

        if (IsKeyDown(KEY_S)) {
            tick+= 5*dt;
        }else {
            tick += dt;
        }

        if (tick >= 1.0) {
            if (move_piece(&tetromino,0,1)) {
                freeze_piece(&tetromino);
                tetromino = next_tetromino;
                tetromino.x = 3;
                tetromino.y = 0;
                init_piece(&next_tetromino);
                if (next_tetromino.type>4){
                    next_tetromino.x = 9;
                }
                else {
                    next_tetromino.x = 8.5;
                }

                next_tetromino.y = 8;

            }
            tick = 0;
        }
        if (is_valid(tetromino)){

            if (IsKeyPressed(KEY_UP)) move_grid(0);
            if (IsKeyPressed(KEY_RIGHT)) move_grid(270);
            if (IsKeyPressed(KEY_DOWN)) move_grid(180);
            if (IsKeyPressed(KEY_LEFT)) move_grid(90);
        }
        if (IsKeyPressed(KEY_A)) move_piece(&tetromino,-1,0);
        if (IsKeyPressed(KEY_D)) move_piece(&tetromino,1,0);
        if (IsKeyPressed(KEY_E)) rotate_piece(&tetromino,1);
        if (IsKeyPressed(KEY_Q)) rotate_piece(&tetromino,-1);
        if (IsKeyPressed(KEY_W)) {
            while (!move_piece(&tetromino,0,1));

        }




        BeginDrawing();
        ClearBackground(BLACK);
        DrawLine(360,0,360,720,WHITE);

        draw_num(score,370,120,340,180,WHITE);




        DrawLine(0,360,360,360,WHITE);
        for (int i = 0; i<GRID_WIDTH; i++) {
            for (int j = 0; j<GRID_HEIGHT; j++) {
                if (grid[i][j] == 1) {

                    Rectangle rect = {60*i,60*j,56,56};

                    DrawTexture(tile,60*i,60*j,color_grid[i][j]);
                    draw_num(grid_nums[i][j],60*i+10,60*j+10,40,40,BLACK);

                }
            }
        }

        draw_piece(&next_tetromino);
        draw_piece(&tetromino);
        EndDrawing();
    }

}