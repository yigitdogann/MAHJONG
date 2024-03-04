#include <raylib.h>
#include <stdio.h>

#define ROW 10
#define COLUMN 25
#define WIDTH 50
#define HEIGHT 70
#define COLOR1 (Color){ 0, 0, 0, 40} //used in gridlines  

FILE* file;

typedef struct block {
	Rectangle rec;
	Color color;
	int active;
	int point;
	bool drag;
	int zIndex;
}block;

block blocks[10][25][5];
block* draggedBlock; //pointer used to access the selected block

bool drag; //is dragging now?
int maxZIndex; //
Vector2 mousePoint = { 0.0f, 0.0f }; //position of mouse cursor

//screen variables
const int screenWidth = 1400; //1250
const int screenHeight = 850; //700
int game_edge_offset_x;
int game_edge_offset_y;
const int offset_x;
const int offset_y;
int frameCounter;

//functions
void InitGame();
void updateGame(); 
void drawGame();

block* GetTopmostBlock(block blocks[10][25][5], Vector2 mousePoint);
Color GetBlockColor(int point);
int findMaxZIndex(block blocks[10][25][5]);


int main() {
	InitWindow(screenWidth, screenHeight, "MAHJONG"); //opening a window
	InitGame();
	SetTargetFPS(60); //Setting FPS 

	while (!WindowShouldClose()) {
		updateGame();
		drawGame();
	}
	CloseWindow();
}

//initializing necessary values
void InitGame() {
	drag = false;
	maxZIndex = 0;
	//setting the game corner 
	game_edge_offset_x = (screenWidth - 1250) / 2;
	game_edge_offset_y = (screenHeight - 700) / 2;

	//MAP SELECTION YAPILACAK
	//read map from text file
	file = fopen("../assets/map1.txt", "r");
	//file = fopen("../assets/map2.txt", "r");

	for (int j = 0; j < 10; j++) {
		for (int k = 0; k < 25; k++) {
			int a = 0;
			fscanf(file, "%d", &a);
			while (a) {
				blocks[j][k][a - 1].point = a;
				blocks[j][k][a - 1].rec.width = (float)(screenWidth / COLUMN);
				blocks[j][k][a - 1].rec.height = (float)(screenHeight / ROW);
				blocks[j][k][a - 1].rec.x = (float)(k * WIDTH + game_edge_offset_x);
				blocks[j][k][a - 1].rec.y = (float)(j * HEIGHT + game_edge_offset_y);
				blocks[j][k][a - 1].drag = false;
				blocks[j][k][a - 1].zIndex = 0;
				a--;
			}
		}
	}
	fclose(file);
}

//update game before each frame is drawn
void updateGame() {
	mousePoint = GetMousePosition();

	maxZIndex = findMaxZIndex(blocks); // Find the maximum zIndex

	if (!drag) {
		block* topmostBlock = GetTopmostBlock(blocks, mousePoint);
		if (topmostBlock != NULL) {
			if (CheckCollisionPointRec(mousePoint, topmostBlock->rec) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				topmostBlock->drag = true;
				drag = true;
				draggedBlock = topmostBlock;  // Set the dragged block

				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
					topmostBlock->drag = true;
					drag = true;
					draggedBlock = topmostBlock;
					maxZIndex = findMaxZIndex(blocks);
					draggedBlock->zIndex = maxZIndex + 1;
				}
			}
		}
	}
	else if (draggedBlock != NULL) {
		// update position of dragged block
		draggedBlock->rec.x = mousePoint.x - WIDTH / 2;
		draggedBlock->rec.y = mousePoint.y - HEIGHT / 2;

		// release the dragged block
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			draggedBlock->drag = false;
			drag = false;
			draggedBlock->zIndex += 1;
			draggedBlock = NULL;  // reset the dragged block

		}
	}
}

//drawing each frame separately
void drawGame() {
	BeginDrawing();

	ClearBackground(MAROON);
	DrawRectangle((screenWidth - 1250) / 2, (screenHeight - 700) / 2, 1250, 700, GRAY);
	DrawText("move ball with mouse and click mouse button to change color", 10, 10, 20, DARKGRAY);

	//drawing rectangles (mahjong tiles)
	for (int zIndex = 0; zIndex <= findMaxZIndex(blocks); zIndex++) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 25; j++) {
				for (int k = 0; k < 5; k++) {
					if (blocks[i][j][k].zIndex == zIndex) {
						Color color = GetBlockColor(blocks[i][j][k].point);
						DrawRectangle(blocks[i][j][k].rec.x, blocks[i][j][k].rec.y, WIDTH, HEIGHT, color);
					}
				}
			}
		}
	}

	// Draw the dragged block last
	if (drag && draggedBlock != NULL) {
		Color color = GetBlockColor(draggedBlock->point);
		DrawRectangle(draggedBlock->rec.x, draggedBlock->rec.y, WIDTH, HEIGHT, color);
	}

	//drawing gridlines 
	for (int i = 0; i < ROW; i++) {
		DrawLine(0 + game_edge_offset_x, i * HEIGHT + game_edge_offset_y, screenWidth - game_edge_offset_x, i * HEIGHT + game_edge_offset_y, COLOR1);
	}
	for (int i = 0; i < COLUMN; i++) {
		DrawLine(i * WIDTH + game_edge_offset_x, game_edge_offset_y, i * WIDTH + game_edge_offset_x, screenHeight - game_edge_offset_y, COLOR1);
	}
	frameCounter++; 
	
	EndDrawing();
}

//color according to points (drawing fonksiyonunda fazladan if/else kurmaktan kacinildi)
Color GetBlockColor(int point) {
	switch (point) {
	case 1: return BLACK;
	case 2: return YELLOW;
	case 3: return GREEN;
	case 4: return BLUE;
	case 5: return RED;;
	}
}

//get the highest layer block OR last dragged block
//eger onceden suruklenilmisse onu secmesini saglar yoksa initial puanina gore en yukaridaki blogu sectirir.
block* GetTopmostBlock(block blocks[10][25][5], Vector2 mousePoint) {
	block* topmostBlock = NULL;
	int highestZIndex = -1;
	int highestPointIndex = -1;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 25; j++) {
			for (int k = 0; k < 5; k++) {
				if (CheckCollisionPointRec(mousePoint, blocks[i][j][k].rec)) {
					// Check if this block has the highest zIndex or the same zIndex but a higher point
					if (blocks[i][j][k].zIndex > highestZIndex ||
						(blocks[i][j][k].zIndex == highestZIndex && blocks[i][j][k].point > highestPointIndex)) {
						highestZIndex = blocks[i][j][k].zIndex;
						highestPointIndex = blocks[i][j][k].point;
						topmostBlock = &blocks[i][j][k];
					}
				}
			}
		}
	}

	return topmostBlock;
}

//get the last dragged blocks 
//update fonksitonunda son suruklenen ogelerin tespitinde kullanildi 
//drawing fonksitonunda son suruklenen ogelerin daha sonra cizilmesi icin kullanildi 
int findMaxZIndex(block blocks[10][25][5]) {
	int maxZ = -1;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 25; j++) {
			for (int k = 0; k < 5; k++) {
				if (blocks[i][j][k].zIndex > maxZ) {
					maxZ = blocks[i][j][k].zIndex;
				}
			}
		}
	}
	return maxZ;
}