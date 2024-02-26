#include <raylib.h>
#include <math.h>
#include <time.h>
#define PLATFORMS 5

int main(void) {
	srand(time(NULL));
	int screenWidth = 1000;
	int screenHeight= 600;

	InitWindow(screenWidth, screenHeight, "Platforms");
	SetTargetFPS(144);

//////PLAYER SETTINGS//////
	Rectangle player = { 0 };
	player.x = 465;
	player.y = 465;
	player.width = 35;
	player.height = 35;

//////PLATFORM SETTINGS//////
	Rectangle floor[PLATFORMS] = {0};
	for (int i = 0; i < PLATFORMS; i++) {
		floor[i].x = (rand() % (screenWidth - 50));
		floor[i].y = (rand() % (screenHeight - 150));
		floor[i].width = 50;
		floor[i].height = 20;
	}
	

	while (!WindowShouldClose()) {

		int elapsedTime = GetTime();
		//double frametime = GetFrameTime();

		if (IsKeyDown(KEY_RIGHT)) {
			player.x += 5;
		}if (IsKeyDown(KEY_LEFT)) {
			player.x -= 5;
		}if (IsKeyDown(KEY_UP)) {
			player.y -= 2.5;
		}
		

		

		BeginDrawing();
			
		DrawText(TextFormat("Total Time Elapsed: %d", elapsedTime), 10, 10, 22, DARKGRAY);
			ClearBackground(LIGHTGRAY);//SKY
			
			//DrawText(TextFormat("frame time test: %f", frametime), 250, 350, 20, RED);
			DrawRectangleRec(player, DARKPURPLE);//PLAYER
			DrawRectangle(0, screenHeight * 5/6, screenWidth, screenHeight*1/6, DARKGRAY);//GROUND

			for (int i = 0; i < PLATFORMS; i++) {//PLATFORMS
				DrawRectangleRec(floor[i], MAROON);

				if (CheckCollisionRecs(player, floor[i])) {//COLLISION
					ClearBackground(RED);
				}
			}
			
			

		EndDrawing();
	}

	CloseWindow();
	return 0;
}