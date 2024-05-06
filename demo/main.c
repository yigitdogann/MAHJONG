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

Shader shader;

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Game");//pencere genislik X yukseklik, pencere adi 
    Shader shader = LoadShader("../basic.vs", "../basic.fs");
    InitAudioDevice();
    SetTargetFPS(60);
    InitImagesSounds();
    //PlaySound(gameSound);
    
    while (!WindowShouldClose()){
        updateAndDraw();
    }
    unloadGameSounds();
    CloseAudioDevice();     // Close audio device
    CloseWindow();

    return 0;
}

void updateAndDraw() {
    BeginDrawing();
    switch (gameState.gameScreen) {
    case starting:
        if (!gameState.isMapSelected) {
            DrawTexture(backGroundTexture[0], 0, 0, WHITE);
            readFile(&gameState); // Consider moving or optimizing this call
        }
        if (gameState.isMapSelected) {
            InitMap();
            InitObjects(&LastClicks);
            gameState.remainingTile = ARRAY_SIZE;
            gameState.startTime = GetTime();
            gameState.gameScreen = game;

            DrawTexture(backGroundTexture[1], 0, 0, WHITE); 
        }
        break;
    case game:
        drawGame();
        drawCombo();
        updateGame(&gameState);
        break;
    case gameOver:
        DrawTexture(backGroundTexture[2], 0, 0, RAYWHITE);
        if (GuiButton((Rectangle) { 670, 610, 100, 30 }, "MAIN")) {
            resetGame();
            PlaySound(buttonSound);
            gameState.gameScreen = starting;
        }
        if (saveGuiVisible == true && gameState.totalPoint > point[9]) {
            savingText();
        }
        if (saveGuiVisible == false) {
            DrawText("Saved!", screenWidth / 2 - 50, 340, 20, GREEN); // Feedback to user
        }
        print10();
        if (gameState.totalPoint > point[9]) {
            DrawText("NEW HIGH SCORE!!!", 500, 400, 100, GOLD);
        }

        break;
    case victory:
        DrawTexture(backGroundTexture[3], 0, 0, WHITE);
        if (GuiButton((Rectangle) { 670, 610, 100, 30 }, "MAIN")) {
            resetGame();
            PlaySound(buttonSound);
            gameState.gameScreen = starting;
        }
        if (saveGuiVisible == true && gameState.totalPoint > point[9]) {
            savingText();
        }
        if (saveGuiVisible == false) {
            DrawText("Saved!", screenWidth / 2 - 50, 340, 20, GREEN); // Feedback to user
        }
        print10();
        if (gameState.totalPoint > point[9]) {
            DrawText("NEW HIGH SCORE!!!", 500, 400, 100, GOLD);
        }        

        break;
    }

    framesCounter++;
    EndDrawing();
}

void updateGame(GameState* gameState) {
    mousePosition = GetMousePosition();
    minutesAndSeconds(gameState); //zamanı dakika saniye cinsinden 3 dakikadan itibaren geriye saymamızı sağlar
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        processClick(&hint, &LastClicks, mousePosition, &head);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 25, 100, 30 }, "SHUFFLE (TAB)") || IsKeyPressed(KEY_TAB)) {
        deletePointsforShuffle(gameState);
        resetHint(&hint);
        resetLastClicks(&LastClicks);
        PlaySound(buttonSound); //butona basildigi icin buton sesi verildi
        shuffle_all(&hint, isExist, original);
        countMatchableTiles(gameState);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 100, 100, 30 }, "HINT (SPACE)") || IsKeyPressed(KEY_SPACE)) {
        deletePointsforHint(gameState);
        PlaySound(gameButtonSound);
        resetHint(&hint);
        resetLastClicks(&LastClicks);
        countMatchableTiles(gameState);
        giveHint(&hint, &LastClicks, clickable_freq);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 175, 100, 30 }, "UNDO (CTRL)") || IsKeyPressed(KEY_LEFT_CONTROL)) {
        PlaySound(gameButtonSound);
        deleteBegin(&head, isExist, &LastClicks);
        countMatchableTiles(gameState);
        resetLastClicks(&LastClicks);
        gameState->combo = 1;
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 250, 100, 30 }, "MAIN")) {
        PlaySound(buttonSound);
        resetGame();
        gameState->gameScreen = starting;
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 325, 100, 30 }, "END (X)") || IsKeyPressed(KEY_X)) {
        PlaySound(buttonSound);
        gameState->gameScreen = gameOver;
    }
    gameState->gameTime = GetTime() - gameState->startTime;
    gameState->matchable = countMatchableTiles(gameState);
    isGameOver(gameState);
    updateCombo(gameState);
}

tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition) {
    for (int i = LAYER - 1; i >= 0; i--) {//ilk en yukaridaki layerlari kontrol et
        for (int j = 0, a = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                if ((CheckCollisionPointRec(mousePosition, tiles[j][k][i].rectangle)) && tiles[j][k][i].isExists == true) { //3 boyutu da kontrol eder ve eger tas varsa, mousun ucundaki tasi bulur 
                    return &tiles[j][k][i]; // pointera gonderir ileride kullanacagiz
                }
            }
        }
    }

    return NULL;
}

void processClick(LastTwoClicked* hint, LastTwoClicked* LastClicks, Vector2 mousePosition, node** head) {
    tile* pointer = NULL;
    resetHint(hint);
    
    pointer = getTopMostTile(tiles, mousePosition);// imlec ile tiklanilan tasi point eder
    
    //tiklanilabilir bir tas ise && son tiklanilan tasa bir kez daha tiklanmiyorsa LastClicked guncellenir
    if (isClickable(pointer) && LastClicks->lastClicked != pointer) {
        PlaySound(selectSound); //secilebilir bir tasa tikladigi icin selectSound sesi verildi

        if (LastClicks->previousClicked != NULL) {//crash olmamasi icin eklendi, bos olan bir adresin degerini degistirmeye calisacakti, boylece program cokecekti 
            LastClicks->previousClicked->color = RAYWHITE;//eski onceki tiklanilani beyaz yapar (son 2yi kirmizi yapiyoruz o artik 3.)
        }        
        if (LastClicks->lastClicked != NULL) {
            LastClicks->lastClicked->color = RAYWHITE;//yeni onceki tiklanilani eski son tiklanilan yapar
        }

        LastClicks->previousClicked = LastClicks->lastClicked;//yeni onceki tiklanilani eski son tiklanilan yapar
        LastClicks->lastClicked = pointer;//son tiklanilani alir
        if (LastClicks->lastClicked != NULL) {
            LastClicks->lastClicked->color = RED;//son tiklanilani kirmizi yapar
        }

        if (isRemovable(*LastClicks) == true) {//son 2 tas kaldirilabilir tas mi?
            addBegin(head, LastClicks, isExist);//linked liste ekle (masadan kaldir)
            addPoints(*LastClicks, &gameState);
            resetLastClicks(LastClicks);
            countMatchableTiles(&gameState);
        }
    }
    //free(pointer);
    return;
}

void addBegin(node** head, LastTwoClicked* LastClicks, int* isExist) {
    node* newNode = (node*)malloc(sizeof(node)); // memoryden node kadar yer ayir ve bu yeri pointerla erisilebilir yap
    if (newNode == NULL) {//memory ayrilamadiysa konsola allocation erroru yazdir.
        printf("Memory allocation error in add_node_begin()\n");
        return;
    }

    gameState.remainingTile -= 2;

    LastClicks->lastClicked->isExists = false; // son tiklanilan taslarin cizilemez ve tiklanilamaz olmasi icin 
    LastClicks->previousClicked->isExists = false;

    newNode->data1 = LastClicks->lastClicked; //son tiklanilan ciftin adresini tut
    newNode->data2 = LastClicks->previousClicked;

    newNode->data1->combo = (&gameState)->combo;
    newNode->data2->combo = (&gameState)->combo;

    isExist[newNode->data1->order] = 0;//linked liste eklerken o taşa karşılık değer 0 oldu
    isExist[newNode->data2->order] = 0;
   
    newNode->nextNode = *head; //newNode listenin en basina eklendi

    *head = newNode; //artik head cagrilirken onceki satirda guncellenmis liste cagirilacak
    
    return;
}

int giveHint(LastTwoClicked* hint, LastTwoClicked* LastClicks, int* clickable_freq) {
    int hintedId = -1;
    for (int i = 0; i < NUM_IMAGES; i++) {
        if (clickable_freq[i] >= 2) {
            hintedId = i;
        }
    }

    if (hintedId == -1) {
        printf("there are no removable\n"); //tahtada erişilebilecek taş olmadığını söyler
        return 1;
    }
    
    for (int i = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                if (tiles[j][k][i].id == hintedId && tiles[j][k][i].isClickable) {
                    if (hint->lastClicked == NULL) {
                        hint->lastClicked = &tiles[j][k][i];
                    }
                    else if (hint->previousClicked == NULL) {
                        hint->previousClicked = &tiles[j][k][i];
                        goto loop_out;
                    }
                }
            }
        }
    }

    loop_out://BURA COK MU KOTU BATAYA SOR
    if (hint->lastClicked != NULL && hint->previousClicked != NULL) {
        hint->lastClicked->color = BLUE;
        hint->previousClicked->color = BLUE;
    }
    return 2;
}

void deleteBegin(node** head, int* isExist, LastTwoClicked* LastClicks) {
    if (*head == NULL) {//linked listten kaldirilacak oge yok, linked list zaten bos
        // printf("Linked list is already empty\n");
        return;
    }
    node* tempNode = *head;

    tempNode->data1->isExists = true;//linked listten silerken cizilebilir hale getir
    tempNode->data2->isExists = true;
    
    (&gameState)->totalPoint += -2 * tempNode->data1->combo * tempNode->data1->point;

    isExist[tempNode->data1->order] = 1;//linked listten silerken o taşa karşılık değer 1 oldu
    isExist[tempNode->data2->order] = 1;

    (tempNode)->data1->color = RAYWHITE;//linked listten silerken renklerini normal haline getir
    (tempNode)->data2->color = RAYWHITE;

    gameState.remainingTile += 2;

    *head = (*head)->nextNode;//basa eklemeli linked list oldugu icin listen artik bir sonraki next node ile temsil ediliyor, ilk elemani cikartacagiz
    //add begin - delete begin 

    free(tempNode);//node u listten kaldirdin, memorysini kullanmana gerek olmadigi icin serbest birak
}