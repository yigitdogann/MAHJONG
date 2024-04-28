#include "utils.h"

void InitImagesSounds() {
    backGround[1] = LoadImage("../assets/bg-black.png"); //arkaplan ekleme 
    backGroundTexture[1] = LoadTextureFromImage(backGround[1]);
    UnloadImage(backGround[1]);

    backGround[0] = LoadImage("../assets/bg-first.png"); //arkaplan ekleme 
    backGroundTexture[0] = LoadTextureFromImage(backGround[0]);
    UnloadImage(backGround[0]);

    gameSound = LoadSound("../assets/relaxing.mp3");
    buttonSound = LoadSound("../assets/buttonSound.wav");
    selectSound = LoadSound("../assets/selectSound.mp3");
    
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
    if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 - 150, 100, 30 }, "EASY")) {
        PlaySound(buttonSound);
        file = fopen("../assets/map2.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isGameActive = true;
    }
    else if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 - 50, 100, 30 }, "NORMAL")) {
        PlaySound(buttonSound);
        file = fopen("../assets/map1.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isGameActive = true;
    }
    else if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 + 50, 100, 30 }, "EXPERT")) {
        PlaySound(buttonSound);
        file = fopen("../assets/map3.txt", "r"); //dosyayi oku 
        if (file == NULL) { //dosyayi okumazsan konsola error ver
            perror("Failed to open file");
            return;
        }
        gameState->isGameActive = true;
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
    free(file);
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
                tiles[j][k][i].rectangle.y = (float)(j * HEIGHT / 2 - i * 10);

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
    if (LastClicks.lastClicked == NULL || LastClicks.previousClicked == NULL) {//son tiklanilan 2 tastan bir tanesi NULL ise false doner, program crash olmamasi icin 
        return false;
    }
    if (LastClicks.lastClicked->id != LastClicks.previousClicked->id) {//eger taslarin simgeleri ayni degilse bunlar masadan kaldirilamaz
        return false;
    }
    if ((LastClicks.lastClicked->x == LastClicks.previousClicked->x)//eger ayni tasa 2 kere cift tiklarsan masadan kalkmamasi icin 
        && (LastClicks.lastClicked->y == LastClicks.previousClicked->y)
        && (LastClicks.lastClicked->z == LastClicks.previousClicked->z)) {
        return false;
    }
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
    }
    if (LastClicks->previousClicked != NULL) {
        LastClicks->previousClicked->color = RAYWHITE;
    }

    if (LastClicks->lastClicked != NULL) {
        LastClicks->lastClicked = NULL;
    }
    if (LastClicks->previousClicked != NULL) {
        LastClicks->previousClicked = NULL;
    }
}

void unloadGameSounds() {
    UnloadSound(gameSound);
    UnloadSound(buttonSound);
}

void addPoints(LastTwoClicked LastClicks, GameState* gameState) {
    int combo = gameState->combo;
    float elapsedTime = gameState->currentTime - gameState->lastMatchTime;
    int point = LastClicks.lastClicked->point;

    if (elapsedTime <= 10.0) {
        if (combo < 3) {
            combo++;
        }
    }
    else if (elapsedTime > 10.0) {
        combo = 1;
    }

    gameState->totalPoint += 2 * combo * point;
    gameState->lastMatchTime = gameState->currentTime;
    gameState->combo = combo;

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
