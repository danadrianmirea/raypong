#pragma once

#include "raylib.h"
#include "ball.h"
#include "paddle.h"
#include "cpupaddle.h"

const Color boardColor = DARKGREEN;
const Color markingColor = GREEN;
const Color ballColor = WHITE;
const int screenWidth = 1280;
const int screenHeight = 800;

const int maxLevels = 6;
const int ballSpeeds[maxLevels]         = {600, 625 , 650, 700, 750, 900};
//const int cpuPaddleSpeeds[maxLevels]        = {500, 600, 500, 500, 500, 500};
//const float cpuPaddleThinkTimes[maxLevels] = {0.01f, 0.01f, 0.01f, 0.01f, 0.01f};
const float cpuPaddleThinkTimes[maxLevels] = {0.007f, 0.006f, 0.005f, 0.003f, 0.002f, 0.00f};

const float cpuThinkTimeDecay = 0.0005f;

const int ballSpeedBounceIncrement = 50;
const float bounceBack = 1.1f; 

class Game
{
public:
    virtual ~Game();
    static Game *GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new Game();
        }
        return instance;
    }

    void Update(float dt);
    void Draw();
    void DrawUI();
    void ResetObjects();
    void ResetGame();    
    void NextLevel();
    int GetNumBallBounces();

private:
    Game();                                // private constructor
    Game(const Game&) = delete;            // remove copy constructor
    Game& operator=(const Game&) = delete; // remove copy assignment operator
    Game(Game&&) = delete;                 // remove move constructor
    Game& operator=(Game&&) = delete;      // remove move assigment operator

    static Game *instance;

public:
    int player_score;
    int oponent_score;
    bool firstRun;
    bool playerScored;
    bool oponentScored;
    bool gameover;
    bool playerWins;
    bool levelComplete;
    int level;
    int numBounces;
    Sound sndBallBounce;
    

private:
    Ball ball;
    Paddle player;
    CPUPaddle oponent;
};
