#include "game.h"

Game::Game()
{
    firstTimeGameStart = true;
    sndBallBounce = LoadSound("res/ball_bounce.mp3");
    targetRenderTex = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(targetRenderTex.texture, TEXTURE_FILTER_BILINEAR);
    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
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

    bool running = (firstTimeGameStart == false && paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false & oponentScored == false && playerScored == false && levelComplete == false);
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
}

void Game::UpdateUI()
{
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        isInExitMenu = true;
        return;
    }

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

    if (IsKeyPressed(KEY_SPACE))
    {
        if (firstTimeGameStart && IsKeyPressed(KEY_SPACE))
        {
            firstTimeGameStart = false;
        }
        else if (gameOver && IsKeyPressed(KEY_SPACE))
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

    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && isFirstFrameAfterReset == false && IsKeyPressed(KEY_P))
    {
        if (paused)
        {
            paused = false;
        }
        else
        {
            paused = true;
        }
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
    // DrawLine(0, screenHeight/2, screenWidth, screenHeight/2, markingColor);
    Color centerColor = markingColor;
    centerColor.a = 100;
    DrawCircle(gameScreenWidth / 2, gameScreenHeight / 2, 100, centerColor);
    ball.Draw();
    player.Draw();
    oponent.Draw();
    DrawUI();

    EndTextureMode();

    // render the scaled frame texture to the screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(targetRenderTex.texture, (Rectangle){0.0f, 0.0f, (float)targetRenderTex.texture.width, (float)-targetRenderTex.texture.height},
                   (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * screenScale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * screenScale)) * 0.5f, (float)gameScreenWidth * screenScale, (float)gameScreenHeight * screenScale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    DrawScreenSpaceUI();
    EndDrawing();
}

void Game::DrawUI()
{
    DrawText(TextFormat("%i", oponent_score), gameScreenWidth / 4 - 20, 80, 80, oponent.color);
    DrawText(TextFormat("%i", player_score), 3 * gameScreenWidth / 4 - 20, 80, 80, player.color);

    DrawText(TextFormat("Level: %i", level), gameScreenWidth / 2 - 140, 80, 80, WHITE);
}

void Game::DrawScreenSpaceUI()
{
    if (exitWindowRequested)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (firstTimeGameStart)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Press SPACE to play", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 40, yellow);
    }
    else if (paused)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (lostWindowFocus)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameOver)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);

        if (playerWins)
        {
            DrawText("You win! Press SPACE to play again", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }
        else
        {

            DrawText("Game over, press SPACE to play again", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }
    }
    else if (levelComplete)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Level complete! Press SPACE for next level", GetScreenWidth() / 2 - 450, GetScreenHeight() / 2, 40, yellow);
    }
    else if (oponentScored)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Oponent scores! Press SPACE", GetScreenWidth() / 2 - 300, GetScreenHeight() / 2, 40, yellow);
    }
    else if (playerScored)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Player scores! Press SPACE", GetScreenWidth() / 2 - 300, GetScreenHeight() / 2, 40, yellow);
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