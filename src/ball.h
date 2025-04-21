#pragma once

#include "raylib.h"

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

    float x, y;
    int speed_x;
    int speed_y;
    int radius;
    Color color;
    const int cRadius = 20;
};
