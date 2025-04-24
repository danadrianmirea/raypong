#include "globals.h"
#include "paddle.h"

Paddle::Paddle()
{
    Init();
}

void Paddle::Init()
{
    width = 25;
    height = 120;
    x = gameScreenWidth - width - 10;
    y = gameScreenHeight / 2 - height / 2;
    speed = 900;
    color = BLUE;
}

void Paddle::Draw()
{
    DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, color);
}

void Paddle::Update(float dt)
{
    LimitPaddleBounds();
}

void Paddle::ResetPosition()
{
    x = gameScreenWidth - width - 10;
    y = gameScreenHeight / 2 - height / 2;
}

void Paddle::LimitPaddleBounds()
{
    if (y <= 0)
    {
        y = 0;
    }

    if (y > gameScreenHeight - height)
    {
        y = gameScreenHeight - height;
    }
}