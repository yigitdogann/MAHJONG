#ifndef GLOBAL_H
#define GLOBAL_H

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define WIDTH 70
#define HEIGHT 96
#define ARRAY_Y 18
#define ARRAY_X 34
#define LAYER 6
#define NUM_IMAGES 36
#define ARRAY_SIZE 144
#define MAX_LINE_LENGTH 30
#define MAX_LINES 1024 

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
    int combo;
}tile;

typedef struct LastTwoClicked {
    tile* previousClicked;
    tile* lastClicked;
}LastTwoClicked;

typedef enum screen { starting, game, gameOver, win} screen;

typedef struct GameState {
    // Define game state variables
    bool isGameActive;
    bool isMapSelected;
    //bool runbefore;

    int remainingTile;
    int totalPoint;
    int combo;
    int matchable;

    int minutes;
    int seconds;

    screen gameScreen;

    double startTime;
    double gameTime;
    double lastMatchTime;
    double lastUndoTime;

    // Additional game states like current level, score, etc.
} GameState;

typedef struct node {
    tile* data1;
    tile* data2;
    struct node* nextNode;
}node;

typedef struct {
    int points;
    char text[30];
} ScoreEntry;

extern Image images[NUM_IMAGES];    // Array to hold image data
extern Texture2D textures[NUM_IMAGES]; // Array to hold textures

extern Image backGround[4];    // background
extern Texture2D backGroundTexture[4];

extern Image symbols[6]; 
extern Texture2D symbolsTexture[6];

extern Sound gameSound;
extern Sound buttonSound;
extern Sound selectSound;
extern Sound mapSelectionSound;
extern Sound gameButtonSound;

//extern screen gameScreen;
extern LastTwoClicked LastClicks;
extern LastTwoClicked hint;
extern Vector2 combo;

extern ScoreEntry entries[MAX_LINES];
extern char lines[MAX_LINES][MAX_LINE_LENGTH];
extern char text[256];
extern bool saveGuiVisible;
extern bool saveText;
extern int* num_lines_read;
extern char array[10][30]; // Array of strings, each can hold up to 29 characters plus null terminator
extern int point[10];      // Array to store integers

extern int offset;

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

extern int shakeTime;
extern float shakeMagnitude;
extern Camera2D camera;

#endif