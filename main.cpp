#include <iostream>
#include <raylib.h>
#include <game.h>

using namespace std;

Game* Game::instance = NULL; 

int main()
{
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(144);

    Game* game = Game::GetInstance();
    game->ResetGame();

    float dt = GetFrameTime();

    while (WindowShouldClose() == false)
    {
        game->Update(dt);

        BeginDrawing();
        game->Draw();
        EndDrawing();

        dt = GetFrameTime();
    }

    CloseWindow();
    return 0;
}