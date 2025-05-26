#include "game.h"
#include <math.h>

// Helper function to clamp a value between min and max
static float Clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

Game::Game()
{
    firstTimeGameStart = true;
    musicMuted = false;  // Initialize music as not muted
    sndBallBounce = LoadSound("res/ball_bounce.mp3");
    sndBallBounceWall = LoadSound("res/ball_bounce.mp3");
    backgroundMusic = LoadMusicStream("res/music.mp3");
    SetMusicVolume(backgroundMusic, 0.5f);  // Set volume to 50%
    targetRenderTex = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(targetRenderTex.texture, TEXTURE_FILTER_BILINEAR);
    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    // Initialize mobile controls
    if (isMobile) {
        upButton = { 0.0f + buttonSpacingX, (float)(gameScreenHeight - buttonSize - buttonSpacingY), (float)buttonSize, (float)buttonSize };
        downButton = { (float)gameScreenWidth - buttonSize - buttonSpacingX, (float)(gameScreenHeight - buttonSize - buttonSpacingY), (float)buttonSize, (float)buttonSize };
        upButtonExpanded = { upButton.x - upButton.width * collisionScale, upButton.y - upButton.height * collisionScale, upButton.width * (1.0f + 2.0f * collisionScale), upButton.height * (1.0f + 2.0f * collisionScale) };
        downButtonExpanded = { downButton.x - downButton.width * collisionScale, downButton.y - downButton.height * collisionScale, downButton.width * (1.0f + 2.0f * collisionScale), downButton.height * (1.0f + 2.0f * collisionScale) };
        upButtonPressed = false;
        downButtonPressed = false;
    }

    InitGame();
}

Game::~Game()
{
    UnloadRenderTexture(targetRenderTex);
    UnloadFont(font);
    UnloadSound(sndBallBounce);
    UnloadSound(sndBallBounceWall);
    UnloadMusicStream(backgroundMusic);
    delete instance;
    instance = 0;
}

void Game::InitGame()
{
    isFirstFrameAfterReset = true;
    isInExitMenu = false;
    paused = false;
    lostWindowFocus = false;
    gameOver = false;

    level = 1;

    playerScored = false;
    oponentScored = false;
    playerWins = false;
    levelComplete = false;

    player_score = 0;
    oponent_score = 0;
    numBounces = 0;

    ball.Init();
    player.Init();
    oponent.Init();

    ball.SetSpeed(ballSpeeds[level - 1], ballSpeeds[level - 1]);
}

void Game::Reset()
{
    InitGame();
}

void Game::NextLevel()
{
    level++;
    levelComplete = false;
    playerScored = false;
    oponentScored = false;
    oponent_score = 0;
    player_score = 0;
    numBounces = 0;
    ball.SetSpeed(ballSpeeds[level - 1], ballSpeeds[level - 1]);
    // oponent.SetSpeed(cpuPaddleSpeeds[level-1]);
    ResetObjects();
}

int Game::GetNumBallBounces()
{
    return numBounces;
}

void Game::Update(float dt)
{
    if (dt == 0)
    {
        return;
    }

    UpdateMusicStream(backgroundMusic);  // Update music stream
    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    UpdateUI();

    bool running = (firstTimeGameStart == false && paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false && oponentScored == false && playerScored == false && levelComplete == false);
    if (running)
    {
        HandleInput();

        ball.Update(dt);
        player.Update(dt);
        oponent.Update(dt, ball.y);

        // Check collisions
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            // Calculate the normal vector for the paddle collision
            // For the player paddle (left side), the normal points right
            Vector2 normal = {1.0f, 0.0f};
            ball.Reflect(normal);
            
            numBounces++;
            ball.AddBounceSpeed(ballSpeedBounceIncrement);
            
            // Adjust position to prevent sticking
            if (ball.velocity.y < 0)
            {
                ball.SetPosition(ball.x - (int)(ball.radius * bounceBack), ball.y - (int)(ball.radius * bounceBack));
            }
            else
            {
                ball.SetPosition(ball.x - (int)(ball.radius * bounceBack), ball.y + (int)(ball.radius * bounceBack));
            }
            PlaySound(sndBallBounce);
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{oponent.x, oponent.y, oponent.width, oponent.height}))
        {
            // Calculate the normal vector for the paddle collision
            // For the opponent paddle (right side), the normal points left
            Vector2 normal = {-1.0f, 0.0f};
            ball.Reflect(normal);
            
            numBounces++;
            ball.AddBounceSpeed(ballSpeedBounceIncrement);
            
            // Adjust position to prevent sticking
            if (ball.velocity.y < 0)
            {
                ball.SetPosition(ball.x + (int)(ball.radius * bounceBack), ball.y - (int)(ball.radius * bounceBack));
            }
            else
            {
                ball.SetPosition(ball.x + (int)(ball.radius * bounceBack), ball.y + (int)(ball.radius * bounceBack));
            }
            PlaySound(sndBallBounce);
        }

        if (ball.x >= gameScreenWidth - ball.cRadius)
        {
            oponent_score++;
            oponentScored = true;
        }

        if (ball.x < ball.cRadius)
        {
            player_score++;
            playerScored = true;
        }

        if (oponent_score >= 5)
        {
            gameOver = true;
        }
        else if (player_score >= 5)
        {
            if (level >= maxLevels)
            {
                level = maxLevels;
                gameOver = true;
                playerWins = true;
            }
            else
            {
                levelComplete = true;
            }
        }
    }
}

void Game::HandleInput()
{
    if (isFirstFrameAfterReset)
    {
        isFirstFrameAfterReset = false;
        return;
    }

    if (isMobile) {
        // Handle mobile touch input
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && paused == false) {
            Vector2 touchPos = GetMousePosition();
            
            // Adjust touch position for screen scaling
            touchPos.x = (touchPos.x - (GetScreenWidth() - gameScreenWidth * screenScale) * 0.5f) / screenScale;
            touchPos.y = (touchPos.y - (GetScreenHeight() - gameScreenHeight * screenScale) * 0.5f) / screenScale;

            upButtonExpanded = { upButton.x - upButton.width * collisionScale, upButton.y - upButton.height * collisionScale, upButton.width * (1.0f + 2.0f * collisionScale), upButton.height * (1.0f + 2.0f * collisionScale) };
            downButtonExpanded = { downButton.x - downButton.width * collisionScale, downButton.y - downButton.height * collisionScale, downButton.width * (1.0f + 2.0f * collisionScale), downButton.height * (1.0f + 2.0f * collisionScale) };

            upButtonPressed = CheckCollisionPointRec(touchPos, upButtonExpanded);
            downButtonPressed = CheckCollisionPointRec(touchPos, downButtonExpanded);
            
            if (upButtonPressed) {
                player.y -= player.speed * GetFrameTime();
            }
            if (downButtonPressed) {
                player.y += player.speed * GetFrameTime();
            }
        } else {
            upButtonPressed = false;
            downButtonPressed = false;
        }
    } else {
        // Handle keyboard input for desktop
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            player.y -= player.speed * GetFrameTime();
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            player.y += player.speed * GetFrameTime();
        }
    }
}

void Game::UpdateUI()
{
    bool shouldTogglePause = false;  // Track if we should toggle pause state

#ifndef EMSCRIPTEN_BUILD
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        isInExitMenu = true;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        if (fullscreen)
        {
            fullscreen = false;
            ToggleBorderlessWindowed();
        }
        else
        {
            fullscreen = true;
            ToggleBorderlessWindowed();
        }
    }    
#endif

    // Add music toggle with M key
    if (!firstTimeGameStart && IsKeyPressed(KEY_M)) {
        musicMuted = !musicMuted;
        if (musicMuted) {
            PauseMusicStream(backgroundMusic);
        } else {
            ResumeMusicStream(backgroundMusic);
        }
    }

    // Handle mobile touch input for various game states
    if (isMobile && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 touchPos = GetMousePosition();
        touchPos.x = (touchPos.x - (GetScreenWidth() - gameScreenWidth * screenScale) * 0.5f) / screenScale;
        touchPos.y = (touchPos.y - (GetScreenHeight() - gameScreenHeight * screenScale) * 0.5f) / screenScale;
        
        if (firstTimeGameStart) {
            firstTimeGameStart = false;
            PlayMusicStream(backgroundMusic);  // Start playing music after welcome screen
        }
        else if (gameOver) {
            Reset();
        }
        else if (levelComplete) {
            NextLevel();
        }
        else if (playerScored || oponentScored) {
            playerScored = false;
            oponentScored = false;
            ResetObjects();
        }
        else if (!CheckCollisionPointRec(touchPos, upButtonExpanded) && !CheckCollisionPointRec(touchPos, downButtonExpanded)) {
            // 
            shouldTogglePause = true;
        }
        else if(paused) {
            paused = false;
        }
    }

    // Handle keyboard input for non-mobile
    if (!isMobile) {
        if (IsKeyPressed(KEY_ENTER)) {
            if (firstTimeGameStart) {
                firstTimeGameStart = false;
                PlayMusicStream(backgroundMusic);  // Start playing music after welcome screen
            }
            else if (gameOver) {
                Reset();
            }
            else if (levelComplete) {
                NextLevel();
            }
            else if (playerScored || oponentScored) {
                playerScored = false;
                oponentScored = false;
                ResetObjects();
            }
            return;
        }

        if (exitWindowRequested) {
            if (IsKeyPressed(KEY_Y)) {
                exitWindow = true;
            }
            else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                exitWindowRequested = false;
                isInExitMenu = false;
            }
        }

        if (IsWindowFocused() == false) {
            lostWindowFocus = true;
        }
        else {
            lostWindowFocus = false;
        }

        // Check for keyboard pause input
#ifndef EMSCRIPTEN_BUILD
        if (!firstTimeGameStart && !gameOver && !levelComplete && !playerScored && !oponentScored && IsKeyPressed(KEY_P)) {
            shouldTogglePause = true;
        }
#else
        // Handle pause input for web builds (non mobile)
        if (!firstTimeGameStart && !gameOver && !levelComplete && !playerScored && !oponentScored && (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))) {
            shouldTogglePause = true;
        }
#endif
    }

    if (shouldTogglePause && !firstTimeGameStart && !gameOver && !levelComplete && !playerScored && !oponentScored) {
        paused = !paused;
    }
}

void Game::Draw()
{
    // render everything to a texture
    BeginTextureMode(targetRenderTex);
    ClearBackground(boardColor);

    //Color centerLineColor = markingColor;
    //centerLineColor.a = 90;
    //DrawLine(gameScreenWidth / 2, 0, gameScreenWidth / 2, gameScreenHeight, centerLineColor);
    Color centerColor = markingColor;
    centerColor.a = 100;
    DrawCircle(gameScreenWidth / 2, gameScreenHeight / 2, 100, centerColor);


    int lineThickness = 6;
    // Border
    Color lineColor = BLACK;
    lineColor.a = 128;
    float padding = 1;
    DrawRectangleLinesEx((Rectangle){padding, padding, gameScreenWidth - padding * 2, gameScreenHeight - padding * 2}, lineThickness, lineColor);
    // Vertical center line
    DrawRectangle(gameScreenWidth / 2 - lineThickness / 2, padding, lineThickness, gameScreenHeight - padding * 2, lineColor);
    // Horizontal center line
    DrawRectangle(padding, gameScreenHeight / 2 - lineThickness / 2, gameScreenWidth - padding * 2, lineThickness, lineColor);



    ball.Draw();
    player.Draw();
    oponent.Draw();
    DrawUI();

    // Draw mobile controls if on mobile
    if (isMobile) {
        // Position buttons at bottom relative to the center of the screen;
        upButton = { 0.0f + buttonSpacingX, (float)(gameScreenHeight - buttonSize - buttonSpacingY), (float)buttonSize, (float)buttonSize };
        downButton = { (float)gameScreenWidth - buttonSize - buttonSpacingX, (float)(gameScreenHeight - buttonSize - buttonSpacingY), (float)buttonSize, (float)buttonSize };
        
        Vector2 upButtonCenter = { upButton.x + upButton.width/2, upButton.y + upButton.height/2 };
        Vector2 downButtonCenter = { downButton.x + downButton.width/2, downButton.y + downButton.height/2 };
        
        // Draw up button
        if (upButtonPressed) {
            DrawCircle(upButtonCenter.x, upButtonCenter.y, buttonSize/2, RED);
        } else {
            DrawCircle(upButtonCenter.x, upButtonCenter.y, buttonSize/2, WHITE);
        }
        
        // Draw up arrow triangle
        float arrowSize = buttonSize/3;
        Vector2 upArrowTop = { upButtonCenter.x, upButtonCenter.y - arrowSize };
        Vector2 upArrowLeft = { upButtonCenter.x - arrowSize, upButtonCenter.y + arrowSize/2 };
        Vector2 upArrowRight = { upButtonCenter.x + arrowSize, upButtonCenter.y + arrowSize/2 };
        DrawTriangle(upArrowTop, upArrowLeft, upArrowRight, BLACK);
        
        // Draw down button
        if (downButtonPressed) {
            DrawCircle(downButtonCenter.x, downButtonCenter.y, buttonSize/2, RED);
        } else {
            DrawCircle(downButtonCenter.x, downButtonCenter.y, buttonSize/2, WHITE);
        }
        
        // Draw down arrow triangle
        Vector2 downArrowBottom = { downButtonCenter.x, downButtonCenter.y + arrowSize };
        Vector2 downArrowLeft = { downButtonCenter.x - arrowSize, downButtonCenter.y - arrowSize/2 };
        Vector2 downArrowRight = { downButtonCenter.x + arrowSize, downButtonCenter.y - arrowSize/2 };
        DrawTriangle(downArrowRight, downArrowLeft, downArrowBottom, BLACK);
    }

    EndTextureMode();

    // render the scaled frame texture to the screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(targetRenderTex.texture, (Rectangle){0.0f, 0.0f, (float)targetRenderTex.texture.width, (float)-targetRenderTex.texture.height},
                   (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * screenScale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * screenScale)) * 0.5f, (float)gameScreenWidth * screenScale, (float)gameScreenHeight * screenScale},
                   (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
}

void Game::DrawUI()
{
    // Draw semi-transparent background for all UI elements
    Color uiBgColor = BLACK;
    uiBgColor.a = 64;  // Semi-transparent black
    
    // Single background rectangle covering all UI elements
    DrawRectangleRounded({(float)(gameScreenWidth / 4 - 60), 60, (float)(gameScreenWidth / 2 + 120), 100}, 0.2f, 10, uiBgColor);

    DrawText(TextFormat("%i", oponent_score), gameScreenWidth / 4 - 20, 80, 80, oponent.color);
    DrawText(TextFormat("%i", player_score), 3 * gameScreenWidth / 4 - 20, 80, 80, player.color);
    DrawText(TextFormat("Level: %i", level), gameScreenWidth / 2 - 140, 80, 80, WHITE);

    // Draw music toggle status at the bottom of the screen
    if(!isMobile) {
        const char* musicStatus = musicMuted ? "Press M to toggle music (OFF)" : "Press M to toggle music (ON)";
        int textWidth = MeasureText(musicStatus, 32);
        DrawText(musicStatus, gameScreenWidth / 2 - textWidth / 2, gameScreenHeight - 40, 32, WHITE);
    }

    if (exitWindowRequested)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
    }
    else if (firstTimeGameStart)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 200), 1000, 400}, 0.76f, 20, BLACK);
        
        if (isMobile) {
            DrawText("Welcome to Pong!", gameScreenWidth / 2 - 200, gameScreenHeight / 2 - 150, 50, yellow);
            DrawText("Controls:", gameScreenWidth / 2 - 100, gameScreenHeight / 2 - 80, 40, WHITE);
            DrawText("Use the up/down buttons at the bottom", gameScreenWidth / 2 - 400, gameScreenHeight / 2 - 20, 30, WHITE);
            DrawText("Tap the top half of the screen to pause", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 20, 30, WHITE);
            DrawText("Tap to start", gameScreenWidth / 2 - 200, gameScreenHeight / 2 + 100, 40, yellow);
        } else {
            DrawText("Welcome to Pong!", gameScreenWidth / 2 - 200, gameScreenHeight / 2 - 150, 50, yellow);
            DrawText("Controls:", gameScreenWidth / 2 - 100, gameScreenHeight / 2 - 80, 40, WHITE);
            DrawText("W/S or Up/Down Arrow Keys: Move paddle", gameScreenWidth / 2 - 400, gameScreenHeight / 2 - 20, 30, WHITE);
#ifndef EMSCRIPTEN_BUILD
            DrawText("P: Pause game", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 20, 30, WHITE);
#else
            DrawText("P or ESC: Pause game", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 20, 30, WHITE);
#endif
#ifndef EMSCRIPTEN_BUILD
            DrawText("ESC: Exit game", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 60, 30, WHITE);
            DrawText("ALT+ENTER: Toggle fullscreen", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 100, 30, WHITE);
#endif
            DrawText("Press ENTER to start", gameScreenWidth / 2 - 200, gameScreenHeight / 2 + 140, 40, yellow);
        }
    }
    else if (paused)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
#ifndef EMSCRIPTEN_BUILD    
        DrawText("Game paused, press P to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
#else
        if (isMobile) {
            DrawText("Game paused, tap to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
        } else {
            DrawText("Game paused, press P or ESC to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
        }
#endif
    }
    else if (lostWindowFocus)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game paused, focus window to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
    }
    else if (gameOver)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);

        if (playerWins)
        {
            if (isMobile) {
                DrawText("You win! Tap to play again", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
            } else {
                DrawText("You win! Press ENTER to play again", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
            }
        }
        else
        {
            if (isMobile) {
                DrawText("Game over, tap to play again", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
            } else {
                DrawText("Game over, press ENTER to play again", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
            }
        }
    }
    else if (levelComplete)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        if (isMobile) {
            DrawText("Level complete! Tap to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
        } else {
            DrawText("Level complete! Press ENTER for next level", gameScreenWidth / 2 - 450, gameScreenHeight / 2, 40, yellow);
        }
    }
    else if (oponentScored)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        if (isMobile) {
            DrawText("Oponent scores! Tap to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
        } else {
            DrawText("Oponent scores! Press ENTER", gameScreenWidth / 2 - 300, gameScreenHeight / 2, 40, yellow);
        }
    }
    else if (playerScored)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        if (isMobile) {
            DrawText("Player scores! Tap to continue", gameScreenWidth / 2 - 400, gameScreenHeight / 2, 40, yellow);
        } else {
            DrawText("Player scores! Press ENTER", gameScreenWidth / 2 - 300, gameScreenHeight / 2, 40, yellow);
        }
    }
}

std::string Game::FormatWithLeadingZeroes(int number, int width)
{
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    numberText = std::string(leadingZeros, '0') + numberText;
    return numberText;
}

void Game::ResetObjects()
{
    numBounces = 0;
    ball.SetSpeed(ballSpeeds[level - 1], ballSpeeds[level - 1]);
    ball.ResetBall();
    player.ResetPosition();
    oponent.ResetPosition();
}