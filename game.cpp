#include "game.h"

Game::Game()
{
    firstRun = true;
    InitAudioDevice();
    SetMasterVolume(0.5f);
    sndBallBounce = LoadSound("res/ball_bounce.mp3"); // Load WAV audio file
    ResetGame();
}

Game::~Game()
{
    delete instance;
    instance = 0;
    UnloadSound(sndBallBounce);
}

void Game::ResetGame()
{
    level = 1;
    gameover = false;
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

    if (gameover || firstRun)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            if (firstRun)
            {
                firstRun = false;
            }
            ResetGame();
            return;
        }
    }
    else if (levelComplete)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            NextLevel();
            return;
        }
    }
    else if (playerScored || oponentScored)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            playerScored = false;
            oponentScored = false;
        }
    }
    else
    {
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

        if (ball.x >= GetScreenWidth() - ball.cRadius)
        {
            oponent_score++;
            oponentScored = true;
            ResetObjects();
        }

        if (ball.x < ball.cRadius)
        {
            player_score++;
            playerScored = true;
            ResetObjects();
        }

        if (oponent_score >= 3)
        {
            gameover = true;
        }
        else if (player_score >= 3)
        {
            if (level >= maxLevels)
            {
                level = maxLevels;
                gameover = true;
                playerWins = true;
            }
            else
            {
                levelComplete = true;
            }
        }
    }
}

void Game::Draw()
{
    ClearBackground(boardColor);

    Color centerLineColor = markingColor;
    centerLineColor.a = 90;

    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, centerLineColor);
    // DrawLine(0, screenHeight/2, screenWidth, screenHeight/2, markingColor);

    Color centerColor = markingColor;
    centerColor.a = 100;
    DrawCircle(screenWidth / 2, screenHeight / 2, 100, centerColor);

    ball.Draw();
    player.Draw();
    oponent.Draw();

    DrawUI();
}

void Game::DrawUI()
{
    DrawText(TextFormat("%i", oponent_score), screenWidth / 4 - 20, 80, 80, oponent.color);
    DrawText(TextFormat("%i", player_score), 3 * screenWidth / 4 - 20, 80, 80, player.color);

    DrawText(TextFormat("Level: %i", level), screenWidth / 2 - 140, 80, 80, WHITE);

    if (firstRun)
    {
        DrawText("Press space to play", screenWidth / 2 - 250, screenHeight / 2 + 50, 50, WHITE);
    }
    else if (gameover)
    {
        if (playerWins)
        {
            DrawText("You win! Press space to play again", screenWidth / 2 - 450, screenHeight / 2 + 50, 50, WHITE);
        }
        else
        {
            DrawText("Game Over! Press space to play again", screenWidth / 2 - 450, screenHeight / 2 + 50, 50, WHITE);
        }
    }
    else if (playerScored && !levelComplete)
    {
        DrawText("Blue scores! Press space", screenWidth / 2 - 300, screenHeight / 2 + 50, 50, WHITE);
    }
    else if (oponentScored && !levelComplete)
    {
        DrawText("Red scores! Press space", screenWidth / 2 - 300, screenHeight / 2 + 50, 50, WHITE);
    }

    else if (levelComplete)
    {
        DrawText("You win! Press space for next level", screenWidth / 2 - 450, screenHeight / 2 + 50, 50, WHITE);
    }
}

void Game::ResetObjects()
{
    numBounces = 0;
    ball.SetSpeed(ballSpeeds[level - 1], ballSpeeds[level - 1]);
    ball.ResetBall();
    player.ResetPosition();
    oponent.ResetPosition();
}
