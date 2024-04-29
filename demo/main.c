#include "globals.h"
#include "utils.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void processClick(LastTwoClicked*, LastTwoClicked*, Vector2, node**);
void addBegin(node**, LastTwoClicked*, int*);
void deleteBegin(node**, int*, LastTwoClicked*);
int giveHint(LastTwoClicked*, LastTwoClicked*, int*);
tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition);
void drawGame();
void updateGame(GameState*);

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Game");//pencere genislik X yukseklik, pencere adi 
    InitAudioDevice();
    SetSoundVolume(gameSound, 0.2);
    SetTargetFPS(60);
    InitImagesSounds();
    //PlaySound(gameSound);

    while (!WindowShouldClose()){
        drawGame();
    }
    unloadGameSounds();
    CloseAudioDevice();     // Close audio device
    CloseWindow();

    return 0;
}

void drawGame() {
    BeginDrawing();
    switch (gameScreen) {
    case starting:
        DrawTexture(backGroundTexture[0], 0, 0, WHITE);
        readFile(&gameState);
        if (gameState.isGameActive == true) {
            InitMap();
            InitObjects(&LastClicks);
            gameState.startTime = GetTime();
            gameState.remainingTile = ARRAY_SIZE;
            gameScreen = game;
        }
    case game:
        if (gameState.isGameActive == true) {
            
            gameState.matchable = countMatchableTiles(&gameState);
            gameState.currentTime = GetTime() - gameState.startTime;
            DrawTexture(backGroundTexture[1], 0, 0, WHITE);
            DrawTexture(symbolsTexture[5], screenWidth / 2 + 500, 120, WHITE);
            DrawTexture(symbolsTexture[4], screenWidth / 2 + 500, 220, WHITE);
            DrawTexture(symbolsTexture[3], screenWidth / 2 + 495, 20, WHITE);
            DrawTexture(symbolsTexture[2], screenWidth / 2 + 495, 320, WHITE);

            DrawText(TextFormat("%d", (&gameState)->totalPoint), screenWidth / 2 + 570, 135, 40, RAYWHITE);
            DrawText(TextFormat("x %d", (&gameState)->matchable), screenWidth / 2 + 500 + 70, 240, 40, RAYWHITE);
            DrawText(TextFormat("%.0lfs", gameState.currentTime), screenWidth / 2 + 495 + 70, 40, 40, RAYWHITE);
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
            updateGame(&gameState); //oyundaki degerleri update et
        }
    default: break;
    }

    framesCounter++; // kullanilmadi
    EndDrawing();
}

void updateGame(GameState* gameState) {
    mousePosition = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        processClick(&hint, &LastClicks, mousePosition, &head);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 25, 100, 30 }, "SHUFFLE")) {
        deletePointsforShuffle(gameState);
        resetHint(&hint);
        resetLastClicks(&LastClicks);
        PlaySound(buttonSound); //butona basildigi icin buton sesi verildi
        shuffle_all(&hint, isExist, original);
        countMatchableTiles(gameState);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 100, 100, 30 }, "HINT")) {
        deletePointsforHint(gameState);
        PlaySound(gameButtonSound);
        resetHint(&hint);
        resetLastClicks(&LastClicks);
        countMatchableTiles(gameState);
        giveHint(&hint, &LastClicks, clickable_freq);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 175, 100, 30 }, "UNDO")) {
        PlaySound(gameButtonSound);
        deleteBegin(&head, isExist, &LastClicks);
        countMatchableTiles(gameState);
        resetLastClicks(&LastClicks);
    }
    if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 + 250, 100, 30 }, "MAIN")) {
        PlaySound(buttonSound);
        gameScreen = starting;
        gameState->isGameActive = false;
        //gameState->runbefore = true;
    }
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
        (&gameState)->lastMatchTime = (&gameState)->currentTime; //en son 2tas bu surede silindi (komboda kullanilacak) 

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

    isExist[tempNode->data1->order] = 1;//linked listten silerken o taşa karşılık değer 1 oldu
    isExist[tempNode->data2->order] = 1;

    (tempNode)->data1->color = RAYWHITE;//linked listten silerken renklerini normal haline getir
    (tempNode)->data2->color = RAYWHITE;

    gameState.remainingTile += 2;

    *head = (*head)->nextNode;//basa eklemeli linked list oldugu icin listen artik bir sonraki next node ile temsil ediliyor, ilk elemani cikartacagiz
    //add begin - delete begin 

    free(tempNode);//node u listten kaldirdin, memorysini kullanmana gerek olmadigi icin serbest birak
}