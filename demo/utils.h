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
#endif