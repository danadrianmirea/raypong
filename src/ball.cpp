#include "game.h"
#include "ball.h"

// Helper function to clamp a value between min and max
static float Clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

Ball::Ball()
{
    Init();
}

void Ball::Init()
{
    radius = cRadius;
    x = gameScreenWidth / 2;
    y = gameScreenHeight / 2;
    velocity = { (float)ballSpeeds[0], (float)ballSpeeds[0] };
    color = ballColor;
}

void Ball::Draw()
{
    DrawCircle(x, y, radius, color);
}

void Ball::Update(float dt)
{
    x += velocity.x * dt;
    y += velocity.y * dt;

    // Check for wall collisions
    if (y >= gameScreenHeight - cRadius)
    {
        Reflect({ 0, -1 }); // Reflect off bottom wall
        y = gameScreenHeight - cRadius; // Prevent sticking to wall
    }
    else if (y <= cRadius)
    {
        Reflect({ 0, 1 }); // Reflect off top wall
        y = cRadius; // Prevent sticking to wall
    }
}

void Ball::ResetBall()
{
    x = gameScreenWidth / 2;
    y = gameScreenHeight / 2;

    // Set random initial direction while maintaining speed
    float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
    float angle = (45.0f + 90.0f * GetRandomValue(0, 3)) * DEG2RAD;
    velocity.x = cosf(angle) * speed;
    velocity.y = sinf(angle) * speed;
}

void Ball::SetSpeed(int spdX, int spdY)
{
    velocity.x = (float)spdX;
    velocity.y = (float)spdY;
}

void Ball::SetPosition(int posX, int posY)
{
    x = posX;
    y = posY;
}

void Ball::AddBounceSpeed(int bounce)
{
    // Increase speed while maintaining direction
    float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
    float direction = atan2f(velocity.y, velocity.x);
    speed += bounce;
    velocity.x = cosf(direction) * speed;
    velocity.y = sinf(direction) * speed;
}

void Ball::Reflect(const Vector2& normal)
{
    // Calculate reflection vector using the formula: R = V - 2(V·N)N
    float dotProduct = velocity.x * normal.x + velocity.y * normal.y;
    velocity.x = velocity.x - 2 * dotProduct * normal.x;
    velocity.y = velocity.y - 2 * dotProduct * normal.y;
}
