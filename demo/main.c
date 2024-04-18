#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 70
#define HEIGHT 96
#define ARRAY_Y 18
#define ARRAY_X 30
#define LAYER 6
#define NUM_IMAGES 36
#define ARRAY_SIZE 144


Image images[NUM_IMAGES];    // Array to hold image data
Texture2D textures[NUM_IMAGES]; // Array to hold textures

Image backGround;    // background
Texture2D backGroundTexture;

typedef struct tile {
    int point;//point of tile, tür
    int id; //shuffled, tür
    bool isExists; //newMap, konum
    Rectangle rectangle;
    Texture2D texture;

    int x, y, z;
}tile;

//typedef struct LastTwoClicked{
//    tile* previousClicked;
//    tile* lastClicked;
//}LastTwoClicked;
//
//typedef struct doublyLinkedList {
//    tile
//}doublyLinkedList;
//
//LastTwoClicked LastClicks;

int map[ARRAY_Y][ARRAY_X];
int newMap[ARRAY_Y][ARRAY_X][LAYER] = { 0 };

int original[ARRAY_SIZE];
int shuffled[ARRAY_SIZE];

tile tiles[ARRAY_Y][ARRAY_X][LAYER];
int values[ARRAY_Y][ARRAY_X];
Vector2 mousePosition;
const int screenWidth = 1440;
const int screenHeight = 900;
int framesCounter = 0;          // Useful to count frames

void InitGame();
void updateGame();
void shuffle(int* array, int n);
void randomFiller();
void processClick();

tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition);
Color GetBlockColor(int point);
FILE* file;

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Game");
    InitGame();

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        updateGame();
        BeginDrawing();

        //drawing rectangles (mahjong tiles)

        DrawTexture(backGroundTexture, 0, 0, WHITE);

        int found_i = -1, found_j = -1, found_k = -1;
        for (int k = 1, a = 0; k < LAYER; k++) {
            for (int i = 0; i < ARRAY_Y; i++) {
                for (int j = 0; j < ARRAY_X; j++) {
                    if (tiles[i][j][k].isExists == true) {
                        Color color = GetBlockColor(k);
                        //DrawRectangle(tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, WIDTH, HEIGHT, color);
                        DrawTexture(tiles[i][j][k].texture, tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, RAYWHITE);
                        DrawCircle(tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, 5, color);
                        a++;
                    }
                }
            }
        }

        DrawCircle(tiles[0][0][0].rectangle.x, tiles[0][0][0].rectangle.y, 10, RED);
        //DrawRectangleLinesEx((Rectangle) { 670, 418, 70, 95 }, 12.5, (Color) { 0, 0, 0, 50 });
        //DrawRectangleLinesEx((Rectangle) { 645, 382, 140, 190 }, 20.5, (Color) { 0, 0, 0, 50 });
        //DrawRectangleLinesEx((Rectangle) { 670, 418, 70, 95 }, 12.5, (Color) { 0, 0, 0, 50 });

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void InitGame() {

    randomFiller();
    // Karıştırılmış diziyi yazdır (isteğe bağlı)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", shuffled[i]);
        if ((i + 1) % 12 == 0) printf("\n");
    }


    //MAP SELECTION YAPILACAK
    //read map from text file
    file = fopen("../assets/original-map.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    int x, y;

    // Reading the file into a 2D array
    for (y = 0; y < ARRAY_Y; y++) {
        for (x = 0; x < ARRAY_X; x++) {
            if (fscanf(file, "%1d", &map[y][x]) != 1) {
                perror("Failed to read data");
                return;
            }
        }
    }

    fclose(file);  // Always close the file after you're done reading

    /*
    // Print original map for debugging
    for (int i = 0; i < ARRAY_Y; i++) {
        for (int j = 0; j < ARRAY_X; j++) {
            printf("%d", map[i][j]);
        }
        puts("");
    }
    */

    backGround = LoadImage("../assets/bg-black.png");
    backGroundTexture = LoadTextureFromImage(backGround);
    UnloadImage(backGround);

    for (int i = 0; i < NUM_IMAGES; i++) {
        char filePath[NUM_IMAGES]; // Buffer to hold the file path
        sprintf(filePath, "../assets/tiles/%d.png", i + 1); // Create file path, note images are 1-indexed in your folder

        images[i] = LoadImage(filePath); // Load image data into CPU memory (RAM)
        textures[i] = LoadTextureFromImage(images[i]); // Image converted to texture, GPU memory (RAM -> VRAM)
        UnloadImage(images[i]); // Unload image data from CPU memory (RAM)
    }

    for (int k = LAYER - 1; k > 0; k--) {
        for (int i = 0; i < HEIGHT - 1; i++) {
            for (int j = 0; j < WIDTH - 1; j++) {
                if ((map[i][j] == k) && (map[i][j + 1] == k) && (map[i + 1][j] == k) && (map[i + 1][j + 1] == k)) {
                    newMap[i][j][k]++;
                    map[i][j]--;
                    map[i][j + 1]--;
                    map[i + 1][j]--;
                    map[i + 1][j + 1]--;
                }
            }
        }
    }


    //// Print new map for debugging
    //for (int i = 0; i < ARRAY_Y; i++) {
    //    for (int j = 0; j < ARRAY_X; j++) {
    //        printf("%d", newMap[i][j][5]);
    //    }
    //    puts("");
    //}
    //puts("");

    for (int i = 0, a = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                tiles[j][k][i].y = j;
                tiles[j][k][i].x = k;
                tiles[j][k][i].z = i;
                tiles[j][k][i].isExists = newMap[j][k][i];
                tiles[j][k][i].rectangle.x = (float)(k * WIDTH / 2 - i * 8);
                tiles[j][k][i].rectangle.y = (float)(j * HEIGHT / 2 - i * 20);
                tiles[j][k][i].rectangle.width = (float)WIDTH;
                tiles[j][k][i].rectangle.height = (float)HEIGHT;
                if (tiles[j][k][i].isExists == true) {
                    tiles[j][k][i].id = shuffled[a];
                    tiles[j][k][i].texture = textures[shuffled[a] - 1];
                    a++;
                }
            }
        }
    }


}

void randomFiller() {
    // Diziyi doldur
    for (int i = 0; i < ARRAY_SIZE; i++) {
        original[i] = i / 4 + 1;  // Her 4 eleman aynı değeri alacak
    }

    // Rastgele sayı üreticisini başlat
    srand(time(NULL));

    // Orijinal diziyi karıştır
    shuffle(original, ARRAY_SIZE);

    // Karıştırılmış diziyi yeni bir diziye kopyala
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shuffled[i] = original[i];
    }
}

// Fisher-Yates shuffle algorithm
void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


void updateGame() {
    mousePosition = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) == true) {
        processClick();
    }
}

//color according to points (drawing fonksiyonunda fazladan if/else kurmaktan kacinildi)
Color GetBlockColor(int layer) {
    switch (layer) {
    case 1: return BLACK;
    case 2: return YELLOW;
    case 3: return GREEN;
    case 4: return BLUE;
    case 5: return RED;
    default: return MAGENTA;
    }
}

tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition) {
    tile* pointer = NULL;
    for (int i = LAYER - 1; i >= 0; i--) {
        for (int j = 0, a = 0; j < ARRAY_Y; j++){
            for (int k = 0; k < ARRAY_X; k++){
                if ((CheckCollisionPointRec(mousePosition, tiles[j][k][i].rectangle)) && tiles[j][k][i].isExists == true) {
                pointer = &tiles[j][k][i];
                goto end;
                }
            }
        }
    }
    end:
    if (pointer != NULL) {
        printf("Z:%d Y:%d X:%d\n", pointer->z, pointer->y, pointer->x);
    }
    else {
        printf("No tile found at the given position.\n");
    }
    return pointer;
}

bool isClickable(tile* tile) {
    int x = tile->x;
    int y = tile->y;
    int z = tile->z;

    if (tiles[y][x + 1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y + 1][x + 1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y][x][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y + 1][x][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y -1][x-1][z+1].isExists == true) {
        return false;
    }
    if (tiles[y + 1][x-1][z + 1].isExists == true) {
        return false;
    }
    if (tiles[y - 1][x+1][z + 1].isExists == true) {
        return false;
    }
    
    bool right = tiles[y][x + 1][z].isExists || tiles[y][x + 2][z].isExists || tiles[y + 1][x + 1][z].isExists 
        || tiles[y + 1][x + 2][z].isExists || tiles[y - 1][x + 1][z].isExists || tiles[y - 1][x + 2][z].isExists;
    
    bool left = tiles[y][x - 1][z].isExists || tiles[y][x - 2][z].isExists || tiles[y + 1][x - 1][z].isExists
        || tiles[y + 1][x - 2][z].isExists || tiles[y - 1][x - 1][z].isExists || tiles[y - 1][x - 2][z].isExists;
    if ((right && left) == true) {
        return false;
    }
    
    return true;
}

void processClick() {
    tile* pointer = NULL;
    pointer = getTopMostTile(tiles, mousePosition);

    if (isClickable(pointer)) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }

}