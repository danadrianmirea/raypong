#include "game.h"
#include <math.h>

Game::Game()
{
    firstTimeGameStart = true;
    sndBallBounce = LoadSound("res/ball_bounce.mp3");
    targetRenderTex = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(targetRenderTex.texture, TEXTURE_FILTER_BILINEAR);
    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    // Initialize mobile controls
    if (isMobile) {
        // Position buttons at bottom center
        int totalHeight = 2 * buttonSize + buttonSpacing;
        int startY = gameScreenHeight - totalHeight - buttonSpacing;
        int centerX = gameScreenWidth / 2 - buttonSize / 2;
        
        upButton = { (float)centerX, (float)startY, (float)buttonSize, (float)buttonSize };
        downButton = { (float)centerX, (float)(startY + buttonSize + buttonSpacing), (float)buttonSize, (float)buttonSize };
        
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

    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

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
            ball.speed_x *= -1;
            numBounces++;
            ball.AddBounceSpeed(ballSpeedBounceIncrement);
            if (ball.speed_y < 0)
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
            ball.speed_x *= -1;
            numBounces++;
            ball.AddBounceSpeed(ballSpeedBounceIncrement);
            if (ball.speed_y < 0)
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
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 touchPos = GetMousePosition();
            
            // Adjust touch position for screen scaling
            touchPos.x = (touchPos.x - (GetScreenWidth() - gameScreenWidth * screenScale) * 0.5f) / screenScale;
            touchPos.y = (touchPos.y - (GetScreenHeight() - gameScreenHeight * screenScale) * 0.5f) / screenScale;
            
            // Check if touch is outside the buttons (for pause)
            if (!CheckCollisionPointRec(touchPos, upButton) && !CheckCollisionPointRec(touchPos, downButton)) {
                paused = !paused;
            }
            
            upButtonPressed = CheckCollisionPointRec(touchPos, upButton);
            downButtonPressed = CheckCollisionPointRec(touchPos, downButton);
            
            if (upButtonPressed) {
                player.y -= player.speed * GetFrameTime();
            }
            if (downButtonPressed) {
                player.y += player.speed * GetFrameTime();
            }
        } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 touchPos = GetMousePosition();
            
            // Adjust touch position for screen scaling
            touchPos.x = (touchPos.x - (GetScreenWidth() - gameScreenWidth * screenScale) * 0.5f) / screenScale;
            touchPos.y = (touchPos.y - (GetScreenHeight() - gameScreenHeight * screenScale) * 0.5f) / screenScale;
            
            upButtonPressed = CheckCollisionPointRec(touchPos, upButton);
            downButtonPressed = CheckCollisionPointRec(touchPos, downButton);
            
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
#ifndef EMSCRIPTEN_BUILD
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        isInExitMenu = true;
        return;
    }
#endif

#ifdef AM_RAY_DEBUG
    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        if (fullscreen)
        {
            fullscreen = false;
            ToggleBorderlessWindowed();
            SetWindowPosition(minimizeOffset, minimizeOffset);
        }
        else
        {
            fullscreen = true;
            ToggleBorderlessWindowed();
        }
    }
#endif

    if (firstTimeGameStart) {
        if (isMobile) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                firstTimeGameStart = false;
            }
        } else if (IsKeyPressed(KEY_ENTER)) {
            firstTimeGameStart = false;
        }
        return;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            Reset();
        }
        else if (levelComplete)
        {
            NextLevel();
        }
        else if (playerScored || oponentScored)
        {
            playerScored = false;
            oponentScored = false;
            ResetObjects();
        }
        return;
    }

    if (exitWindowRequested)
    {
        if (IsKeyPressed(KEY_Y))
        {
            exitWindow = true;
        }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE))
        {
            exitWindowRequested = false;
            isInExitMenu = false;
        }
    }

    if (IsWindowFocused() == false)
    {
        lostWindowFocus = true;
    }
    else
    {
        lostWindowFocus = false;
    }

#ifndef EMSCRIPTEN_BUILD
    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && isFirstFrameAfterReset == false && IsKeyPressed(KEY_P))
    {
        paused = !paused;
    }
#else
    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && isFirstFrameAfterReset == false)
    {
        if (isMobile) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 touchPos = GetMousePosition();
                touchPos.x = (touchPos.x - (GetScreenWidth() - gameScreenWidth * screenScale) * 0.5f) / screenScale;
                touchPos.y = (touchPos.y - (GetScreenHeight() - gameScreenHeight * screenScale) * 0.5f) / screenScale;
                
                if (!CheckCollisionPointRec(touchPos, upButton) && !CheckCollisionPointRec(touchPos, downButton)) {
                    paused = !paused;
                }
            }
        } else if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
            paused = !paused;
        }
    }
#endif

    // Handle game over restart for mobile
    if (gameOver && isMobile && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Reset();
    }
}

void Game::Draw()
{
    // render everything to a texture
    BeginTextureMode(targetRenderTex);
    ClearBackground(boardColor);

    Color centerLineColor = markingColor;
    centerLineColor.a = 90;
    DrawLine(gameScreenWidth / 2, 0, gameScreenWidth / 2, gameScreenHeight, centerLineColor);
    Color centerColor = markingColor;
    centerColor.a = 100;
    DrawCircle(gameScreenWidth / 2, gameScreenHeight / 2, 100, centerColor);
    ball.Draw();
    player.Draw();
    oponent.Draw();
    DrawUI();

    // Draw mobile controls if on mobile
    if (isMobile) {
        // Calculate button centers
        Vector2 upButtonCenter = { upButton.x + buttonSize/2, upButton.y + buttonSize/2 };
        Vector2 downButtonCenter = { downButton.x + buttonSize/2, downButton.y + buttonSize/2 };
        
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
    DrawText(TextFormat("%i", oponent_score), gameScreenWidth / 4 - 20, 80, 80, oponent.color);
    DrawText(TextFormat("%i", player_score), 3 * gameScreenWidth / 4 - 20, 80, 80, player.color);

    DrawText(TextFormat("Level: %i", level), gameScreenWidth / 2 - 140, 80, 80, WHITE);

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
            DrawText("Tap the screen to pause", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 20, 30, WHITE);
            DrawText("Tap to start", gameScreenWidth / 2 - 200, gameScreenHeight / 2 + 100, 40, yellow);
        } else {
            DrawText("Welcome to Pong!", gameScreenWidth / 2 - 200, gameScreenHeight / 2 - 150, 50, yellow);
            DrawText("Controls:", gameScreenWidth / 2 - 100, gameScreenHeight / 2 - 80, 40, WHITE);
            DrawText("W/S or Up/Down Arrow Keys: Move paddle", gameScreenWidth / 2 - 400, gameScreenHeight / 2 - 20, 30, WHITE);
            DrawText("P: Pause game", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 20, 30, WHITE);
            DrawText("ESC: Exit game", gameScreenWidth / 2 - 400, gameScreenHeight / 2 + 60, 30, WHITE);
            DrawText("Press ENTER to start", gameScreenWidth / 2 - 200, gameScreenHeight / 2 + 100, 40, yellow);
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
        DrawText("Level complete! Press ENTER for next level", gameScreenWidth / 2 - 450, gameScreenHeight / 2, 40, yellow);
    }
    else if (oponentScored)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Oponent scores! Press ENTER", gameScreenWidth / 2 - 300, gameScreenHeight / 2, 40, yellow);
    }
    else if (playerScored)
    {
        DrawRectangleRounded({(float)(gameScreenWidth / 2 - 500), (float)(gameScreenHeight / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Player scores! Press ENTER", gameScreenWidth / 2 - 300, gameScreenHeight / 2, 40, yellow);
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