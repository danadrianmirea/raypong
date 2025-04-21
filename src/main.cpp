#include <iostream>
#include <raylib.h>
#include "globals.h"
#include "game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

Game *Game::instance = NULL;

// Global game instance
Game *game = nullptr;

// Game loop function
void gameLoop() {
    float dt = GetFrameTime();
    game->Update(dt);
    game->Draw();
}

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Pong");
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL);

    game = Game::GetInstance();
    ToggleBorderlessWindowed();
    SetTargetFPS(144);

#ifdef __EMSCRIPTEN__
    // Set up the game loop for Emscripten
    emscripten_set_main_loop(gameLoop, 0, 1);
#else
    // Regular desktop game loop
    while (!exitWindow)
    {
        gameLoop();
    }

    CloseAudioDevice();
    CloseWindow();
#endif

    return 0;
}