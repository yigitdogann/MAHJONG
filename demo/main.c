#include <raylib.h>
#include <stdio.h>

#define ROW 10
#define COLUMN 25
#define WIDTH 50
#define HEIGHT 70
#define COLOR1 (Color){ 0, 0, 0, 40} 

FILE* file;

typedef struct block {
	Rectangle rec;
	Color color;
	int active;
	int point;
	bool drag;
}block;

block blocks[10][25][5];
block* draggedBlock;
block* GetTopmostBlock(block blocks[10][25][5], Vector2 mousePoint);
bool drag;
const int screenWidth = 1400; //1250
const int screenHeight = 850; //700
Color GetBlockColor(int point);
Vector2 mousePoint = { 0.0f, 0.0f };

int game_edge_offset_x;
int game_edge_offset_y;
int offset_x;
int offset_y;

void updateGame();
void drawGame();
void InitGame();

int main() {
	InitWindow(screenWidth, screenHeight, "MAHJONG");
	InitGame();
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		updateGame();
		drawGame();
	}
	CloseWindow();
}

void drawGame() {
	BeginDrawing();

	ClearBackground(MAROON);
	DrawRectangle((screenWidth - 1250) / 2, (screenHeight - 700) / 2, 1250, 700, GRAY);
	DrawText("move ball with mouse and click mouse button to change color", 10, 10, 20, DARKGRAY);

	//drawing rectangles (mahjong tiles)
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 25; j++) {
			for (int k = 0; k < 5; k++) {
				if (!blocks[i][j][k].drag) {
					Color color = GetBlockColor(blocks[i][j][k].point);
					DrawRectangle(blocks[i][j][k].rec.x, blocks[i][j][k].rec.y, WIDTH, HEIGHT, color);
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

	EndDrawing();
}


void updateGame() {
    mousePoint = GetMousePosition();
    
    if (!drag) {
        block* topmostBlock = GetTopmostBlock(blocks, mousePoint);
        if (topmostBlock != NULL) {
            if (CheckCollisionPointRec(mousePoint, topmostBlock->rec) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                topmostBlock->drag = true;
                drag = true;
                draggedBlock = topmostBlock;  // Set the dragged block
            }
        }
    } else if (draggedBlock != NULL) {
        // update position of dragged block
        draggedBlock->rec.x = mousePoint.x - WIDTH / 2;
        draggedBlock->rec.y = mousePoint.y - HEIGHT / 2;

        // release the dragged block
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            draggedBlock->drag = false;
            drag = false;
            draggedBlock = NULL;  // reset the dragged block
        }
    }
}

//get color by point 
Color GetBlockColor(int point) {
	switch (point){
	case 1: return BLACK;
	case 2: return YELLOW;
	case 3: return GREEN;
	case 4: return BLUE;
	case 5: return RED;
	}
}

block* GetTopmostBlock(block blocks[10][25][5], Vector2 mousePoint) {
	block* topmostBlock = NULL;
	int highestZIndex = -1;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 25; j++) {
			for (int k = 0; k < 5; k++) {
				if (CheckCollisionPointRec(mousePoint, blocks[i][j][k].rec)) {
					if (blocks[i][j][k].point > highestZIndex) {
						highestZIndex = blocks[i][j][k].point;
						topmostBlock = &blocks[i][j][k];
					}
				}
			}
		}
	}

	return topmostBlock;
}

void InitGame() {
	drag = false;
	//setting the game corner 
	game_edge_offset_x = (screenWidth - 1250) / 2;
	game_edge_offset_y = (screenHeight - 700) / 2;

	//read map from text file
	file = fopen("../assets/map1.txt", "r");
	//file = fopen("../assets/map2.txt", "r");
	
	for (int j = 0; j < 10; j++) {
		for (int k = 0; k < 25; k++) {
			int a = 0;
			fscanf(file, "%d", &a);
			while(a){
				blocks[j][k][a].point = a;				
				blocks[j][k][a].rec.width = (float)(screenWidth / COLUMN);
				blocks[j][k][a].rec.height = (float)(screenHeight / ROW);
				blocks[j][k][a].rec.x = (float)(k * WIDTH + game_edge_offset_x);
				blocks[j][k][a].rec.y = (float)(j * HEIGHT + game_edge_offset_y);
				blocks[j][k][a].drag = false;
				a--;
			}
		}
	}
}