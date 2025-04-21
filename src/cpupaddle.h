#pragma once

#include "raylib.h"
#include "paddle.h"

class CPUPaddle : public Paddle
{
public:
    CPUPaddle();
    void Init();
    void Update(float dt, float ball_y);
    void SetSpeed(int spd);
    void ResetPosition();

    float thinkDelayTime;
};
