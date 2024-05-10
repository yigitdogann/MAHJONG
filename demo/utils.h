#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

void InitImagesSounds();

void readFile(GameState*);

void InitMap();

void InitObjects(LastTwoClicked*);

bool isClickable(tile*);

// son tiklanilan 2 tas masadan kaldirilabilir mi? YES : NO
bool isRemovable(LastTwoClicked);

tile* getTopMostTile(tile, Vector2);

void shuffleBasedOnCondition(int*, int*, int, int);

void shuffle_all(LastTwoClicked*, int*, int*);

void resetHint(LastTwoClicked*);
//void remover(LastTwoClicked* hint)

void resetLastClicks(LastTwoClicked*);

void randomFiller(int*);

void unloadGameSounds();

void addPoints(LastTwoClicked, GameState*);

void deletePointsforHint(GameState*);

void deletePointsforShuffle(GameState*);

int countMatchableTiles(GameState*);

void minutesAndSeconds(GameState*);

void isGameOver(GameState*);

void drawGame();

void resetGame();

void savingText();

void sort_and_write_scores(const char*,const char*, int);

int compare_scores(const void*, const void*);

void print10();

void updateCombo(GameState*);

void drawCombo();

//void shake();

void endScreen();

#endif