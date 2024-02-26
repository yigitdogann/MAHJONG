#include <raylib.h>

const int screenWidth = 1400; //1230
const int screenHeight = 850; //700

void drawGame();
void updateGame();

typedef struct block {
	Rectangle rect;

}block;

int main() {
	InitWindow(screenWidth, screenHeight, "MAHJONG");
	SetTargetFPS(60);

	block mohjong = { 0 };

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

	EndDrawing();
}

void UpdateGame() {

}