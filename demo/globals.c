#include "globals.h"

Image images[NUM_IMAGES];    // Array to hold image data
Texture2D textures[NUM_IMAGES]; // Array to hold textures

Image backGround[4];    // background
Texture2D backGroundTexture[4];

Image symbols[6];
Texture2D symbolsTexture[6];

Sound gameSound;
Sound buttonSound;
Sound selectSound;
Sound mapSelectionSound;
Sound gameButtonSound;

LastTwoClicked LastClicks;
LastTwoClicked hint;

int map[ARRAY_Y][ARRAY_X];
int newMap[ARRAY_Y][ARRAY_X][LAYER] = { 0 };

int original[ARRAY_SIZE];
int shuffled[ARRAY_SIZE];
int isExist[ARRAY_SIZE] = { 0 };
int clickable_freq[ARRAY_SIZE];

tile tiles[ARRAY_Y][ARRAY_X][LAYER];
int values[ARRAY_Y][ARRAY_X];
Vector2 mousePosition;
GameState gameState = {0};

const int screenWidth = 1440;
const int screenHeight = 900;
int framesCounter = 0; // Useful to count frames
FILE* file;
node* head = NULL; //linked list head pointer 