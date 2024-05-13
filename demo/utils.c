#include "utils.h"

void InitImages() {
    // Load background textures
    backGroundTexture[0] = LoadTexture("../assets/backgrounds/background2.png"); // Load main background image
    backGroundTexture[1] = LoadTexture("../assets/backgrounds/bg-black.png"); // Load game background image
    backGroundTexture[2] = LoadTexture("../assets/backgrounds/gameOver.png"); // Load game over background image
    backGroundTexture[3] = LoadTexture("../assets/backgrounds/victory.png"); // Load victory background image

    // Load symbol textures
    symbolsTexture[0] = LoadTexture("../assets/symbols/2x.png"); // Load 2x combo multiplier symbol
    symbolsTexture[1] = LoadTexture("../assets/symbols/3x.png"); // Load 3x combo multiplier symbol
    symbolsTexture[2] = LoadTexture("../assets/symbols/heart.png"); // Load symbol representing remaining tiles
    symbolsTexture[3] = LoadTexture("../assets/symbols/hourglass.png"); // Load hourglass symbol for time tracking
    symbolsTexture[4] = LoadTexture("../assets/symbols/removable.png"); // Load symbol for removable tiles
    symbolsTexture[5] = LoadTexture("../assets/symbols/trophy.png"); // Load symbol for score display
    symbolsTexture[6] = LoadTexture("../assets/symbols/highScore.png"); // Load symbol for displaying the high score list
    symbolsTexture[7] = LoadTexture("../assets/symbols/newRecord.png"); // Load symbol indicating a new record

    // Set initial combo multiplier
    gameState.combo = 1; // Initialize combo multiplier to 1

    // Load tile textures dynamically based on their index
    for (int i = 0; i < NUM_IMAGES; i++) {
        char filePath[NUM_IMAGES]; // Declare a buffer to hold the file path
        sprintf(filePath, "../assets/tiles/%d.png", i); // Format file path string for each tile texture

        textures[i] = LoadTexture(filePath); // Load each tile texture based on its file path
    }
}

void InitSounds() {
    // Load the main game soundtrack to be used as background music throughout the game.
    gameSound = LoadSound("../assets/sounds/soundtrack.mp3");  
    
    // Load the sound effect for button clicks, providing auditory feedback during user interactions.
    buttonSound = LoadSound("../assets/sounds/PixbuttonSound.wav");  

    // Load the sound effect for selecting tiles or options within the game, enhancing the interactive experience.
    selectSound = LoadSound("../assets/sounds/selectSound.mp3");  

    // Load the sound effect specifically for map selections, used to highlight this action in the game settings or menu.
    mapSelectionSound = LoadSound("../assets/sounds/mapSelectionSound.mp3");
    
    // Load the sound effect for pressing game-related buttons, such as during gameplay or on game-specific menus.
    gameButtonSound = LoadSound("../assets/sounds/gameButtonSound.mp3"); 

    // Load the sound effect for shuffling tiles or elements within the game, used to signify a major change or action.
    shuffleSound = LoadSound("../assets/sounds/shuffleSound.wav");
}

void readFile(GameState* gameState) {
    // Button for selecting the "Easy" difficulty map
    if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2 - 100, 100, 30 }, "Easy")) {
        PlaySound(mapSelectionSound); // Play a sound indicating a map has been selected
        file = fopen("../assets/maps/map2.txt", "r"); // Attempt to open the map file for "Easy" difficulty
        if (file == NULL) { // Check if the file failed to open
            perror("Failed to open file"); // Print an error message to the console
            return;
        }
        gameState->isMapSelected = true; // Set the flag indicating that a map has been selected
    }
    // Button for selecting the "Normal" difficulty map
    else if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2 - 50, 100, 30 }, "Normal")) {
        PlaySound(mapSelectionSound); // Play a sound indicating a map has been selected
        file = fopen("../assets/maps/map3.txt", "r"); // Attempt to open the map file for "Normal" difficulty
        if (file == NULL) { // Check if the file failed to open
            perror("Failed to open file"); // Print an error message to the console
            return;
        }
        gameState->isMapSelected = true; // Set the flag indicating that a map has been selected
    }
    // Button for selecting the "Expert" difficulty map
    else if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2, 100, 30 }, "Expert")) {
        PlaySound(mapSelectionSound); // Play a sound indicating a map has been selected
        file = fopen("../assets/maps/map1.txt", "r"); // Attempt to open the map file for "Expert" difficulty
        if (file == NULL) { // Check if the file failed to open
            perror("Failed to open file"); // Print an error message to the console
            return;
        }
        gameState->isMapSelected = true; // Set the flag indicating that a map has been selected
    }
}
void InitMap() {
    int x, y;
    // Reading the file into a 2D array to set up the game map
    for (y = 0; y < ARRAY_Y; y++) {
        for (x = 0; x < ARRAY_X; x++) {
            // Read each digit from the file; expect a single digit (0-9) per map position
            if (fscanf(file, "%1d", &map[y][x]) == NULL) {
                perror("Failed to read data"); // If reading fails, output an error message
                return; // Exit the function on failure
            }
        }
    }
    fclose(file);  // Close the file after reading to free system resources and avoid data corruption

    // Process the map data to initialize gameplay elements
    for (int k = LAYER - 1; k > 0; k--) { // Start reading from the top layer down to create a layered effect
        for (int i = 0; i < HEIGHT - 1; i++) { // Iterate over the height of the map grid
            for (int j = 0; j < WIDTH - 1; j++) { // Iterate over the width of the map grid
                // Check a 2x2 block in the grid for matching layer numbers
                if ((map[i][j] == k) && (map[i][j + 1] == k) &&
                    (map[i + 1][j] == k) && (map[i + 1][j + 1] == k)) { // If all four corners of a 2x2 block match the layer number

                    newMap[i][j][k]++; // Increment the tile count at this position for the layer
                    // Decrease the layer number of each part of this 2x2 block, effectively "consuming" it to create a tile
                    map[i][j]--;
                    map[i][j + 1]--;
                    map[i + 1][j]--;
                    map[i + 1][j + 1]--;
                }
            }
        }
    }
}

void InitObjects() {
    for (int i = 0, a = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                tiles[j][k][i].y = j; // ileride 2 tasi kiyaslarken kullanilacak // save the x, y, z values of every tile
                tiles[j][k][i].x = k;
                tiles[j][k][i].z = i;

                tiles[j][k][i].isExists = newMap[j][k][i]; // 3 boyutlu arrayde deger 1 ise oraya tas cizecegiz //if the value is 1 in 3D array, draw a tile there

                tiles[j][k][i].rectangle.x = (float)(k * WIDTH / 2 - i * 10); //taslarin x ve y degerleri girildi, i ile carpim 3 boyun goruntusu kazandirmak icin eklendi
                tiles[j][k][i].rectangle.y = (float)(j * HEIGHT / 2 - i * 10 + offset);

                tiles[j][k][i].rectangle.width = (float)WIDTH; //tasin yuksekligi //width of tiles
                tiles[j][k][i].rectangle.height = (float)HEIGHT;// tasin uzunlugu //height of tiles
                if (tiles[j][k][i].isExists == true) {//eger tas var ise //check if there's a tile in that coordinates
                    tiles[j][k][i].color = RAYWHITE;//tasi normal renk tonunda ciz // set the color of tiles to white
                    tiles[j][k][i].id = tileIDs[a]; //taslar karistirildi ve cizilmesi gerekenlere sirayla atanacak //tiles shuffled for placing to table
                    tiles[j][k][i].texture = textures[tileIDs[a]];
                    tiles[j][k][i].order = a;
                    if (tileIDs[a] >= 14 && tileIDs[a] <= 19) {
                        tiles[j][k][i].point = 20;
                    }
                    else {
                        tiles[j][k][i].point = 10;
                    }
                    isExist[a]++;
                    a++;
                }
            }
        }
    }
}

bool isClickable(tile* getTopMostTile) {
    // Check if the tile is not empty and does exist.
    if (getTopMostTile == NULL || getTopMostTile->isExists == false) {
        return false;
    }

    // Coordinates of the tile.
    int x = getTopMostTile->x;
    int y = getTopMostTile->y;
    int z = getTopMostTile->z;

    // Check for obstructing tiles in the immediate vicinity and above.
    if (tiles[y][x + 1][z + 1].isExists) { // Check if the tile half to the right and one layer up exists.
        return false;
    }
    if (tiles[y + 1][x + 1][z + 1].isExists) { // Check if the tile half down and half to the right, one layer up, exists.
        return false;
    }
    if (tiles[y][x][z + 1].isExists) { // Check if the tile directly one layer above exists.
        return false;
    }
    if (tiles[y + 1][x][z + 1].isExists) { // Check if the tile half down, one layer up, exists.
        return false;
    }
    if (tiles[y - 1][x - 1][z + 1].isExists) { // Check if the tile half up and half to the left, one layer up, exists.
        return false;
    }
    if (tiles[y + 1][x - 1][z + 1].isExists) { // Check if the tile half down and half to the left, one layer up, exists.
        return false;
    }
    if (tiles[y - 1][x + 1][z + 1].isExists) { // Check if the tile half up and half to the right, one layer up, exists.
        return false;
    }
    if (tiles[y - 1][x][z + 1].isExists) { // Check if the tile half up, one layer above, exists.
        return false;
    }
    if (tiles[y][x - 1][z + 1].isExists) { // Check if the tile half to the left, one layer up, exists.
        return false;
    }

    // Check for tiles on the right and left that may block side movement.
    bool rightBlocked = tiles[y - 1][x + 2][z].isExists || tiles[y][x + 2][z].isExists || tiles[y + 1][x + 2][z].isExists;
    bool leftBlocked = tiles[y - 1][x - 2][z].isExists || tiles[y][x - 2][z].isExists || tiles[y + 1][x - 2][z].isExists;

    // Determine clickability based on side blockages.
    if (rightBlocked && leftBlocked) {
        return false;
    }

    // If no obstructing conditions are met, the tile is clickable.
    return true;
}

bool isRemovable(LastTwoClicked LastClicks) {
    // Receives the last two clicked tiles as parameters.
    tile* tile1 = LastClicks.lastClicked;
    tile* tile2 = LastClicks.previousClicked;

    // Return false if either of the last two clicked tiles is NULL, to prevent the program from crashing.
    if (tile1 == NULL || tile2 == NULL) {
        return false;
    }
    // If the tiles' identifiers do not match, they cannot be removed together.
    if (tile1->id != tile2->id) {
        return false;
    }
    // Prevent the same tile from being removed if it was clicked twice.
    if ((tile1->x == tile2->x) && (tile1->y == tile2->y) && (tile1->z == tile2->z)) {
        return false;
    }

    return true; // Return true if none of the above conditions are met, indicating the tiles can be removed.
}


tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition) {
    // Iterate from the topmost layer down to the bottom layer.
    for (int i = LAYER - 1; i >= 0; i--) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                // Check if the mouse is over the tile and if the tile currently exists.
                if (CheckCollisionPointRec(mousePosition, tiles[j][k][i].rectangle) && tiles[j][k][i].isExists) {
                    // If the mouse is over an existing tile, return a pointer to that tile.
                    return &tiles[j][k][i];
                }
            }
        }
    }

    // Return NULL if no tile is found at the mouse position in any layer.
    return NULL;
}

void shuffleTilesBasedOnState(int* tileIDs, int* tileStates, int numTiles, int activeState) {
    // Seed the random number generator with the current time to ensure different shuffle patterns each time the function is called.
    srand(time(NULL));

    // Allocate memory dynamically to store indices of tiles whose states match the specified active state.
    int* indices = (int*)malloc(numTiles * sizeof(int));
    if (indices == NULL) {
        return; // Exit if memory cannot be allocated to prevent the function from proceeding with null pointers.
    }

    // Iterate over the tileStates array to find tiles that are in the 'active' state as defined by the game logic.
    int count = 0;
    for (int i = 0; i < numTiles; i++) {
        if (tileStates[i] == activeState) {
            indices[count++] = i; // Store the index of each active tile and increment the counter.
        }
    }

    // Shuffle only the active tiles. This shuffling algorithm ensures that each element has an equal chance of being in any position.
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Generate a random index less than or equal to i.
        // Swap the elements at the current index (i) and the randomly chosen index (j) to shuffle the tile IDs.
        int temp = tileIDs[indices[i]];
        tileIDs[indices[i]] = tileIDs[indices[j]];
        tileIDs[indices[j]] = temp;
    }

    // Clean up by freeing the dynamically allocated memory to avoid memory leaks.
    free(indices);
    indices = NULL; // Set the pointer to NULL to prevent use after free, enhancing security and stability.
}

void AssignNewValuesToTiles(int* tileActivityStates, int* tileOrder) {
    // Iterate through each layer of the game's tile grid
    for (int i = 0, tileIndex = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++, tileIndex++) {
                // Check if the current tile is marked as existing and active
                if (tiles[j][k][i].isExists == true && tileActivityStates[tiles[j][k][i].order] == 1) {

                    // Assign a new ID from the shuffled tile order to the active tile
                    tiles[j][k][i].id = tileOrder[tiles[j][k][i].order];

                    // Update the tile's texture based on the new ID
                    tiles[j][k][i].texture = textures[tiles[j][k][i].id];
                }
            }
        }
    }
}

void resetHint(LastTwoClicked* hint) {// Reset hint colors to white and clear the hint selection if any hints are currently displayed.
    if (hint->lastClicked != NULL) {
        hint->lastClicked->color = RAYWHITE; // Set the color of the last clicked hint tile back to white.
    }
    if (hint->previousClicked != NULL) {
        hint->previousClicked->color = RAYWHITE; // Set the color of the previously clicked hint tile back to white.
    }

    // Clear the hint pointers to indicate that there are no current hints.
    if (hint->lastClicked != NULL) {
        hint->lastClicked = NULL; // Remove the reference to the last clicked hint tile.
    }
    if (hint->previousClicked != NULL) {
        hint->previousClicked = NULL; // Remove the reference to the previously clicked hint tile.
    }
}


void setupTileIDs(int* tileIDs) {
    // Fill the array with numbers where each number repeats four times. This setup is for 36 types of tiles, making a total of 144 tiles.
    for (int i = 0; i < ARRAY_SIZE; i++) {
        tileIDs[i] = i / 4;  // Every four slots get the same number, matching the tile types.
    }
}

void resetLastClicks(LastTwoClicked* LastClicks) {
    // If there are tiles selected, change their color back to white and clear the selections.
    if (LastClicks->lastClicked != NULL) {
        LastClicks->lastClicked->color = RAYWHITE; // Change the color of the last clicked tile to white.
    }
    if (LastClicks->previousClicked != NULL) {
        LastClicks->previousClicked->color = RAYWHITE; // Change the color of the previously clicked tile to white.
    }

    // Clear the pointers to the last and previous clicked tiles to reset the selection.
    if (LastClicks->lastClicked != NULL) {
        LastClicks->lastClicked = NULL; // Clear the reference to the last clicked tile.
    }
    if (LastClicks->previousClicked != NULL) {
        LastClicks->previousClicked = NULL; // Clear the reference to the previously clicked tile.
    }
}

void unloadGameSounds() {
    UnloadSound(gameSound);// Remove the game background music from memory to free up resources.
    
    UnloadSound(buttonSound);// Remove the sound effect used for button clicks to free up resources.

    UnloadSound(selectSound); // Remove the sound effect used for selecting items to free up resources.
}

void addPoints(LastTwoClicked LastClicks, GameState* gameState) {
                                    
    int combo = gameState->combo; // Get the current combo level from the game.
    
    int point = LastClicks.lastClicked->point; // Get the number of points from the last clicked tile.

    if (gameState->combo < 3) { // If the combo level is below 3, increase it by 1.
        gameState->combo++;
    }

    gameState->totalPoint += 2 * combo * point; // Add more points to the total score, multiplying the tile’s points by the combo and 2.

    gameState->lastMatchTime = gameState->gameTime; // Record the time when this match was made.

    return;
}


void deletePointsforHint(GameState* gameState) { // Subtract 20 pts from totalPoint, every time a hint is called.
    gameState->totalPoint -= 20;
}

void deletePointsforShuffle(GameState* gameState) { // Subtract 50 pts from totalPoint, every time shuffle is called.
    gameState->totalPoint -= 50;
}

int countMatchableTiles(GameState* gameState) {
    // Reset the frequency counter for each type of tile.
    for (int i = 0; i < NUM_IMAGES; i++) {
        clickableTilesPerType[i] = 0;
    }

    int count = 0; // Initialize a counter to track the number of matchable tile types.

    // Traverse through each layer and tile on the board.
    for (int i = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) { // Determine if each tile is clickable and update its status.
                tiles[j][k][i].isClickable = isClickable(&tiles[j][k][i]);

                if (tiles[j][k][i].isClickable) { // If the tile is clickable, increment its type's frequency in the clickableTilesPerType array.
                    clickableTilesPerType[tiles[j][k][i].id]++;
                }
            }
        }
    }

    for (int i = 0; i < NUM_IMAGES; i++) { // Count how many tile types have two or more clickable instances.
        if (clickableTilesPerType[i] >= 2) {
            count++;
        }
    }

    gameState->matchable = count; // Update the game state with the number of matchable tile types.

    return count; // Return the total number of matchable tile types.
}

void updateTime(GameState* gameState) {
    const int countdown_minute = 5; // Set the total countdown time in minutes.

    // Calculate the remaining minutes by subtracting the elapsed minutes from the countdown total.
    int minutes = countdown_minute - (int)(gameState->gameTime / 60) - 1;
    // Calculate the remaining seconds by taking the remainder of the elapsed time divided by 60.
    int seconds = 60 - (int)gameState->gameTime % 60;

    // Correct the seconds and minutes calculation if seconds reach 60.
    if (seconds == 60) {
        seconds = 0; // Reset seconds to zero.
        minutes++; // Increment the minutes since a full minute has passed.
    }

    // Update the game state with the new minutes and seconds.
    gameState->minutes = minutes;
    gameState->seconds = seconds;
}

void isGameOver(GameState* gameState) {
    int minutes = gameState->minutes; // Get the remaining minutes from game state.
    int seconds = gameState->seconds; // Get the remaining seconds from game state.

    // Check if all tiles have been matched or used, which means the player wins.
    if (gameState->remainingTile == 0) {
        gameState->isMapSelected = false; // Indicate that no map is currently selected.
        gameState->gameScreen = win; // Change the game screen to the win screen.
    }
    // Check if the total time has exceeded 5 minutes (300 seconds), indicating game over due to timeout.
    else if (minutes * 60 + seconds > 300) {
        gameState->isMapSelected = false; // Indicate that no map is currently selected.
        gameState->isGameActive = false; // Indicate that the game is no longer active.
        gameState->gameScreen = gameOver; // Change the game screen to the game over screen.
    }
}


void drawGame() {
    // Draw the game background.
    DrawTexture(backGroundTexture[1], 0, 0, WHITE);

    // Draw various symbols on the game screen.
    DrawTexture(symbolsTexture[2], screenWidth / 2 + 495, 320, WHITE); // Draw the symbol for remaining tiles.
    DrawTexture(symbolsTexture[3], screenWidth / 2 + 495, 20, WHITE); // Draw the hourglass symbol for remaining time.
    DrawTexture(symbolsTexture[4], screenWidth / 2 + 500, 220, WHITE); // Draw the symbol for removable tiles.
    DrawTexture(symbolsTexture[5], screenWidth / 2 + 500, 120, WHITE); // Draw the symbol for current points.

    // Display dynamic game information such as remaining tiles, time, matchable tiles, and current points.
    DrawText(TextFormat("x %d", gameState.remainingTile), screenWidth / 2 + 565, 340, 40, RAYWHITE); // Remaining tiles count.
    DrawText(TextFormat("%d:%.2d", gameState.minutes, gameState.seconds), screenWidth / 2 + 495 + 70, 40, 40, RAYWHITE); // Remaining game time.
    DrawText(TextFormat("x %d", gameState.matchable), screenWidth / 2 + 500 + 70, 240, 40, RAYWHITE); // Number of matchable tiles.
    DrawText(TextFormat("%d", gameState.totalPoint), screenWidth / 2 + 570, 135, 40, RAYWHITE); // Current score.

    // Begin drawing in 2D mode with the camera settings.
    BeginMode2D(camera);

    // Loop through each layer and tile to draw the existing tiles with their respective textures and colors.
    for (int k = 1; k < LAYER; k++) {
        for (int i = 0; i < ARRAY_Y; i++) {
            for (int j = 0; j < ARRAY_X; j++) {
                if (tiles[i][j][k].isExists) {
                    DrawTexture(tiles[i][j][k].texture, tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, tiles[i][j][k].color);
                }
            }
        }
    }

    // End the 2D mode after drawing all elements.
    EndMode2D();
}

void resetGame() {
    // Reset all values in newMap to zero, clearing any previous game state.
    for (int y = 0; y < ARRAY_Y; y++) {
        for (int x = 0; x < ARRAY_X; x++) {
            for (int l = 0; l < LAYER; l++) {
                newMap[y][x][l] = 0;
            }
        }
    }

    // Clear the main map array to prepare for a new game.
    for (int y = 0; y < ARRAY_Y; y++) {
        for (int x = 0; x < ARRAY_X; x++) {
            map[y][x] = 0;
        }
    }

    // Reset tile existence flags to indicate no tiles are initially present.
    for (int i = 0; i < ARRAY_SIZE; i++) {
        isExist[i] = 0;
    }

    // Clear counts of clickable tiles per type, resetting game logic related to tile interaction.
    for (int i = 0; i < NUM_IMAGES; i++) {
        clickableTilesPerType[i] = 0;
    }

    // Reinitialize tile IDs to their default setup, ensuring consistent starting conditions.
    setupTileIDs(tileIDs);

    // Reset hints to clear any selections and visual indicators from previous games.
    resetHint(&hint);

    // Clear last clicks to prevent unintended interactions from lingering selections.
    resetLastClicks(&LastClicks);

    // Reset various game state parameters to default values.
    gameState.combo = 1;             // Reset combo multiplier.
    gameState.gameTime = 0;          // Reset the game clock.
    gameState.totalPoint = 0;        // Reset the score to zero.
    gameState.lastMatchTime = 0;     // Clear the timestamp of the last match.
    gameState.remainingTile = ARRAY_SIZE;  // Reset the count of remaining tiles.
    gameState.isMapSelected = false; // Indicate no map is currently selected.
    saveGuiVisible = true;           // Ensure the save GUI is visible if needed.

    // Clear any dynamically allocated memory for the game's linked list and reset the head pointer.
    free(head);
    head = NULL;

    // Reset the text buffer used for input or display.
    strcpy(text, "\0");
}


void savingText() {
    // Check if the Save button is clicked or Enter key is pressed.
    if (GuiButton((Rectangle) { 790, 400, 100, 30 }, "Save") || IsKeyPressed(KEY_ENTER)) {
        saveGuiVisible = false; // Hide the save GUI elements after the button is pressed or Enter is hit.

        // Sort the high scores and write the new entry to the high scores file.
        sort_and_write_scores("../assets/highscores.txt", text, gameState.totalPoint);
        // Output the saved text to the console for confirmation.
        printf("%s\n", text);
    }

    // Display instructions for entering a name above the text box.
    DrawText("Enter a name: ", screenWidth / 2 - 240, 354, 25, RAYWHITE);
    // Create a text box for player input, enabling the player to type their name.
    GuiTextBox((Rectangle) { screenWidth / 2 - 50, 350, 220, 30 }, text, 256, true);

    // Check if the save GUI is not visible and the highscores file could not be opened.
    if (!saveGuiVisible && fopen("../assets/highscores.txt", "r") == NULL) {
        // Display an error message if the high scores file fails to open.
        DrawText("Failed to save!", screenWidth / 2 - 50, 340, 20, RED);
    }
}


void sort_and_write_scores(const char* filename, const char* text, int points) {
    int count = 0;
    ScoreEntry entries[MAX_LINES];  // Assuming this is defined somewhere globally or passed in.

    // Open the file to read existing scores.
    FILE* file = fopen(filename, "r+");
    if (file != NULL) {
        // Read existing entries from the file.
        while (fscanf(file, "%s %d\n", entries[count].text, &entries[count].points) == 2) {
            if (++count >= MAX_LINES) break;
        }
    }
    else {
        // If the file doesn't exist, open it for writing (this creates the file).
        file = fopen(filename, "w");
        if (file == NULL) return; // If the file still can't be opened, exit the function.
    }

    // Add the new score to the list of entries.
    strcpy(entries[count].text, text);
    entries[count].points = points;
    count++;

    // Sort the scores.
    qsort(entries, count, sizeof(ScoreEntry), compare_scores);

    // Rewind the file to start writing from the beginning.
    rewind(file);

    // Write the sorted scores back to the file.
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d\n", entries[i].text, entries[i].points);
    }

    // Close the file.
    fclose(file);
}


int compare_scores(const void* a, const void* b) {
    // Cast the void pointers to ScoreEntry pointers for accessing the points.
    ScoreEntry* entryA = (ScoreEntry*)a;
    ScoreEntry* entryB = (ScoreEntry*)b;

    // Compare the points for sorting in descending order. If entryB's points are
    // greater than entryA's, a positive number is returned, sorting entryB before entryA.
    return (entryB->points - entryA->points);
}


void print10() {
    FILE* file3 = fopen("../assets/highscores.txt", "r");
    if (file3 == NULL) {
        perror("Failed to open file");
        return; // Exit if file opening fails
    }

    char line[50];
    int i = 0;
    while (fgets(line, sizeof(line), file3) && i < 10) {
        // Attempt to parse the string and the integer from the line
        if (sscanf(line, "%50[^\n0123456789] %d", array[i], &point[i]) != 2) {
            printf("Error reading line %d\n", i + 1);
            break; // Break if parsing fails
        }
        i++;
    }

    fclose(file3); // Close the file after reading

    for (int i = 0; i < 10; i++) {
        DrawText(TextFormat("%d %s %d", i+1, array[i], point[i]), 60, 160 + i * 70, 35, RAYWHITE); // Adjust position and size as needed
    }
}

void updateCombo(GameState* gameState) {
    // Check the time elapsed since the last match; reset combo if it exceeds 6 seconds.
    if (gameState->gameTime - gameState->lastMatchTime > 6.0) {
        gameState->combo = 1; // Reset the combo multiplier to 1 when more than 6 seconds pass without a match.
    }
}

void drawCombo() {
    // Calculate the width of the combo timer bar, which decreases over time from the last match.
    int width = 150 * (6 - (gameState.gameTime - gameState.lastMatchTime));

    if (gameState.combo == 2) {
        // Draw the remaining time bar for the combo with a specific color.
        DrawRectangle(155, 20, width, 60, (Color) { 235, 227, 197, 255 }); // Light beige color bar
        
        // Display the 2x combo symbol.
        DrawTexture(symbolsTexture[0], 100, 10, RAYWHITE);
    }
    else if (gameState.combo == 3) {
        // Draw the remaining time bar for the combo with a different color.
        DrawRectangle(155, 20, width, 60, (Color) { 213, 194, 105, 255 }); // Golden color bar
        
        // Display the 3x combo symbol.
        DrawTexture(symbolsTexture[1], 100, 10, RAYWHITE);
    }
}

void endScreen() {
    // Display the high score symbol on the screen.
    DrawTexture(symbolsTexture[6], 30, 30, RAYWHITE);

    // Stop the background game music as the game ends.
    StopSound(gameSound);

    // Check if the 'Main' button is clicked to return to the main game screen.
    if (GuiButton((Rectangle) { 670, 400, 100, 30 }, "Main")) {
        resetGame(); // Reset the game to initial state for a new session.
        PlaySound(buttonSound); // Play a sound to confirm button press.
        gameState.gameScreen = starting; // Change the screen to the starting menu.
    }

    // Show the saving text screen if the player's score is high enough to enter the top scores.
    if (saveGuiVisible == true && gameState.totalPoint >= point[9]) {
        savingText(); // Activate the save screen for the player to enter their name.
    }

    // Display the top 10 scores.
    print10();

    // If the player's score qualifies as a new record, display the new record symbol.
    if (gameState.totalPoint >= point[9]) {
        DrawTexture(symbolsTexture[7], 1130, 615, RAYWHITE);
    }
}

void InitCamera(Camera2D* camera) {
    // Set the camera's target to the center of the screen.
    camera->target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    // Set the camera's offset to center the view in the middle of the screen.
    camera->offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    // Initialize the camera's rotation to zero; no rotation is applied.
    camera->rotation = 0.0f;
    // Set the camera's zoom level to 1.0, implying no zoom.
    camera->zoom = 1.0f;
}


void checkCameraForShake(Camera2D* camera) {
    if (shakeTime > 0) {
        // Apply a shaking effect to the camera by randomly altering its offset.
        camera->offset.x = screenWidth / 2.0f + GetRandomValue(-shakeMagnitude, shakeMagnitude);
        camera->offset.y = screenHeight / 2.0f + GetRandomValue(-shakeMagnitude, shakeMagnitude);
        shakeTime--; // Decrement the shake time with each frame.
    }
    else {
        // Once the shaking time has elapsed, reset the camera offset to the center.
        camera->offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    }
}