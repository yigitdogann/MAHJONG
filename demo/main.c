#include <raylib.h>
#include <stdio.h>
#define ROW 10
#define COLUMN 25

typedef struct block {
	Vector2 point;
	Vector2 size;
	Color color;
	int active;
}block;

const int screenWidth = 1400; //1230
const int screenHeight = 850; //700

void drawGame();
void updateGame();
void InitGame();


FILE *file[2];
block blocks[2][10][25];



int main() {
	InitGame();

	InitWindow(screenWidth, screenHeight, "MAHJONG");
	SetTargetFPS(60);


	while (!WindowShouldClose()) {
		//updateGame();
		drawGame();
	}

	CloseWindow();
}

void drawGame() {

	BeginDrawing();

	ClearBackground(RAYWHITE);
	DrawRectangle(85, 75, 1230, 700, BLACK);
	DrawText("move ball with mouse and click mouse button to change color", 10, 10, 20, DARKGRAY);

	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COLUMN; j++){
			if(blocks[0][i][j].active == 0){
				DrawRectangleV(blocks[0][i][j].point, blocks[0][i][j].size, RED);
			}
		}
	}

	EndDrawing();
}

void UpdateGame() {

}

void InitGame() {
	for (int i = 0; i < 2; i++) {
		file[0] = fopen("C:/Users/Engin Paksoy/source/repos/MAHJONG/assets/map1.txt", "r");//error
		file[1] = fopen("C:/Users/Engin Paksoy/source/repos/MAHJONG/assets/map2.txt", "r");//error
		if (file[i] == NULL) {

		}
		for (int j = 0; j < ROW; j++){
			for (int k = 0; k < COLUMN; k++)
			{
				fscanf(file[i], "%d", &blocks[i][j][k].active);//error
				blocks[i][j][k].point.x = k * 50;
				blocks[i][j][k].point.y = j * 70;
				blocks[i][j][k].size.x = 50;
				blocks[i][j][k].size.y = 70;
			}
		}
	}
}