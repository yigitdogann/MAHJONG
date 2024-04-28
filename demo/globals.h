#ifndef GLOBAL_H
#define GLOBAL_H

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define WIDTH 70
#define HEIGHT 96
#define ARRAY_Y 18
#define ARRAY_X 34
#define LAYER 6
#define NUM_IMAGES 36
#define ARRAY_SIZE 144

typedef struct tile {
    int point;//point of tile, tür
    int id; //shuffled, tür
    int order;
    bool isExists; //newMap, konum
    bool isClickable;

    Rectangle rectangle;
    Texture2D texture;
    Color color;
    int x, y, z;
}tile;

typedef struct LastTwoClicked {
    tile* previousClicked;
    tile* lastClicked;
}LastTwoClicked;

typedef struct GameState {
    // Define game state variables
    bool isGameActive;
    bool runbefore;

    int totalPoint;
    int combo;
    int matchable;

    double startTime;
    double currentTime;
    double lastMatchTime;

    // Additional game states like current level, score, etc.
} GameState;

typedef struct node {
    tile* data1;
    tile* data2;
    struct node* nextNode;
}node;

typedef enum screen { starting, game } screen;

extern Image images[NUM_IMAGES];    // Array to hold image data
extern Texture2D textures[NUM_IMAGES]; // Array to hold textures

extern Image backGround[2];    // background
extern Texture2D backGroundTexture[2];

extern Sound gameSound;
extern Sound buttonSound;
extern Sound selectSound;

extern screen gameScreen;
extern LastTwoClicked LastClicks;
extern LastTwoClicked hint;

extern int map[ARRAY_Y][ARRAY_X];
extern int newMap[ARRAY_Y][ARRAY_X][LAYER];

extern int original[ARRAY_SIZE];
extern int shuffled[ARRAY_SIZE];
extern int isExist[ARRAY_SIZE];
extern int clickable_freq[ARRAY_SIZE];

extern tile tiles[ARRAY_Y][ARRAY_X][LAYER];
extern int values[ARRAY_Y][ARRAY_X];
extern Vector2 mousePosition;
extern GameState gameState;

extern const int screenWidth;
extern const int screenHeight;
extern int framesCounter; // Useful to count frames
extern FILE* file;
extern node* head; //linked list head pointer 

#endif
