#include <iostream>
#include <raylib.h>
#include "globals.h"
#include <game.h>

using namespace std;

Game *Game::instance = NULL;

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Pong");
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL);

    Game *game = Game::GetInstance();
    ToggleBorderlessWindowed();
    SetTargetFPS(144);

    float dt = 0.0f;

    while (!exitWindow)
    {
        dt = GetFrameTime();
        game->Update(dt);
        game->Draw();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}