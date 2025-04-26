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
    velocity = { 0.0f, 0.0f };  // Start with zero velocity
    maxVelocity = { (float)ballSpeeds[0], (float)ballSpeeds[0] };
    accelerationTime = 2.0f;  // 1 second to reach max velocity
    currentAccelTime = 0.0f;
    color = ballColor;
}

void Ball::Draw()
{
    DrawCircle(x, y, radius, color);
}

void Ball::Update(float dt)
{
    // Handle acceleration
    if (currentAccelTime < accelerationTime)
    {
        currentAccelTime += dt;
        float progress = currentAccelTime / accelerationTime;
        progress = Clamp(progress, 0.0f, 1.0f);
        
        // Linearly interpolate from 0 to max velocity
        velocity.x = maxVelocity.x * progress;
        velocity.y = maxVelocity.y * progress;
    }
    else
    {
        // Once acceleration is complete, maintain max velocity
        velocity = maxVelocity;
    }

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
    currentAccelTime = 0.0f;  // Reset acceleration timer

    // Set random initial direction while maintaining speed
    float speed = sqrtf(maxVelocity.x * maxVelocity.x + maxVelocity.y * maxVelocity.y);
    float angle = (45.0f + 90.0f * GetRandomValue(0, 3)) * DEG2RAD;
    maxVelocity.x = cosf(angle) * speed;
    maxVelocity.y = sinf(angle) * speed;
    velocity = { 0.0f, 0.0f };  // Start accelerating from zero
}

void Ball::SetSpeed(int spdX, int spdY)
{
    maxVelocity.x = (float)spdX;
    maxVelocity.y = (float)spdY;
    currentAccelTime = 0.0f;  // Reset acceleration timer
    velocity = { 0.0f, 0.0f };  // Start accelerating from zero
}

void Ball::SetPosition(int posX, int posY)
{
    x = posX;
    y = posY;
}

void Ball::AddBounceSpeed(int bounce)
{
    // Increase max speed while maintaining direction
    float speed = sqrtf(maxVelocity.x * maxVelocity.x + maxVelocity.y * maxVelocity.y);
    float direction = atan2f(maxVelocity.y, maxVelocity.x);
    speed += bounce;
    maxVelocity.x = cosf(direction) * speed;
    maxVelocity.y = sinf(direction) * speed;
}

void Ball::Reflect(const Vector2& normal)
{
    // Calculate reflection vector using the formula: R = V - 2(V·N)N
    float dotProduct = velocity.x * normal.x + velocity.y * normal.y;
    velocity.x = velocity.x - 2 * dotProduct * normal.x;
    velocity.y = velocity.y - 2 * dotProduct * normal.y;
    
    // Also reflect the max velocity to maintain direction
    dotProduct = maxVelocity.x * normal.x + maxVelocity.y * normal.y;
    maxVelocity.x = maxVelocity.x - 2 * dotProduct * normal.x;
    maxVelocity.y = maxVelocity.y - 2 * dotProduct * normal.y;
}
