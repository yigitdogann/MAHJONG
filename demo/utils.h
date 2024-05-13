#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

// Load and initialize all images required by the game.
void InitImages();

// Load and initialize all sound effects and music.
void InitSounds();

// Read text files according to map selection
void readFile(GameState*);

// Initialize tile coordinates from the 2D array to 3D array
void InitMap();

// Initialize game objects from 3D array and ID.
void InitObjects();

// Determine if a given tile can be interacted with.
bool isClickable(tile*);

// Check if the last two clicked tiles can be removed from the game board.
bool isRemovable(LastTwoClicked);

// Get the topmost tile at a given mouse position.
tile* getTopMostTile(tile, Vector2);

// Shuffle tiles based on the current game state or conditions.
void shuffleTilesBasedOnState(int*, int*, int, int);

// Assign new values to tiles after shuffling.
void AssignNewValuesToTiles(int*, int*);

// Reset hints to a default state, clearing any current selections.
void resetHint(LastTwoClicked*);

// Reset the last clicked tiles to prevent unintended interactions.
void resetLastClicks(LastTwoClicked*);

// Set up initial tile IDs for new games or sessions.
void setupTileIDs(int*);

// Unload and free up memory used by game sounds.
void unloadGameSounds();

// Add points to the game state based on the last two clicked tiles.
void addPoints(LastTwoClicked, GameState*);

// Deduct points for using a hint, affecting the game score.
void deletePointsforHint(GameState*);

// Deduct points for shuffling tiles, affecting the game score.
void deletePointsforShuffle(GameState*);

// Count how many tiles can currently be matched in the game.
int countMatchableTiles(GameState*);

// Update the game time and other related timing functions.
void updateTime(GameState*);

// Determine if the game is over based on the current game state.
void isGameOver(GameState*);

// Draw the game interface and tiles on the screen.
void drawGame();

// Reset the entire game to its initial state.
void resetGame();

// Display and handle text input for saving the game or scores.
void savingText();

// Sort scores and write them to a file.
void sort_and_write_scores(const char*, const char*, int);

// Compare two scores to determine their order.
int compare_scores(const void*, const void*);

// Display the top 10 scores on the screen.
void print10();

// Update the combo multiplier based on game interactions.
void updateCombo(GameState*);

// Draw the combo indicator on the game screen.
void drawCombo();

// Handle the end-of-game screen, displaying scores and options.
void endScreen();

// Initialize the camera for the game, setting default parameters.
void InitCamera(Camera2D*);

// Check and apply a shaking effect to the camera if required.
void checkCameraForShake(Camera2D*);

#endif
