#include "game.h"
#include "cpupaddle.h"

CPUPaddle::CPUPaddle()
{
    Init();
}

void CPUPaddle::Init()
{
    width = 25;
    height = 120;
    x = 10;
    //speed = cpuPaddleSpeeds[0];
    speed = 800;
    y = gameScreenHeight / 2 - height / 2;
    color = Color{255, 41, 55, 255};
    thinkDelayTime = 0.0f;
}

void CPUPaddle::Update(float dt, float ball_y)
{
    thinkDelayTime += dt;
    Game* game = Game::GetInstance();
    float totalThinkDelay = cpuPaddleThinkTimes[game->level-1] + cpuThinkTimeDecay* game->GetNumBallBounces();
    //DrawText(TextFormat("%.2f", totalThinkDelay), screenWidth / 2 - 450, screenHeight / 2 + 50, 50, WHITE);

    if(thinkDelayTime < totalThinkDelay )
    {
        return;
    }
    thinkDelayTime = 0.0f;

    if (y + height / 2 > ball_y)
    {
        y -= speed * dt;
    }
    if (y + height / 2 <= ball_y)
    {
        y += speed * dt;
    }

    LimitPaddleBounds();
}

void CPUPaddle::SetSpeed(int spd)
{
    speed = spd;
}

void CPUPaddle::ResetPosition()
{
    x = 10;
    y = gameScreenHeight / 2 - height / 2;
}
