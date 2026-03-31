#include "raylib.h"  // Main Raylib library for graphics, input, audio, etc.
#include <iostream>  // Standard C++ library for console output (cout)
#include <deque>     // Double-ended queue container — allows push/pop from both front and back
#include <raymath.h> // Raylib math utilities — Vector2 operations like Vector2Add, Vector2Equals

using namespace std; // Allows using standard library names without the "std::" prefix

// --- Custom Colors (RGBA format: Red, Green, Blue, Alpha) ---
Color green = { 173, 204, 96,  255 }; // Background color
Color darkGreen = { 43,  51,  24,  255 }; // Snake, borders and text color

// --- Grid Configuration ---
int cellSize = 30; // Size of each grid cell in pixels
int cellCount = 25; // Number of cells per row/column (grid is 25x25)
int offset = 75; // Margin around the grid in pixels (space for title and borders)

// Tracks the last time the game state was updated (used for movement timing)
double lastUpdateTime = 0;

// --- Helper Function: Check if a Vector2 exists inside a deque ---
// Used to prevent food from spawning on the snake's body
// Parameters:
//   element — the position to search for
//   deque   — the list of positions to search in
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        // Vector2Equals checks if two Vector2 positions are identical
        if (Vector2Equals(deque[i], element))
        {
            return true; // Found a match
        }
    }
    return false; // No match found
}

// --- Helper Function: Time-based event trigger ---
// Returns true only when the given interval (in seconds) has passed
// This controls how fast the snake moves, independent of FPS
bool eventTrigger(double interval)
{
    double currentTime = GetTime(); // Get elapsed time since InitWindow()
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime; // Reset the timer
        return true;
    }
    return false;
}

// =============================================================================
// CLASS: Snake
// Represents the player-controlled snake
// =============================================================================
class Snake
{
public:
    // The snake body is a deque of Vector2 grid positions
    // Front = head, back = tail
    // Starts with 3 segments at positions (6,9), (5,9), (4,9)
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };

    Vector2 direction = { 1, 0 }; // Current movement direction (starts moving right)
    Vector2 directionBeforeUpdate = { 1, 0 }; // Buffered direction (set by input, applied on update)
    bool addSegment = false;                   // Flag: if true, the snake grows by one segment

    // Draws each segment of the snake as a rounded rectangle
    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            // Convert grid position to screen pixels, applying the offset margin
            // DrawRectangleRounded: draws a filled rectangle with rounded corners
            // Parameters: rectangle, roundness (0-1), segments (smoothness), color
            Rectangle segment = { offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    // Moves the snake forward by one cell
    void Update()
    {
        // Add a new head in the current movement direction
        // Vector2Add sums two Vector2 values (head position + direction)
        body.push_front(Vector2Add(body[0], direction));

        // Apply the buffered direction after moving (prevents 180° turns mid-frame)
        direction = directionBeforeUpdate;

        if (addSegment == true)
        {
            // If flagged to grow, skip removing the tail (net gain: +1 segment)
            addSegment = false;
        }
        else
        {
            // Remove the last segment to maintain the snake's current length
            body.pop_back();
        }
    }

    // Resets the snake to its original position and direction
    void Reset()
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

// =============================================================================
// CLASS: Food
// Represents the food item the snake must eat to grow
// =============================================================================
class Food
{
public:
    Vector2   position; // Food's current position on the grid
    Texture2D texture;  // GPU texture loaded from the food image file

    // Constructor: loads the food texture and picks a valid random position
    // Receives the snake body to avoid spawning food on top of the snake
    Food(deque<Vector2> snakeBody)
    {
        // Load image into CPU memory first, then send it to the GPU as a texture
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image); // Free CPU image data — GPU texture is now sufficient

        position = GeneratorRandomPos(snakeBody);
    }

    // Destructor: frees the GPU texture memory when the Food object is destroyed
    ~Food()
    {
        UnloadTexture(texture);
    }

    // Draws the food texture at its grid position
    // Casts position.x and position.y to int — DrawTexture requires int coordinates
    void Draw()
    {
        DrawTexture(texture, offset + (int)position.x * cellSize, offset + (int)position.y * cellSize, WHITE);
    }

    // Returns a random grid cell as a Vector2
    // GetRandomValue returns an int, cast to float to match Vector2's float fields
    Vector2 GenerateRandomCell()
    {
        int x = GetRandomValue(0, cellCount - 1);
        int y = GetRandomValue(0, cellCount - 1);
        return Vector2{ (float)x, (float)y };
    }

    // Keeps generating random positions until one is not occupied by the snake
    Vector2 GeneratorRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell(); // Try again if position is taken
        }
        return position;
    }
};

// =============================================================================
// CLASS: Game
// Central controller — manages the snake, food, score, audio and game state
// =============================================================================
class Game
{
public:
    Snake snake = Snake();           // The player's snake
    Food  food = Food(snake.body);  // Food item, initialized avoiding the snake body
    bool  running = true;            // Controls whether the game is active or paused after death
    int   score = 0;              // Player's current score
    Sound eatSound;                  // Sound played when food is eaten
    Sound wallSound;                 // Sound played when the snake hits a wall or itself

    // Constructor: initializes the audio device and loads sound files
    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.mp3");
        wallSound = LoadSound("sounds/wall.mp3");
    }

    // Destructor: unloads sounds from memory when the Game object is destroyed
    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
    }

    // Calls Draw() on all game objects
    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    // Runs all game logic — only processes if the game is active
    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithEdges();
            CheckCollisionWithFood();
            CheckCollisionWithTail();
        }
    }

    // Checks if the snake's head reached the food position
    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GeneratorRandomPos(snake.body); // Reposition food
            snake.addSegment = true; // Flag the snake to grow next update
            score++;
            PlaySound(eatSound);
        }
    }

    // Checks if the snake's head went outside the grid boundaries
    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount)
        {
            snake.body[0].x = 0;
        }
        if (snake.body[0].x == -1)
        {
            snake.body[0].x = (float)cellCount - 1;
        }
        if (snake.body[0].y == cellCount)
        {
            snake.body[0].y = 0;
        }
        if (snake.body[0].y == -1)
        {
            snake.body[0].y = (float)cellCount - 1;
        }
    }

    // Handles the game over state: resets everything and pauses the game
    void GameOver()
    {
        snake.Reset();
        food.position = food.GeneratorRandomPos(snake.body);
        running = false; // Pause the game until the player presses a direction key
        score = 0;
        PlaySound(wallSound);
    }

    // Checks if the snake's head collided with any part of its own body
    void CheckCollisionWithTail()
    {
        // Create a copy of the body without the head to check self-collision
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front(); // Remove head from the copy

        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

// =============================================================================
// MAIN — Entry point of the program
// Handles window creation, game loop, input and rendering
// =============================================================================
int main()
{
    std::cout << "Starting the game..." << std::endl;

    // Create the window — total size accounts for the grid + offset on each side
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "RetroSnake");
    SetTargetFPS(60); // Cap the frame rate at 60 frames per second

    Game game = Game();

    // --- Game Loop ---
    // Runs every frame until the window is closed (X button or ALT+F4)
    while (WindowShouldClose() == false)
    {
        BeginDrawing(); // Begin the frame — everything drawn after this appears on screen

        // eventTrigger(0.2) returns true every 0.2 seconds (5 times per second)
        // This separates the snake's movement speed from the render framerate
        if (eventTrigger(0.1))
        {
            game.Update();
        }

        // --- Input Handling ---
        // Prevents the snake from reversing direction (can't go opposite to current direction)
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.directionBeforeUpdate = { 0, -1 };
            game.running = true; // Resume game if it was paused after game over
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
        if (IsKeyDown(KEY_RIGHT) && game.snake.direction.x != -1) // IsKeyDown allows holding the key
        {
            game.snake.directionBeforeUpdate = { 1, 0 };
            game.running = true;
        }

        // --- Rendering ---
        ClearBackground(green); // Fill the screen with the background color

        // Draw the border rectangle around the game grid
        // DrawRectangleLinesEx draws only the outline of a rectangle
        DrawRectangleLinesEx(
            Rectangle{ (float)offset - 5, (float)offset - 5,
                       (float)cellSize * (float)cellCount + 10,
                       (float)cellSize * (float)cellCount + 10 }, 5, darkGreen);

        // Draw the game title in the top-left area
        DrawText("Retro Snake Raylib", offset - 5, 20, 40, darkGreen);

        // Draw the score right-aligned to the grid's right edge
        // MeasureText returns the width of the text in pixels at the given font size
        // Subtracting it from the right edge position aligns the text to the right
        int textWidth = MeasureText(TextFormat("%04i", game.score), 40);
        DrawText(TextFormat("%04i", game.score),
            offset + cellSize * cellCount + 5 - textWidth, // Right-aligned X position
            20,       // Y position (same height as the title)
            40,       // Font size
            darkGreen
        );

        game.Draw(); // Draw snake and food

        EndDrawing(); // End the frame — swaps buffers and displays the result on screen
    }

    CloseWindow(); // Destroy the window and free Raylib resources
    return 0;      // Return 0 indicates the program finished successfully
}