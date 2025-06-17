#pragma once

#include "raylib.h"
#include "paddle.h"

class CPUPaddle : public Paddle
{
public:
    CPUPaddle();
    void Init();
    void Update(float dt, float ball_y, float ball_x, float ball_velocity_x, float ball_velocity_y);
    void SetSpeed(int spd);
    void ResetPosition();

    float thinkDelayTime;

    // AI behavior parameters
    float predictionAccuracy;    // How accurate the AI's predictions are (0-1)
    float reactionSpeed;         // How quickly the AI reacts to changes
    float mistakeChance;         // Chance of making a mistake in movement
    float personalityAggressiveness; // How aggressively the AI positions itself

    // Internal state
    float targetY;
    float lastBallY;
    float lastBallX;
    float lastBallVelocityX;
    float lastBallVelocityY;
    float decisionTimer;
    bool isMakingMistake;
    float mistakeTimer;

    // Movement smoothing
    float currentVelocity;
    float targetVelocity;
    float smoothingFactor;
};
