#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raygui.h>

#define WIDTH 70
#define HEIGHT 96
#define ARRAY_Y 18
#define ARRAY_X 34
#define LAYER 6
#define NUM_IMAGES 36
#define ARRAY_SIZE 144

Image images[NUM_IMAGES];    // Array to hold image data
Texture2D textures[NUM_IMAGES]; // Array to hold textures

Image backGround[2];    // background
Texture2D backGroundTexture[2];

typedef struct tile {
    int point;//point of tile, tür
    int id; //shuffled, tür
    int order;
    bool isExists; //newMap, konum
    bool isClickable;

    Rectangle rectangle;
    Texture2D texture;
    Color color;
    int x, y, z;
}tile;

typedef struct LastTwoClicked {
    tile* previousClicked;
    tile* lastClicked;
}LastTwoClicked;

typedef struct gameState{
    // Define game state variables
    bool isGameActive;
    bool runbefore;

    // Additional game states like current level, score, etc.
} GameState;

typedef struct node {
    tile* data1;
    tile* data2;
    struct node* nextNode;
}node;

typedef enum screen { starting, game } screen;
screen gameScreen = starting;

LastTwoClicked LastClicks;
LastTwoClicked hint;

int map[ARRAY_Y][ARRAY_X];
int newMap[ARRAY_Y][ARRAY_X][LAYER] = { 0 };
int clickable_freq[NUM_IMAGES] = { 0 };

int original[ARRAY_SIZE];
int shuffled[ARRAY_SIZE];
int isExist[ARRAY_SIZE] = { 0 };

tile tiles[ARRAY_Y][ARRAY_X][LAYER];
Vector2 mousePosition;
Vector2 shuffleCircle;
Vector2 hintCircle;
const int screenWidth = 1440;
const int screenHeight = 900;
int framesCounter = 0; // Useful to count frames

bool isClickable(tile* tile);
bool removable(tile* tile1, tile* tile2);
void SelectScreen();
void updateGame();
void drawGame();
void shuffleBasedOnCondition(int* a, int* b, int size, int x);
void randomFiller();
void processClick();
void addBegin(node** head, LastTwoClicked* LastClicks);
void deleteBegin(node** head);
void shuffle_all();
tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition);
Color GetBlockColor(int point);
int giveHint();
void remover();
void InitMap();
void InitObjects(); 
void InitImages();

GameState gameState = { 0 };
FILE* file;
node* head = NULL; //linked list head pointer 

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Game");//pencere genislik X yukseklik, pencere adi 
    SetTargetFPS(60);
    backGround[0] = LoadImage("../assets/bg-first.png"); //arkaplan ekleme 
    backGroundTexture[0] = LoadTextureFromImage(backGround[0]);
    UnloadImage(backGround[0]);
    while (!WindowShouldClose())
    {
        drawGame();
    }

    CloseWindow();

    return 0;
}

void drawGame() {
    BeginDrawing();
    switch (gameScreen) {
    case starting:
        DrawTexture(backGroundTexture[0], 0, 0, WHITE);
        if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 - 150, 100, 30 }, "EASY")) {
            file = fopen("../assets/map2.txt", "r"); //dosyayi oku 
            if (file == NULL) { //dosyayi okumazsan konsola error ver
                perror("Failed to open file");
                return;
            }
            gameState.isGameActive = true;
        }
        else if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 - 50, 100, 30 }, "NORMAL")) {
            file = fopen("../assets/map1.txt", "r"); //dosyayi oku 
            if (file == NULL) { //dosyayi okumazsan konsola error ver
                perror("Failed to open file");
                return;
            }
            gameState.isGameActive = true;
        }
        else if (GuiButton((Rectangle) { screenWidth / 2 + 250, screenHeight / 2 + 50, 100, 30 }, "EXPERT")) {
            file = fopen("../assets/map3.txt", "r"); //dosyayi oku 
            if (file == NULL) { //dosyayi okumazsan konsola error ver
                perror("Failed to open file");
                return;
            }
            gameState.isGameActive = true;
        }
        if (gameState.isGameActive == true && gameState.runbefore == false) {
            InitImages();
            InitMap();
            InitObjects();
            free(file);
            gameScreen = game;
        }
        if (gameState.isGameActive == true && gameState.runbefore == true) {
            
            InitMap();
            InitObjects();
            gameScreen = game;
        }
    case game:
    if(gameState.isGameActive == true){  
        DrawTexture(backGroundTexture[1], 0, 0, WHITE);
        for (int k = 1, a = 0; k < LAYER; k++) {
            for (int i = 0; i < ARRAY_Y; i++) {
                for (int j = 0; j < ARRAY_X; j++) {
                    if (tiles[i][j][k].isExists == true) {
                        DrawTexture(tiles[i][j][k].texture, tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, tiles[i][j][k].color);
                    }
                }
            }
        }
        updateGame(); //oyundaki degerleri update et
    }
        default: break;
    }
    //framesCounter++;
    EndDrawing();
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

void InitObjects() {
    LastClicks.lastClicked = NULL;
    LastClicks.previousClicked = NULL;

    shuffleCircle.x = (float)1240;
    shuffleCircle.y = (float)450;
    hintCircle.x = (float)1240;
    hintCircle.y = (float)350;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        isExist[i] = 0;
    }
    randomFiller();

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
                    isExist[a]++;
                    a++;
                }
            }
        }
    }
}

void InitImages() {
    backGround[1] = LoadImage("../assets/bg-black.png"); //arkaplan ekleme 
    backGroundTexture[1] = LoadTextureFromImage(backGround[1]);
    UnloadImage(backGround[1]);

    for (int i = 0; i < NUM_IMAGES; i++) {
        char filePath[NUM_IMAGES]; // Buffer to hold the file path
        sprintf(filePath, "../assets/tiles/%d.png", i); // Create file path, note images are 1-indexed in your folder

        images[i] = LoadImage(filePath); // Load image data into CPU memory (RAM)
        textures[i] = LoadTextureFromImage(images[i]); // Image converted to texture, GPU memory (RAM -> VRAM)
        UnloadImage(images[i]); // Unload image data from CPU memory (RAM)
    }
}

void randomFiller() {
    // Diziyi doldur
    for (int i = 0; i < ARRAY_SIZE; i++) {
        original[i] = i / 4;  // Her 4 eleman aynı değeri alacak
    }

    shuffleBasedOnCondition(original, isExist, ARRAY_SIZE, 0);
    /*for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d", original[i]);
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d", isExist[i]);
    }*/
    //// Karıştırılmış diziyi yeni bir diziye kopyala
    //for (int i = 0; i < ARRAY_SIZE; i++) {
    //    shuffled[i] = original[i];
    //}
}

void updateGame() {
    mousePosition = GetMousePosition();
    
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            processClick();
        }
        if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 - 350, 100, 30 }, "SHUFFLE")) {
            shuffle_all();
        }
        if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 - 250, 100, 30 }, "HINT")) {
            remover();
            giveHint();
            
        }
        if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 - 150, 100, 30 }, "UNDO")) {
            deleteBegin(&head);
        }
        if (GuiButton((Rectangle) { screenWidth / 2 + 500, screenHeight / 2 - 50, 100, 30 }, "MAIN")) {
            gameScreen = starting;
            gameState.isGameActive = false;
            gameState.runbefore = true;
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

bool isClickable(tile* tile) {
    if (tile == NULL) {
        return false;
    }
    if (tile->isExists != true) {
        return false;
    }
    int x = tile->x;
    int y = tile->y;
    int z = tile->z;

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
    if (tiles[y][x-1][z + 1].isExists == true) {
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

void processClick() {
    tile* pointer = NULL;
    if (hint.lastClicked != NULL) {
        hint.lastClicked->color = RAYWHITE;
    }
    if (hint.previousClicked != NULL) {
        hint.previousClicked->color = RAYWHITE;
    }

    pointer = getTopMostTile(tiles, mousePosition);// imlec ile tiklanilan tasi point eder
    /*if (pointer != NULL)printf("order: %d\n", pointer->order);
    if (pointer != NULL)printf("isClickable: %d", pointer->isClickable);*/
    if (isClickable(pointer) && LastClicks.lastClicked != pointer) {//tiklanilan bir tas ise && eger son tiklanilan tasa bir kez daha tiklanirsa 2. isaretlemeyi kabul etmez
        printf("buraya girdi\n");
        if (LastClicks.previousClicked != NULL) {//crash olmamasi icin eklendi, bos olan bir adresin degerini degistirmeye calisacakti, boylece program cokecekti 
            LastClicks.previousClicked->color = RAYWHITE;//eski onceki tiklanilani beyaz yapar (son 2yi kirmizi yapiyoruz o artik 3.)
        }

        LastClicks.previousClicked = LastClicks.lastClicked;//yeni onceki tiklanilani eski son tiklanilan yapar
        if (LastClicks.previousClicked != NULL) {
            LastClicks.previousClicked->color = RAYWHITE;//yeni onceki tiklanilani eski son tiklanilan yapar
        }
        LastClicks.lastClicked = pointer;//son tiklanilani alir
        LastClicks.lastClicked->color = RED;//son tiklanilani kirmizi yapar

        if (removable(LastClicks.lastClicked, LastClicks.previousClicked) == true) {//son 2 tas kaldirilabilir tas mi?
            addBegin(&head, &LastClicks);//linked liste ekle (masadan kaldir)
        }
    }
    return;
}

void addBegin(node** head, LastTwoClicked* LastClicks) {
    node* newNode = (node*)malloc(sizeof(node)); // memoryden node kadar yer ayir ve bu yeri pointerla erisilebilir yap
    if (newNode == NULL) {//memory ayrilamadiysa konsola allocation erroru yazdir.
        printf("Memory allocation error in add_node_begin()\n");
        return;
    }
    newNode->data1 = LastClicks->lastClicked; //son tiklanilan ciftin adresini tut
    newNode->data2 = LastClicks->previousClicked;

    newNode->data1->isExists = false; // son tiklanilan taslarin cizilemez ve tiklanilamaz olmasi icin 
    newNode->data2->isExists = false;

    isExist[newNode->data1->order] = 0;//linked liste eklerken o taşa karşılık değer 0 oldu
    isExist[newNode->data2->order] = 0;

    LastClicks->lastClicked = NULL; //son 2 tiklanilan tas bilgisini sifirla
    LastClicks->previousClicked = NULL;

    newNode->nextNode = *head;

    *head = newNode;
    return;
}

void deleteBegin(node** head) {
    if (*head == NULL) {//linked listten kaldirilacak oge yok, linked list zaten bos
        // printf("Linked list is already empty\n");
        return;
    }
    node* tempNode = *head;
    
    tempNode->data1->isExists = true;//linked listten silerken cizilebilir hale getir
    tempNode->data2->isExists = true;

    isExist[tempNode->data1->order] = 1;//linked listten silerken o taşa karşılık değer 1 oldu
    isExist[tempNode->data2->order] = 1;

    (*head)->data1->color = RAYWHITE;//linked listten silerken renklerini normal haline getir
    (*head)->data2->color = RAYWHITE;

    if (LastClicks.lastClicked != NULL) {
        LastClicks.lastClicked->color = RAYWHITE;//son tıklanılanı rengini siler
        LastClicks.lastClicked = NULL;//son tıklanılan geçmişini unutur.
    }
    if (LastClicks.previousClicked != NULL) {
        LastClicks.previousClicked = NULL;
    }
    *head = (*head)->nextNode;//basa eklemeli linked list oldugu icin listen artik bir sonraki next node ile temsil ediliyor, ilk elemani cikartacagiz
    //add begin - delete begin 

    free(tempNode);//node u listten kaldirdin, memorysini kullanmana gerek olmadigi icin serbest birak
}

void shuffle_all() {
    if (hint.lastClicked != NULL && hint.previousClicked != NULL) {
        hint.lastClicked->color = RAYWHITE;
        hint.previousClicked->color = RAYWHITE;

        hint.lastClicked = NULL;
        hint.previousClicked = NULL;
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

    if (hint.lastClicked != NULL && hint.previousClicked != NULL) { //karıştırma komutuna basarsan mavi ile işaretlenen taşları normal haline getirir
        hint.lastClicked->color = RAYWHITE; 
        hint.previousClicked->color = RAYWHITE;
    }
}

// son tiklanilan 2 tas masadan kaldirilabilir mi? YES : NO
bool removable(tile* tile1, tile* tile2) {//son tiklanilan 2 tasi parametre olarak alir
    if (tile1 == NULL || tile2 == NULL) {//son tiklanilan 2 tastan bir tanesi NULL ise false doner, program crash olmamasi icin 
        return false;
    }
    if (tile1->id != tile2->id) {//eger taslarin simgeleri ayni degilse bunlar masadan kaldirilamaz
        return false;
    }
    if ((tile1->x == tile2->x)//eger ayni tasa 2 kere cift tiklarsan masadan kalkmamasi icin 
        && (tile1->y == tile2->y)
        && (tile1->z == tile2->z)) {
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
    free(indices);
}

void remover() {
    if (hint.lastClicked != NULL) {
        hint.lastClicked = NULL;
    }
    if (hint.previousClicked != NULL) {
        hint.previousClicked = NULL;
    }
}

int giveHint() {
    if (LastClicks.lastClicked != NULL) {
        LastClicks.lastClicked->color = RAYWHITE;
        LastClicks.lastClicked = NULL;
    }
    if (LastClicks.previousClicked!= NULL) {
        LastClicks.previousClicked = NULL;
    }

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

    int hintedId = -1;
    for (int i = 0; i < NUM_IMAGES; i++) {
        if (clickable_freq[i] >= 2) {
            hintedId = i;
        }
    }
    if (hintedId == -1) {
        printf("there are no removable\n");
        return 1;
    }
    for (int i = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                if (tiles[j][k][i].id == hintedId && tiles[j][k][i].isClickable) {
                    if (hint.lastClicked == NULL) {
                        hint.lastClicked = &tiles[j][k][i];
                    }
                    else if (hint.previousClicked == NULL) {
                        hint.previousClicked = &tiles[j][k][i];
                        goto yer;
                    }
                }
            }
        }
    }

    yer:
    if (hint.lastClicked != NULL && hint.previousClicked != NULL) {
        hint.lastClicked->color = BLUE;
        hint.previousClicked->color = BLUE;
    }
    for (int i = 0; i < NUM_IMAGES; i++) {
        clickable_freq[i] = 0;
    }
    return 2;   
}