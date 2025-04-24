#pragma once

#include "raylib.h"
#include <cmath>

class Ball
{
public:
    Ball();
    void Init();
    void Draw();
    void Update(float dt);
    void ResetBall();
    void SetSpeed(int spdX, int spdY);
    void SetPosition(int posX, int posY);
    void AddBounceSpeed(int bounce);
    void Reflect(const Vector2& normal);

    float x, y;
    Vector2 velocity;  // Using Vector2 for velocity instead of separate x and y components
    int radius;
    Color color;
    const int cRadius = 20;
};
