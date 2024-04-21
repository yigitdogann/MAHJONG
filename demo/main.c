#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 70
#define HEIGHT 96
#define ARRAY_Y 18
#define ARRAY_X 34
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
    Color color;
    int x, y, z;
}tile;

typedef struct LastTwoClicked {
    tile* previousClicked;
    tile* lastClicked;
}LastTwoClicked;

typedef struct node {
    tile* data1;
    tile* data2;
    struct node* nextNode;
}node;

typedef enum gameScreen { Starting, Game, Options };
gameScreen = Starting;
LastTwoClicked LastClicks;

int map[ARRAY_Y][ARRAY_X];
int newMap[ARRAY_Y][ARRAY_X][LAYER] = { 0 };

int original[ARRAY_SIZE];
int shuffled[ARRAY_SIZE];

tile tiles[ARRAY_Y][ARRAY_X][LAYER];
int values[ARRAY_Y][ARRAY_X];
Vector2 mousePosition;
const int screenWidth = 1440;
const int screenHeight = 900;
int framesCounter = 0; // Useful to count frames
Vector2 shuffleCircle;
bool isSame(tile* tile1, tile* tile2);
bool removable(LastTwoClicked LastClicks);
void InitGame();
void updateGame();
void drawGame();

void shuffle(int* array, int n);
void randomFiller();
void processClick();
void addBegin(node** head, LastTwoClicked* LastClicks);
void deleteBegin(node** head);
void shuffle_all();
tile* getTopMostTile(tile tiles[ARRAY_Y][ARRAY_X][LAYER], Vector2 mousePosition);
Color GetBlockColor(int point);
FILE* file;
node* head = NULL; //linked list head pointer 

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mahjong Game");//pencere genislik X yukseklik, pencere adi 
    SetTargetFPS(60);
    InitGame();

    while (!WindowShouldClose())
    {
        updateGame(); //oyundaki degerleri update et
        drawGame();
    }

    CloseWindow();

    return 0;
}

void InitGame() {
    LastClicks.lastClicked = NULL;
    LastClicks.previousClicked = NULL;

    shuffleCircle.x = (float)1240;
    shuffleCircle.y = (float)450;

    randomFiller();
    // Karıştırılmış diziyi yazdır (isteğe bağlı)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", shuffled[i]);
        if ((i + 1) % 12 == 0) printf("\n");
    }

    //read map from text file
    file = fopen("../assets/original-map.txt", "r"); //dosyayi oku 
    if (file == NULL) { //dosyayi okumazsan konsola error ver
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

    backGround = LoadImage("../assets/bg-black.png"); //arkaplan ekleme 
    backGroundTexture = LoadTextureFromImage(backGround);
    UnloadImage(backGround);

    for (int i = 0; i < NUM_IMAGES; i++) {
        char filePath[NUM_IMAGES]; // Buffer to hold the file path
        sprintf(filePath, "../assets/tiles/%d.png", i + 1); // Create file path, note images are 1-indexed in your folder

        images[i] = LoadImage(filePath); // Load image data into CPU memory (RAM)
        textures[i] = LoadTextureFromImage(images[i]); // Image converted to texture, GPU memory (RAM -> VRAM)
        UnloadImage(images[i]); // Unload image data from CPU memory (RAM)
    }

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
                tiles[j][k][i].y = j; // ileride 2 tasi kiyaslarken kullanilacak 
                tiles[j][k][i].x = k;
                tiles[j][k][i].z = i;

                tiles[j][k][i].isExists = newMap[j][k][i]; // 3 boyutlu arrayde deger 1 ise oraya tas cizecegiz

                tiles[j][k][i].rectangle.x = (float)(k * WIDTH / 2 - i * 10); //taslarin x ve y degerleri girildi, i ile carpim 3 boyun goruntusu kazandirmak icin eklendi
                tiles[j][k][i].rectangle.y = (float)(j * HEIGHT / 2 - i * 10);

                tiles[j][k][i].rectangle.width = (float)WIDTH; //tasin yuksekligi
                tiles[j][k][i].rectangle.height = (float)HEIGHT;// tasin uzunlugu
                if (tiles[j][k][i].isExists == true) {// eger tas var ise
                    tiles[j][k][i].id = shuffled[a]; //taslari karistirildi ve cizilmesi gerekenlere sirayla atanacak
                    tiles[j][k][i].texture = textures[shuffled[a] - 1];
                    a++;// bir sonraki gorseli ata 
                    //bata gote koyum
                    tiles[j][k][i].color = RAYWHITE;//tasi normal renk tonunda ciz, (farkli renk koyup degisiklikler gozlenebilir)
                }
            }
        }
    }
}

void drawGame() {
    BeginDrawing();
    DrawTexture(backGroundTexture, 0, 0, WHITE);

    DrawCircleV(shuffleCircle, 30.0, GOLD);
    for (int k = 1, a = 0; k < LAYER; k++) {
        for (int i = 0; i < ARRAY_Y; i++) {
            for (int j = 0; j < ARRAY_X; j++) {
                if (tiles[i][j][k].isExists == true) {
                    //@Color color = GetBlockColor(k);
                    DrawTexture(tiles[i][j][k].texture, tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, tiles[i][j][k].color);
                    //@DrawCircle(tiles[i][j][k].rectangle.x, tiles[i][j][k].rectangle.y, 5, color);
                    //@a++;
                }
            }
        }
    }

    EndDrawing();
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
        if (!CheckCollisionPointCircle(mousePosition, shuffleCircle, 30.0)) {
            processClick();
        }
        else {
            shuffle_all();
        }
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

    bool RR = tiles[y - 1][x + 2][z].isExists || tiles[y + 1][x + 2][z].isExists;
    //üst kat

    bool LL = tiles[y - 1][x - 2][z].isExists || tiles[y + 1][x - 2][z].isExists;
    //alt kat

    bool right = tiles[y][x + 2][z].isExists;
    bool left = tiles[y][x - 2][z].isExists;

    if ((right || RR) && (left || LL)) {
        return false;
    }

    return true;
}

void processClick() {
    tile* pointer = NULL;

    pointer = getTopMostTile(tiles, mousePosition);// imlec ile tiklanilan tasi point eder

    if (isClickable(pointer) && LastClicks.lastClicked != pointer) {//tiklanilan bir tas ise && eger son tiklanilan tasa bir kez daha tiklanirsa 2. isaretlemeyi kabul etmez
        printf("buraya girdi\n");
        if (LastClicks.previousClicked != NULL) {//crash olmamasi icin eklendi, bos olan bir adresin degerini degistirmeye calisacakti, boylece program cokecekti 
            LastClicks.previousClicked->color = RAYWHITE;//eski onceki tiklanilani beyaz yapar (son 2yi kirmizi yapiyoruz o artik 3.)
        }

        LastClicks.previousClicked = LastClicks.lastClicked;//yeni onceki tiklanilani eski son tiklanilan yapar
        LastClicks.lastClicked = pointer;//son tiklanilani alir
        LastClicks.lastClicked->color = RED;//son tiklanilani kirmizi yapar

        if (removable(LastClicks) == true) {//son 2 tas kaldirilabilir tas mi?
            addBegin(&head, &LastClicks);//linked liste ekle (masadan kaldir)
        }
    }
    if (pointer == NULL) { // tas olmayan yerlere tiklanilirsa 
        deleteBegin(&head); //linked listten cikar (masaya kaldirilan son cifti geri ekler)
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

    LastClicks->lastClicked = NULL; //son 2 tiklanilan tas bilgisini sifirla
    LastClicks->previousClicked = NULL;

    newNode->nextNode = *head;

    *head = newNode;
    return;
}

void deleteBegin(node** head) {
    if (*head == NULL) {//linked listten kaldirilacak oge yok, linked list zaten bos
        printf("Linked list is already empty\n");
        return;
    }
    node* tempNode = *head;

    (*head)->data1->isExists = true;//linked listten silerken cizilebilir hale getir
    (*head)->data2->isExists = true;

    (*head)->data1->color = RAYWHITE;//linked listten silerken renklerini normal haline getir
    (*head)->data2->color = RAYWHITE;

    *head = (*head)->nextNode;//basa eklemeli linked list oldugu icin listen artik bir sonraki next node ile temsil ediliyor, ilk elemani cikartacagiz
    //add begin - delete begin 

    free(tempNode);//node u listten kaldirdin, memorysini kullanmana gerek olmadigi icin serbest birak
}

void shuffle_all() {
    shuffle(shuffled, ARRAY_SIZE);
    for (int i = 0, a = 0; i < LAYER; i++) {
        for (int j = 0; j < ARRAY_Y; j++) {
            for (int k = 0; k < ARRAY_X; k++) {
                if (tiles[j][k][i].isExists == true) {
                    tiles[j][k][i].id = shuffled[a];//taslari karilmis desteden sec
                    tiles[j][k][i].texture = textures[shuffled[a] - 1];//desteden secilen sayiya gore tasa sekil atamasi yap
                    a++;
                }
            }
        }
    }
}

// son tiklanilan 2 tas masadan kaldirilabilir mi? YES : NO
bool removable(LastTwoClicked LastClicks) {//son tiklanilan 2 tasi parametre olarak alir
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