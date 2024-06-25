#include "paddle.h"

Paddle::Paddle()
{
    Init();
}

void Paddle::Init()
{
    width = 25;
    height = 120;
    x = GetScreenWidth() - width - 10;
    y = GetScreenHeight() / 2 - height / 2;
    speed = 600;
    color = BLUE;
}

void Paddle::Draw()
{
    DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, color);
}

void Paddle::Update(float dt)
{
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        y = y - speed * dt;
    }

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        y = y + speed * dt;
    }

    LimitPaddleBounds();
}

void Paddle::ResetPosition()
{
    x = GetScreenWidth() - width - 10;
    y = GetScreenHeight() / 2 - height / 2;
}

void Paddle::LimitPaddleBounds()
{
    if (y <= 0)
    {
        y = 0;
    }

    if (y > GetScreenHeight() - height)
    {
        y = GetScreenHeight() - height;
    }
}