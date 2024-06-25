#include "game.h"
#include "ball.h"

Ball::Ball()
{
    Init();
}

void Ball::Init()
{
    radius = cRadius;
    x = GetScreenWidth() / 2;
    y = GetScreenHeight() / 2;
    speed_x = ballSpeeds[0];
    speed_y = ballSpeeds[0];
    color = ballColor;
}

void Ball::Draw()
{
    DrawCircle(x, y, radius, color);
}

void Ball::Update(float dt)
{
    x += speed_x * dt;
    y += speed_y * dt;

    if ((y >= GetScreenHeight() - cRadius) || (y <= cRadius))
    {
        speed_y *= -1;
    }

    /*
    if ((x >= GetScreenWidth() - cRadius) || (x <= cRadius))
    {
        speed_x *= -1;
    }
    */
}

void Ball::ResetBall()
{
    x = GetScreenWidth() / 2;
    y = GetScreenHeight() / 2;

    int speeds[] = {-1, 1};
    speed_x *= speeds[GetRandomValue(0, 1)];
    speed_y *= speeds[GetRandomValue(0, 1)];
}

void Ball::SetSpeed(int spdX, int spdY)
{
    speed_x = spdX;
    speed_y = spdY;
}

void Ball::SetPosition(int posX, int posY)
{
    x = posX;
    y = posY;
}

void Ball::AddBounceSpeed(int bounce)
{
    if(speed_x >= 0)
    {
        speed_x += bounce;
    }
    else 
    {
        speed_x -= bounce;
    }
    
    /*
    if(speed_y >= 0)
    {
        speed_y += bounce;
    }
    else 
    {
        speed_y -= bounce;
    }
    */
}
