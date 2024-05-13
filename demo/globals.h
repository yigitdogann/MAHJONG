#ifndef GLOBAL_H
#define GLOBAL_H

#include <raylib.h>     // Include Raylib library for game graphics and input handling.
#include <stdio.h>      // Include for standard input and output functions.
#include <stdlib.h>     // Include for standard library functions, including memory management.
#include <time.h>       // Include for handling time-related functions, useful for game timing.
#include <ctype.h>      // Include for character type functions, such as checking character properties.

#define WIDTH 70       // Define the width of each tile.
#define HEIGHT 96      // Define the height of each tile.
#define ARRAY_Y 18     // Define the number of rows in the tile array.
#define ARRAY_X 34     // Define the number of columns in the tile array.
#define LAYER 6        // Define the number of layers of tiles.
#define NUM_IMAGES 36  // Define the number of different tile images.
#define ARRAY_SIZE 144 // Define the total number of tiles.
#define MAX_LINE_LENGTH 30 // Define the maximum length of a line for text handling.
#define MAX_LINES 1024     // Define the maximum number of lines, for high score entries or logs.

// Define a structure for tiles in the game.
typedef struct tile {
    int point;          // Points value of the tile.
    int id;             // Unique identifier for the tile type.
    int order;          // Ordering used for sorting or logic decisions.
    bool isExists;      // Flag to check if the tile currently exists on the board.
    bool isClickable;   // Flag to check if the tile can be interacted with.

    Rectangle rectangle; // Geometric data for drawing the tile.
    Texture2D texture;   // Texture of the tile.
    Color color;         // Color of the tile for additional graphical effects.
    int x, y, z;         // Position of the tile in the grid and layer.
    int combo;           // Combo multiplier associated with this tile.
} tile;

// Structure to manage the last two clicked tiles in the game.
typedef struct LastTwoClicked {
    tile* previousClicked; // Pointer to the previously clicked tile.
    tile* lastClicked;     // Pointer to the most recently clicked tile.
} LastTwoClicked;

// Enumeration to define different screen states in the game.
typedef enum screen { starting, game, gameOver, win } screen;

// Structure to manage the overall game state.
typedef struct GameState {
    bool isGameActive;     // Flag to check if the game is currently active.
    bool isMapSelected;    // Flag to check if a map has been selected.

    int remainingTile;     // Counter for the remaining tiles.
    int totalPoint;        // Accumulated points in the game.
    int combo;             // Current combo multiplier.
    int matchable;         // Number of matchable tile pairs available.

    int minutes;           // Remaining minutes for timed game modes.
    int seconds;           // Remaining seconds for timed game modes.

    screen gameScreen;     // Current screen or state of the game.

    double startTime;      // Record of game start time.
    double gameTime;       // Running total of game time elapsed.
    double lastMatchTime;  // Time of the last match made.
    double lastUndoTime;   // Time when the last undo operation was performed.
} GameState;

// Node structure for a linked list to handle undos or history tracking in the game.
typedef struct node {
    tile* data1;           // Pointer to one of the two tiles in the node.
    tile* data2;           // Pointer to the other tile in the node.
    struct node* nextNode; // Pointer to the next node in the linked list.
} node;

// Structure to manage score entries.
typedef struct {
    int points;            // Points scored in the entry.
    char text[30];         // Text associated with the score, typically the player's name.
} ScoreEntry;

extern Texture2D textures[NUM_IMAGES];
extern Texture2D backGroundTexture[4];
extern Texture2D symbolsTexture[8];

extern ScoreEntry entries[MAX_LINES];

extern char lines[MAX_LINES][MAX_LINE_LENGTH];
extern char text[256];
extern bool saveGuiVisible;
extern bool saveText;

extern int* num_lines_read;
extern char array[10][30]; // Array of strings, each can hold up to 29 characters plus null terminator
extern int point[10];      // Array to store integers

extern int offset;

extern Sound gameSound;
extern Sound buttonSound;
extern Sound selectSound;
extern Sound mapSelectionSound;
extern Sound gameButtonSound;
extern Sound shuffleSound;

extern LastTwoClicked LastClicks;
extern LastTwoClicked hint;

extern tile tiles[ARRAY_Y][ARRAY_X][LAYER];
extern Vector2 mousePosition;
extern GameState gameState;

extern int map[ARRAY_Y][ARRAY_X];
extern int newMap[ARRAY_Y][ARRAY_X][LAYER];

extern int tileIDs[ARRAY_SIZE];
extern int isExist[ARRAY_SIZE];
extern int clickableTilesPerType[NUM_IMAGES];

extern const int screenWidth;
extern const int screenHeight;

extern int framesCounter; // Useful to count frames
extern FILE* file;
extern node* head; //linked list head pointer 

extern int shakeTime;
extern float shakeMagnitude;
extern Camera2D camera;

#endif