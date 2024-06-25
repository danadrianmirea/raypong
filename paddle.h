#pragma once

#include "raylib.h"
//#include "game.h"

class Paddle
{
public:
    Paddle();
    void Init();
    void Draw();
    void Update(float dt);
    void ResetPosition();


    float x, y;
    float width, height;
    int speed;
    Color color;



protected:
    void LimitPaddleBounds();
};
