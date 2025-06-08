#include <iostream>
#include <raylib.h>
#include "globals.h"
#include "game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

Game *Game::instance = NULL;
bool Game::isMobile = false;

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
#ifndef EMSCRIPTEN_BUILD
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    ToggleBorderlessWindowed();
#endif
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL);
    game = Game::GetInstance();
    SetTargetFPS(144);

#ifdef __EMSCRIPTEN__
    // Detect if running on mobile device
    Game::isMobile = EM_ASM_INT({
        return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
    });
    
    // Set up the game loop for Emscripten
    emscripten_set_main_loop(gameLoop, 0, 1);
#else
    // Regular desktop game loop
    if(fullscreen) 
    { 
        ToggleBorderlessWindowed(); 
    }
    while (!exitWindow)
    {
        gameLoop();
    }

    CloseAudioDevice();
    CloseWindow();
#endif

    return 0;
}