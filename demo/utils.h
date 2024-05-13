#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

void InitImages();

void InitSounds();

void readFile(GameState*);

void InitMap();

void InitObjects();

bool isClickable(tile*);

//son tiklanilan 2 tas masadan kaldirilabilir mi? YES : NO
bool isRemovable(LastTwoClicked);

tile* getTopMostTile(tile, Vector2);

void shuffleTilesBasedOnState(int*, int*, int, int);

void AssignNewValuesToTiles(int*, int*);

void resetHint(LastTwoClicked*);

void resetLastClicks(LastTwoClicked*);

void setupTileIDs(int*);

void unloadGameSounds();

void addPoints(LastTwoClicked, GameState*);

void deletePointsforHint(GameState*);

void deletePointsforShuffle(GameState*);

int countMatchableTiles(GameState*);

void updateTime(GameState*);

void isGameOver(GameState*);

void drawGame();

void resetGame();

void savingText();

void sort_and_write_scores(const char*,const char*, int);

int compare_scores(const void*, const void*);

void print10();

void updateCombo(GameState*);

void drawCombo();

void endScreen();

void InitCamera(Camera2D*);

void checkCameraForShake(Camera2D*);

#endif