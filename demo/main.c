#include "globals.h"
#include "utils.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void processClick(LastTwoClicked*, LastTwoClicked*, Vector2, node**);
void addBegin(node**, LastTwoClicked*, int*);
void deleteBegin(node**, int*, LastTwoClicked*);
int giveHint(LastTwoClicked*, LastTwoClicked*, int*);
tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition);
void updateAndDraw();
void updateGame(GameState*);

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Solitaire");//pencere genislik X yukseklik, pencere adi 
    
    InitAudioDevice();
    SetTargetFPS(60);
    InitImages();
    InitSounds();
    InitCamera(&camera);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    while (!WindowShouldClose()){
        checkCameraForShake(&camera);
        updateAndDraw();
    }
    unloadGameSounds();
    CloseAudioDevice();     // Close audio device
    CloseWindow();

    return 0;
}

void updateAndDraw() {
    BeginDrawing(); // Begin the drawing phase

    switch (gameState.gameScreen) {
    case starting:
        if (!gameState.isMapSelected) { // Check if the map isn't selected yet
            DrawTexture(backGroundTexture[0], 0, 0, WHITE); // Draw the main background texture
            readFile(&gameState); // Read text files and upload their contents to a 3D array
        }
        if (gameState.isMapSelected) { // If the map is selected
            DrawTexture(backGroundTexture[1], 0, 0, WHITE); // Draw the game background texture
            PlaySound(gameSound); // Play the background music for the game scene
            InitMap(); // Initialize tile coordinates from the 3D array
            setupTileIDs(tileIDs);
            shuffleTilesBasedOnState(tileIDs, isExist, ARRAY_SIZE, 0); // Shuffle tiles or elements based on specific conditions
            InitObjects(&LastClicks); // Initialize tile game objects(tiles)
            gameState.remainingTile = ARRAY_SIZE; // Set the initial number of remaining tiles to 144
            gameState.startTime = GetTime(); // Record the start time of the game
            gameState.gameScreen = game; // Switch the game state to the main gameplay state
        }
        break;
    case game:
        drawGame(); // Perform game drawing operations
        drawCombo(); // Draw the combo indicator
        updateGame(&gameState); // Update game state and handle input
        break;
    case gameOver:
        DrawTexture(backGroundTexture[2], 0, 0, RAYWHITE); // Draw the game over screen background
        endScreen(); // Display high scores, menu options, and save button
        break;
    case win:
        DrawTexture(backGroundTexture[3], 0, 0, RAYWHITE); // Draw the win screen background
        endScreen(); // Similar to gameOver, display high scores, menu, and save options
        break;
    }
    framesCounter++; // Increment the frames counter for each frame rendered
    EndDrawing(); // End the drawing phase
}


void updateGame(GameState* gameState) {
    mousePosition = GetMousePosition();
    updateTime(gameState); // Counts down time from 5 minutes in minutes and seconds format

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        processClick(&hint, &LastClicks, mousePosition, &head); // Process a click event
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 25, 150, 50 }, "Shuffle (TAB)") || IsKeyPressed(KEY_TAB)) {
        deletePointsforShuffle(gameState); // Deletes points as a penalty for shuffling
        resetHint(&hint); // Resets hint data
        resetLastClicks(&LastClicks); // Resets the last click information
        PlaySound(shuffleSound); // Plays the shuffle sound
        shuffleTilesBasedOnState(tileIDs, isExist, ARRAY_SIZE, 1); // Shuffle the IDs of tiles isExist == 1, to randomize
        AssignNewValuesToTiles(isExist, tileIDs); // Update the tile IDs and textures on the board to reflect the new shuffled order.
        countMatchableTiles(gameState); // Recounts matchable tiles after shuffling
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 100, 150, 50 }, "Hint (SPACE)") || IsKeyPressed(KEY_SPACE)) {
        //deletePointsforHint(gameState); // Points penalty for hint, commented out perhaps for testing
        PlaySound(gameButtonSound); // Plays a generic game button sound
        countMatchableTiles(gameState); // Ensures matchable tiles are counted before hint is given
        resetHint(&hint); // Resets hint data
        resetLastClicks(&LastClicks); // Resets the last clicks data
        giveHint(&hint, &LastClicks, clickableTilesPerType); // Provides a hint to the player
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 175, 150, 50 }, "Undo (CTRL)") || IsKeyPressed(KEY_LEFT_CONTROL)) {
        PlaySound(gameButtonSound); // Plays a generic game button sound
        deleteBegin(&head, isExist, &LastClicks); // Undoes the last move
        countMatchableTiles(gameState); // Updates matchable tiles after undoing
        resetLastClicks(&LastClicks); // Resets the last clicks data
        gameState->combo = 1; // Resets combo multiplier
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 250, 150, 50 }, "Main")) {
        PlaySound(buttonSound); // Plays a sound for returning to the main menu
        resetGame(); // Resets the game and pointer values
        gameState->gameScreen = starting; // Changes the game screen to starting menu
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 325, 150, 50 }, "End (X)", 40) || IsKeyPressed(KEY_X)) {
        PlaySound(buttonSound); // Plays a button sound for game ending
        gameState->gameScreen = gameOver; // Sets the game screen to game over
    }

    gameState->gameTime = GetTime() - gameState->startTime; // Updates the game time
    gameState->matchable = countMatchableTiles(gameState); // Updates the number of matchable tiles available
    isGameOver(gameState); // Checks if the game is over
    updateCombo(gameState); // Updates the combo multiplier based on actions
}


void processClick(LastTwoClicked* hint, LastTwoClicked* LastClicks, Vector2 mousePosition, node** head) {
    tile* pointer = NULL;
    resetHint(hint); // Resets the hint information

    pointer = getTopMostTile(tiles, mousePosition); // Points to the tile clicked by the cursor

    if (pointer != NULL) {
        if (pointer->isClickable == false)
            shakeTime = 10; // Triggers a shake effect if the tile is not clickable
    }

    // If the tile is clickable and is not the same as the last clicked tile, update LastClicked
    if (isClickable(pointer) && LastClicks->lastClicked != pointer) {
        PlaySound(selectSound); // Plays a sound because a clickable tile was clicked

        if (LastClicks->previousClicked != NULL) { // Prevents crash by not trying to modify a NULL address
            LastClicks->previousClicked->color = RAYWHITE; // Changes the color of the previously clicked tile to white
        }
        if (LastClicks->lastClicked != NULL) {
            LastClicks->lastClicked->color = RAYWHITE; // Changes the color of the last clicked tile to white (now the third last)
        }

        LastClicks->previousClicked = LastClicks->lastClicked; // Updates the previously clicked to the last clicked
        LastClicks->lastClicked = pointer; // Updates the last clicked tile
        if (LastClicks->lastClicked != NULL) {
            LastClicks->lastClicked->color = RED; // Sets the color of the last clicked tile to red
        }

        if (isRemovable(*LastClicks) == true) { // Checks if the last two clicked tiles are removable
            addBegin(head, LastClicks, isExist); // Adds to linked list (removes from table)
            addPoints(*LastClicks, &gameState); // Adds points to the game state
            resetLastClicks(LastClicks); // Resets the last clicks information
            countMatchableTiles(&gameState); // Recalculates matchable tiles
            gameState.remainingTile -= 2; // Subtract 2 from the remaining tiles counter
            if (gameState.matchable == 0) {
                shakeTime = 10; // Triggers a shake effect if no tiles are matchable
            }
        }
    }

    return;
}


void addBegin(node** head, LastTwoClicked* LastClicks, int* isExist) {
    node* newNode = (node*)malloc(sizeof(node)); // Allocate memory for a new node
    if (newNode == NULL) { // Check if memory allocation failed
        printf("Memory allocation error in add_node_begin()\n"); // Print error message if allocation fails
        return;
    }

    LastClicks->lastClicked->isExists = false; // Mark the last clicked tile as non-existent
    LastClicks->previousClicked->isExists = false; // Mark the previously clicked tile as non-existent

    newNode->data1 = LastClicks->lastClicked; // Store the pointer to the last clicked tile
    newNode->data2 = LastClicks->previousClicked; // Store the pointer to the previously clicked tile

    newNode->data1->combo = gameState.combo; // Save the current combo value to the last clicked tile
    newNode->data2->combo = gameState.combo; // Save the current combo value to the previously clicked tile

    isExist[newNode->data1->order] = 0; // Update the existence array for the last clicked tile to 0
    isExist[newNode->data2->order] = 0; // Update the existence array for the previously clicked tile to 0

    newNode->nextNode = *head; // Link the new node to the beginning of the list

    *head = newNode; // Update the head of the list to point to the new node

    return;
}


int giveHint(LastTwoClicked* hint, LastTwoClicked* LastClicks, int* clickableTilesPerType) {
    int hintedId = -1; // Initialize the variable to store the ID of the hinted tile, set to -1 as default (no hint)

    // Search through all possible images to find one with at least two clickable instances
    for (int i = 0; i < NUM_IMAGES; i++) {
        if (clickableTilesPerType[i] >= 2) { // If there are at least two tiles of the same type that can be clicked
            hintedId = i; // Store the ID of this image as the hint
        }
    }

    // If there are no tiles that can be matched (game logic detects no matchable tiles)
    if (gameState.matchable == 0) {
        shakeTime = 10; // Trigger a shake effect to indicate no possible moves
    }

    // Iterate over all layers and positions in the game array to find tiles matching the hinted ID
    for (int i = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {

                // Check if the tile matches the hinted ID and is clickable
                if (tiles[j][k][i].id == hintedId && tiles[j][k][i].isClickable) {
                    if (hint->lastClicked == NULL) { // If the last clicked hint slot is empty
                        hint->lastClicked = &tiles[j][k][i]; // Assign this tile to the last clicked hint
                    }

                    else if (hint->previousClicked == NULL) { // If the previous clicked hint slot is empty
                        hint->previousClicked = &tiles[j][k][i]; // Assign this tile to the previous clicked hint

                        goto loop_out; // Exit the loops once two hints are found
                    }
                }
            }
        }
    }

    loop_out: // Label used to break out of the nested loops
    
    if (hint->lastClicked != NULL && hint->previousClicked != NULL) { // If both hint slots are filled
        hint->lastClicked->color = BLUE; // Highlight the last clicked hint tile
        hint->previousClicked->color = BLUE; // Highlight the previous clicked hint tile
    }
    return 2; // Return 2 as a hardcoded value, typically indicating successful hint placement
}


void deleteBegin(node** head, int* isExist, LastTwoClicked* LastClicks) {
    if (*head == NULL) { // Check if the linked list is empty
        shakeTime = 10; // Trigger a shake effect if there's nothing to undo
        return;
    }
    node* tempNode = *head; // Temporarily store the head node

    tempNode->data1->isExists = true; // Make the first tile in the node drawable and clickable again
    tempNode->data2->isExists = true; // Make the second tile in the node drawable and clickable again

    gameState.totalPoint -= 2 * tempNode->data1->combo * tempNode->data1->point; // Reverse the points earned from this node

    isExist[tempNode->data1->order] = 1; // Mark the first tile as existing again in the tracking array
    isExist[tempNode->data2->order] = 1; // Mark the second tile as existing again in the tracking array

    tempNode->data1->color = RAYWHITE; // Reset the color of the first tile to white
    tempNode->data2->color = RAYWHITE; // Reset the color of the second tile to white

    gameState.remainingTile += 2; // Increment the remaining tiles count as these tiles are added back to the game

    *head = (*head)->nextNode; // Remove the first node by setting head to the next node

    free(tempNode); // Free the memory allocated to the removed node
}