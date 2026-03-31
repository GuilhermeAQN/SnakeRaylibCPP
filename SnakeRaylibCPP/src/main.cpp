#include "raylib.h"
#include <iostream>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTrigger(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    Vector2 directionBeforeUpdate = { 1, 0 };
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = { offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        direction = directionBeforeUpdate;
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }
    
    void Reset()
    {
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = { 1, 0 };
    }
};

// Food class definition
class Food
{
public:
    Vector2 position;   // Position of the food on the grid
    Texture2D texture;  // Texture for the food image

    // Constructor: loads the food image and sets a random position
    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GeneratorRandomPos(snakeBody);
    }

    // Destructor: unloads the texture to free resources
    ~Food()
    {
        UnloadTexture(texture);
    }

    // Draws the food on the screen
    void Draw()
    {
        DrawTexture(texture, offset + (int)position.x * cellSize, offset + (int)position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x, y };
    }

    // Generates a random position for the food within the grid
    Vector2 GeneratorRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.mp3");
        wallSound = LoadSound("sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GeneratorRandomPos(snake.body);
            snake.addSegment = true;
            score ++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GeneratorRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    std::cout << "Starting the game..." << std::endl;

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "RetroSnake");
    SetTargetFPS(60); 

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTrigger(0.2))
        {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.directionBeforeUpdate = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.directionBeforeUpdate = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.directionBeforeUpdate = { -1, 0 };
            game.running = true;
        }
        if (IsKeyDown(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.directionBeforeUpdate = { 1, 0 };
            game.running = true;
        }

        ClearBackground(green);

        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * (float)cellCount + 10, (float)cellSize * (float)cellCount + 10 }, 5, darkGreen);

        DrawText("Retro Snake Raylib", offset - 5, 20, 40, darkGreen);
        int textWidth = MeasureText(TextFormat("%04i", game.score), 40);
        DrawText(TextFormat("%04i", game.score), offset + cellSize * cellCount + 5 - textWidth, 20, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}