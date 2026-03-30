#include "raylib.h"
#include <iostream>
#include <deque>

using namespace std;

Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

int cellSize = 30;
int cellCount = 25;

class Snake
{
public:
	deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };

	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			int x = body[i].x;
			int y = body[i].y;

			DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, darkGreen);
		}
	}
};

class Food
{
public:
	Vector2 position;
	Texture2D texture;

	Food() // Constructor
	{
		Image image = LoadImage("graphics/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GeneratorRandomPos();
	}

	~Food() // Destructor
	{
		UnloadTexture(texture);
	}

	void Draw()
	{
		DrawTexture(texture, (int)position.x * cellSize, (int)position.y * cellSize, WHITE);
	}

	Vector2 GeneratorRandomPos()
	{
		int x = GetRandomValue(0, cellCount - 1);
		int y = GetRandomValue(0, cellCount - 1);
		return Vector2{ (float)x, (float)y };
	}
};

int main()
{
	std::cout << "Starting the game..." << std::endl;
	InitWindow(cellSize * cellCount, cellSize * cellCount, "RetroSnake");
	SetTargetFPS(60);

	Food food = Food();
	Snake snake = Snake();

	while (WindowShouldClose() == false)
	{
		BeginDrawing();

		// Set background color
		ClearBackground(green);
		food.Draw();
		snake.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}