#include "globals.h"

Texture2D textures[NUM_IMAGES]; // Stores textures for different tiles or game elements.
Texture2D backGroundTexture[4]; // Array to hold different background textures for various game screens.
Texture2D symbolsTexture[8]; // Array for storing symbol textures used in the game's GUI.

ScoreEntry entries[MAX_LINES]; // Array to store score entries for displaying or logging.
char lines[MAX_LINES][MAX_LINE_LENGTH]; // Buffer for reading lines of text, typically from files.
char text[256] = "\0"; // Text storage, used in text input or display.
bool saveText = false; // Flag to determine if text needs to be saved, used in text editing or submission.
bool saveGuiVisible = true; // Flag to control visibility of the save GUI component.

int* num_lines_read = 10; // Pointer to an integer tracking the number of lines read, commonly used in file operations.
char array[10][30]; // Array of strings for storing short texts or labels.
int point[10]; // Array to store point values, possibly for scoring or indexing purposes.

int offset = 80; // Commonly used offset value for positioning elements in the UI.

Sound gameSound; // Background game music.
Sound buttonSound2; // Sound played when buttons are clicked.
Sound selectSound; // Sound for selecting options or tiles.
Sound mapSelectionSound; // Sound specific to selecting maps in the game.
Sound buttonSound1; // General sound for game-related button interactions.
Sound shuffleSound; // Sound played when tiles or elements are shuffled.

LastTwoClicked LastClicks; // Struct to keep track of the last two tiles clicked by the user.
LastTwoClicked hint; // Struct to store hint information related to tile selection.

tile tiles[ARRAY_Y][ARRAY_X][LAYER]; // 3D array of tile structures for the game board.
Vector2 mousePosition; // Structure to hold the current mouse position, used in click detection.
GameState gameState = { 0 }; // Struct to hold various states and statuses of the game.

int map[ARRAY_Y][ARRAY_X]; // 2D array representing the basic tile map or layout.
int newMap[ARRAY_Y][ARRAY_X][LAYER] = { 0 }; // 3D array for managing layered tile arrangements.

int tileIDs[ARRAY_SIZE]; // Array of tile IDs, mapping to textures or types.
int isExist[ARRAY_SIZE] = { 0 }; // Array tracking the existence state of tiles(If exist 1, else 0).
int clickableTilesPerType[NUM_IMAGES]; // Array counting how many tiles of each type are clickable.

const int screenWidth = 1440; // Fixed screen width for the game window.
const int screenHeight = 900; // Fixed screen height for the game window.

int framesCounter = 0; // Counter for frames, used in animations or timing functions.
FILE* file; // File pointer for reading or writing data to files.
node* head = NULL; // Head pointer for a linked list, possibly used for tracking game elements or history.

int shakeTime = 0; // Counter or timer for implementing a shake effect.
float shakeMagnitude = 10.0f; // Magnitude of shake, used in visual effects.
Camera2D camera = { 0 }; // Camera for 2D rendering, initialized to zero.
