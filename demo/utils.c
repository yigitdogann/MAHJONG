#include "utils.h"

void InitImagesSounds() {
    backGround[1] = LoadImage("../assets/bg-black.png"); //arkaplan ekleme 
    backGroundTexture[1] = LoadTextureFromImage(backGround[1]);
    UnloadImage(backGround[1]);

    backGround[0] = LoadImage("../assets/background2.png"); //arkaplan ekleme 
    backGroundTexture[0] = LoadTextureFromImage(backGround[0]);
    UnloadImage(backGround[0]);
    
    backGround[2] = LoadImage("../assets/gameOver.png"); //arkaplan ekleme 
    backGroundTexture[2] = LoadTextureFromImage(backGround[2]);
    UnloadImage(backGround[2]);

    backGround[3] = LoadImage("../assets/victory.png"); //arkaplan ekleme 
    backGroundTexture[3] = LoadTextureFromImage(backGround[3]);
    UnloadImage(backGround[3]);

    //
    symbols[0] = LoadImage("../assets/2x.png"); //arkaplan ekleme 
    symbolsTexture[0] = LoadTextureFromImage(symbols[0]);
    UnloadImage(symbols[0]);

    symbols[1] = LoadImage("../assets/3x.png"); //arkaplan ekleme 
    symbolsTexture[1] = LoadTextureFromImage(symbols[1]);
    UnloadImage(symbols[1]);

    symbols[2] = LoadImage("../assets/heart.png"); //arkaplan ekleme 
    symbolsTexture[2] = LoadTextureFromImage(symbols[2]);
    UnloadImage(symbols[2]);

    symbols[3] = LoadImage("../assets/hourglass.png"); //arkaplan ekleme 
    symbolsTexture[3] = LoadTextureFromImage(symbols[3]);
    UnloadImage(symbols[3]);
    
    symbols[4] = LoadImage("../assets/removable.png"); //arkaplan ekleme 
    symbolsTexture[4] = LoadTextureFromImage(symbols[4]);
    UnloadImage(symbols[4]);

    symbols[5] = LoadImage("../assets/trophy.png"); //arkaplan ekleme 
    symbolsTexture[5] = LoadTextureFromImage(symbols[5]);
    UnloadImage(symbols[5]);

    //gameSound = LoadSound("../assets/relaxing.mp3");
    buttonSound = LoadSound("../assets/PixbuttonSound.wav");
    selectSound = LoadSound("../assets/selectSound.mp3");
    mapSelectionSound = LoadSound("../assets/mapSelectionSound.mp3");
    gameButtonSound = LoadSound("../assets/gameButtonSound.mp3");
    (&gameState)->combo = 1;
    //UnloadSound(gameSound);     // Unload sound data
    for (int i = 0; i < NUM_IMAGES; i++) {
        char filePath[NUM_IMAGES]; // Buffer to hold the file path
        sprintf(filePath, "../assets/tiles/%d.png", i); // Create file path, note images are 1-indexed in your folder

        images[i] = LoadImage(filePath); // Load image data into CPU memory (RAM)
        textures[i] = LoadTextureFromImage(images[i]); // Image converted to texture, GPU memory (RAM -> VRAM)
        UnloadImage(images[i]); // Unload image data from CPU memory (RAM)
    }
}
void readFile(GameState* gameState) {
    if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2 - 100, 100, 30 }, "EASY")) {
        PlaySound(mapSelectionSound);
        file = fopen("../assets/map2.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isMapSelected = true;
    }
    else if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2 - 50, 100, 30 }, "NORMAL")) {
        PlaySound(mapSelectionSound);
        file = fopen("../assets/map3.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isMapSelected = true;
    }
    else if (GuiButton((Rectangle) { screenWidth / 2 - 45, screenHeight / 2, 100, 30 }, "EXPERT")) {
        PlaySound(mapSelectionSound);
        file = fopen("../assets/map1.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isMapSelected = true;
    }
}
void InitMap() {
    int x, y;
    // Reading the file into a 2D array
    for (y = 0; y < ARRAY_Y; y++) {
        for (x = 0; x < ARRAY_X; x++) {
            if (fscanf(file, "%1d", &map[y][x]) == NULL) {
                perror("Failed to read data");
                return;
            }
        }
    }
    fclose(file);  // Always close the file after you're done reading
     //free(file);
    for (int k = LAYER - 1; k > 0; k--) { //mapi okumak icin once en ust kati oku
        for (int i = 0; i < HEIGHT - 1; i++) { //arrayin dikey degerlerini oku
            for (int j = 0; j < WIDTH - 1; j++) { //arrayin yatay degerlerini oku 
                if ((map[i][j] == k) && (map[i][j + 1] == k) && (map[i + 1][j] == k) && (map[i + 1][j + 1] == k)) { //her 2x2 birim ayni degerde ise = 1 tas
                    newMap[i][j][k]++;
                    map[i][j]--; //sonra 1 azalt okdudugun degerleri alt kata gec 
                    map[i][j + 1]--;
                    map[i + 1][j]--;
                    map[i + 1][j + 1]--;
                }
            }
        }
    }
}
void InitObjects(LastTwoClicked* LastClicks) {
    LastClicks->lastClicked = NULL;
    LastClicks->previousClicked = NULL;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        isExist[i] = 0;
    }
    randomFiller(original);
    shuffleBasedOnCondition(original, isExist, ARRAY_SIZE, 0);

    for (int i = 0, a = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                tiles[j][k][i].y = j; // ileride 2 tasi kiyaslarken kullanilacak 
                tiles[j][k][i].x = k;
                tiles[j][k][i].z = i;

                tiles[j][k][i].isExists = newMap[j][k][i]; // 3 boyutlu arrayde deger 1 ise oraya tas cizecegiz

                tiles[j][k][i].rectangle.x = (float)(k * WIDTH / 2 - i * 10); //taslarin x ve y degerleri girildi, i ile carpim 3 boyun goruntusu kazandirmak icin eklendi
                tiles[j][k][i].rectangle.y = (float)(j * HEIGHT / 2 - i * 10 + offset);

                tiles[j][k][i].rectangle.width = (float)WIDTH; //tasin yuksekligi
                tiles[j][k][i].rectangle.height = (float)HEIGHT;// tasin uzunlugu
                if (tiles[j][k][i].isExists == true) {// eger tas var ise
                    tiles[j][k][i].color = RAYWHITE;//tasi normal renk tonunda ciz, (farkli renk koyup degisiklikler gozlenebilir)
                    tiles[j][k][i].id = original[a]; //taslari karistirildi ve cizilmesi gerekenlere sirayla atanacak
                    tiles[j][k][i].texture = textures[original[a]];
                    tiles[j][k][i].order = a;
                    if (original[a] >= 14 && original[a] <= 19) {
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
    if (getTopMostTile == NULL) {
        return false;
    }
    if (getTopMostTile->isExists != true) {
        return false;
    }
    int x = getTopMostTile->x;
    int y = getTopMostTile->y;
    int z = getTopMostTile->z;

    if (tiles[y][x + 1][z + 1].isExists == true) {//tasin yarim sagi ve dolu mu? doluysa tiklanilamaz
        return false;
    }
    if (tiles[y + 1][x + 1][z + 1].isExists == true) {//tasin yarim sag ustu dolu mu? doluysa tiklanilamaz
        return false;
    }
    if (tiles[y][x][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y + 1][x][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y - 1][x - 1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y + 1][x - 1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y - 1][x + 1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y - 1][x][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y][x - 1][z + 1].isExists == true) {
        return false;
    }
    bool right = tiles[y - 1][x + 2][z].isExists || tiles[y][x + 2][z].isExists || tiles[y + 1][x + 2][z].isExists;
    //üst kat

    bool left = tiles[y - 1][x - 2][z].isExists || tiles[y][x - 2][z].isExists || tiles[y + 1][x - 2][z].isExists;
    //alt kat

    if (right && left) {
        return false;
    }
    return true;
}

bool isRemovable(LastTwoClicked LastClicks) {//son tiklanilan 2 tasi parametre olarak alir
    tile* tile1 = LastClicks.lastClicked;
    tile* tile2 = LastClicks.previousClicked;
    
    if (tile1 == NULL || tile2 == NULL) {//son tiklanilan 2 tastan bir tanesi NULL ise false doner, program crash olmamasi icin 
        return false;
    }
    if (tile1->id != tile2->id) {//eger taslarin simgeleri ayni degilse bunlar masadan kaldirilamaz
        return false;
    }
    //eger ayni tasa 2 kere cift tiklarsan masadan kalkmamasi icin 
    if ((tile1->x == tile2->x) && (tile1->y == tile2->y) && (tile1->z == tile2->z)) {
        return false;
    }

    tile1 = NULL;
    tile2 = NULL;

    return true;
    
}

void shuffleBasedOnCondition(int* a, int* b, int size, int x) {
    srand(time(NULL));  // Seed the random number generator

    // Dynamically allocate memory to store indices where b[i] == 1
    int* indices = (int*)malloc(size * sizeof(int));
    if (indices == NULL) {
        return; // Return if memory allocation failed
    }

    // Collect indices where b[i] == 1
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (b[i] == x) {
            indices[count++] = i;
        }
    }

    // Shuffle elements in a according to collected indices
    // Only shuffle elements where b[i] == 1
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Generate a random index
        // Swap the elements at indices[i] and indices[j]
        int temp = a[indices[i]];
        a[indices[i]] = a[indices[j]];
        a[indices[j]] = temp;
    }

    // Free the dynamically allocated memory
    free(indices); //memory ile işimiz bitti deallocate ediyoruz
    indices = NULL; //hata ile az karşılaşmak için
}

void shuffle_all(LastTwoClicked* hint, int* isExist, int* original) {
    if (hint->lastClicked != NULL && hint->previousClicked != NULL) {
        hint->lastClicked->color = RAYWHITE;
        hint->previousClicked->color = RAYWHITE;

        hint->lastClicked = NULL;
        hint->previousClicked = NULL;
    }
    shuffleBasedOnCondition(original, isExist, ARRAY_SIZE, 1);
    for (int i = 0, a = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++, a++) {
                if (tiles[j][k][i].isExists == true && isExist[tiles[j][k][i].order] == 1) {
                    tiles[j][k][i].id = original[tiles[j][k][i].order];//taslari karilmis desteden sec
                    tiles[j][k][i].texture = textures[tiles[j][k][i].id];//desteden secilen sayiya gore tasa sekil atamasi yap
                }
            }
        }
    }

    if (hint->lastClicked != NULL && hint->previousClicked != NULL) { //karıştırma komutuna basarsan mavi ile işaretlenen taşları normal haline getirir
        hint->lastClicked->color = RAYWHITE;
        hint->previousClicked->color = RAYWHITE;
    }
}

void resetHint(LastTwoClicked* hint) {
    if (hint->lastClicked != NULL) {    
        hint->lastClicked->color = RAYWHITE;
    }
    if (hint->previousClicked != NULL) {
        hint->previousClicked->color = RAYWHITE;
    }

    if (hint->lastClicked != NULL) {
        hint->lastClicked = NULL;
    }
    if (hint->previousClicked != NULL) {
        hint->previousClicked = NULL;
    }
}

void randomFiller(int* original) {
    // Diziyi doldur
    for (int i = 0; i < ARRAY_SIZE; i++) {
        original[i] = i / 4;  // Her 4 eleman aynı değeri alacak
    }
}

void resetLastClicks(LastTwoClicked* LastClicks) {
    if (LastClicks->lastClicked != NULL) {
        LastClicks->lastClicked->color = RAYWHITE;
        LastClicks->lastClicked = NULL;
    }
    if (LastClicks->previousClicked != NULL) {
        LastClicks->previousClicked->color = RAYWHITE;
        LastClicks->previousClicked = NULL;
    }
}

void unloadGameSounds() {
    UnloadSound(gameSound);
    UnloadSound(buttonSound);
    UnloadSound(selectSound);
}

void addPoints(LastTwoClicked LastClicks, GameState* gameState) {
    int combo = gameState->combo;
    int point = LastClicks.lastClicked->point;
    if (gameState->combo < 3) {
        gameState->combo++;
    }

    gameState->totalPoint += 2 * combo * point;
    gameState->lastMatchTime = gameState->gameTime;

    return;
}

void deletePointsforHint(GameState* gameState) {
    gameState->totalPoint -= 20;
}

void deletePointsforShuffle(GameState* gameState) {
    gameState->totalPoint -= 50;
}

int countMatchableTiles(GameState* gameState) {
    for (int i = 0; i < NUM_IMAGES; i++){
        clickable_freq[i] = 0;
    }

    int count = 0;
    for (int i = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                tiles[j][k][i].isClickable = isClickable(&tiles[j][k][i]);
                //printf(" %d ", tiles[j][k][i].id);
                if (tiles[j][k][i].isClickable == 1) {
                    clickable_freq[tiles[j][k][i].id] += 1;
                }
            }
        }
    }
    
    for (int i = 0; i < NUM_IMAGES; i++){
        if (clickable_freq[i] >= 2) {
            count++;
        }
    }

    gameState->matchable = count;
    
    return count;
}

void minutesAndSeconds(GameState* gameState) {
    const int countdown_minute = 5;

    int minutes = countdown_minute - (int)gameState->gameTime / 60 - 1;
    int seconds = 60 - (int)gameState->gameTime % 60;
    if (seconds == 60) {
        seconds = 0;
        minutes++;
    }

    gameState->minutes = minutes;
    gameState->seconds = seconds;
}

void isGameOver(GameState* gameState) {
    /*printf("Checking game over conditions: Frames: %d, Start Time: %.2f, Current Time: %.2f\n",
        framesCounter, gameState->startTime, GetTime());*/

    if (gameState->remainingTile == 0) {
        gameState->isMapSelected = false;
        gameState->gameScreen = victory;
        //printf("Game Over: Victory\n");
    }
    else if ((double)framesCounter / 60 - gameState->startTime > 300.0) {
        gameState->isMapSelected = false;
        gameState->gameScreen = gameOver;
        //printf("Game Over: Time Limit Exceeded\n");
    }
}

void drawGame() {
    DrawTexture(backGroundTexture[1], 0, 0, WHITE);

    DrawTexture(symbolsTexture[5], screenWidth / 2 + 500, 120, WHITE);
    DrawTexture(symbolsTexture[4], screenWidth / 2 + 500, 220, WHITE);
    DrawTexture(symbolsTexture[3], screenWidth / 2 + 495, 20, WHITE);
    DrawTexture(symbolsTexture[2], screenWidth / 2 + 495, 320, WHITE);

    DrawText(TextFormat("%d", (&gameState)->totalPoint), screenWidth / 2 + 570, 135, 40, RAYWHITE);
    DrawText(TextFormat("x %d", (&gameState)->matchable), screenWidth / 2 + 500 + 70, 240, 40, RAYWHITE);
    DrawText(TextFormat("%d:%.2d", gameState.minutes, gameState.seconds), screenWidth / 2 + 495 + 70, 40, 40, RAYWHITE);
    DrawText(TextFormat("x %d", gameState.remainingTile), screenWidth / 2 + 565, 340, 40, RAYWHITE);
    //DrawText(TextFormat("Remaining Tile: %.2lf", gameState.lastMatchTime), 1100, 500, 40, RED);

    for (int k = 1, a = 0; k < LAYER; k++) {
        for (int i = 0; i < ARRAY_Y; i++) {
            for (int j = 0; j < ARRAY_X; j++) {
                if (tiles[i][j][k].isExists == true) {
                    DrawTexture(tiles[i][j][k].texture, tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, tiles[i][j][k].color);
                }
            }
        }
    }
}

void resetGame() {
 
    for (int y = 0; y < ARRAY_Y; y++) {
        for (int x = 0; x < ARRAY_X; x++) {
            for (int l = 0; l < LAYER; l++) {
                newMap[y][x][l] = 0;
            }
        }
    }
    
    for (int y = 0; y < ARRAY_Y; y++) {
        for (int x = 0; x < ARRAY_X; x++) {
            map[y][x] = 0;
        }
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        isExist[i] = 0;
        clickable_freq[i] = 0;
    }

    randomFiller(original);
    resetHint(&hint);
    resetLastClicks(&LastClicks);
    (&gameState)->combo = 1;
    (&gameState)->gameTime = 0;
    (&gameState)->totalPoint = 0;
    (&gameState)->lastMatchTime = 0;
    (&gameState)->remainingTile = ARRAY_SIZE;
    (&gameState)->isMapSelected = false;
    saveGuiVisible = true;
    free(head);
}

void savingText() {
    // GUI to get text input and handle save trigger
    if (GuiButton((Rectangle) { screenWidth / 2 - 50, 300, 100, 30 }, "Save Text") || IsKeyPressed(KEY_ENTER)) {
        saveGuiVisible = false; // Hide the GUI elements after interaction
        /*int blank = 0;
        for (int i = 0; i < strlen(text); i++) {
            if (isblank(text[i])) {
                blank++;
            }
        }
        for (int i = 0; i < blank-1; i++){
            if (isblank(text[i])) {
                text[i] = '_';
            }
        }*/
        // Sort and save the new entry
        sort_and_write_scores("../output.txt", text, gameState.totalPoint);

        DrawText("Saved!", screenWidth / 2 - 50, 340, 20, GREEN); // Feedback to user
    }
    DrawText("Enter a name: ", screenWidth / 2 - 130, 100, 40, GOLD);
    GuiTextBox((Rectangle) { screenWidth / 2 - 100, 200, 200, 30 }, text, 256, true);
    

    if (!saveGuiVisible && fopen("../output.txt", "r") == NULL) {
        DrawText("Failed to save!", screenWidth / 2 - 50, 340, 20, RED); // Error message if file opening fails
    }
}

void sort_and_write_scores(const char* filename, const char* text, int points) {
    
    int count = 0;

    // Read existing data
    FILE* file1 = fopen("../output.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%[^0-9]%d\n", entries[count].text, &entries[count].points) == 2) {
            if (++count >= MAX_LINES) break;
        }
        fclose(file1);
    }

    // Add new entry
    strcpy(entries[count].text, text);
    entries[count].points = points;
    count++;

    // Sort the array
    qsort(entries, count, sizeof(ScoreEntry), compare_scores);

    // Write sorted data back to file
    file1 = fopen("../output.txt", "w");
    if (file1 != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %d\n", entries[i].text, entries[i].points);
        }
        fclose(file1);
    }
}

int compare_scores(const void* a, const void* b) {
    ScoreEntry* entryA = (ScoreEntry*)a;
    ScoreEntry* entryB = (ScoreEntry*)b;
    return (entryB->points - entryA->points);  // Descending order
}

void print10() {
    FILE* file3 = fopen("../output.txt", "r");
    if (file3 == NULL) {
        perror("Failed to open file");
        return; // Exit if file opening fails
    }

    for (size_t i = 0; i < 10; i++) {
        if (fscanf(file3, "%29s %d", array[i], &point[i]) != 2) {
            printf("Error reading line %zu\n", i + 1);
            break; // Exit loop if there is a mismatch or not enough data
        }
    }

    fclose(file3); // Close the file after reading

    for (int i = 0; i < 10; i++) {
        DrawText(TextFormat("%d. %s %d", i+1, array[i], point[i]), 10, 20 + i * 70, 50, GOLD); // Adjust position and size as needed
    }
}

void updateCombo(GameState* gameState) {
    /*if (gameState->combo == 0) {
        gameState->combo = 1;
    }*/
    if (gameState->gameTime - gameState->lastMatchTime > 6.0) {
        gameState->combo = 1;
    }
    //printf("%d\n", gameState->combo);
    printf("combo:%d elapsedTime: %f\n", gameState->combo, gameState->gameTime - gameState->lastMatchTime);
}

void drawCombo() {
    int width = 150 * (6 - (gameState.gameTime - gameState.lastMatchTime));
    if (gameState.combo == 2) {
        DrawRectangle(155, 20, width, 60, (Color) { 235, 227, 197, 255 });
        DrawTexture(symbolsTexture[0], 100, 10, RAYWHITE);
    }
    else if (gameState.combo == 3) {
        DrawRectangle(155, 20, width, 60, (Color) { 213, 194, 105, 255 });
        DrawTexture(symbolsTexture[1], 100, 10, RAYWHITE);
    }
}