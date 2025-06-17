#pragma once
#include <string>

#include "raylib.h"
#include "globals.h"
#include "ball.h"
#include "paddle.h"
#include "cpupaddle.h"

const Color boardColor = DARKGREEN;
const Color markingColor = GREEN;
const Color ballColor = WHITE;

const int maxLevels = 6;
const int ballSpeeds[maxLevels] = {600, 625, 650, 700, 750, 900};
// const int cpuPaddleSpeeds[maxLevels]        = {500, 600, 500, 500, 500, 500};
// const float cpuPaddleThinkTimes[maxLevels] = {0.01f, 0.01f, 0.01f, 0.01f, 0.01f};
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

    void InitGame();
    void Reset();
    void ResetObjects();

    void Update(float dt);
    void HandleInput();
    void UpdateUI();

    void Draw();
    void DrawUI();
    void DrawScreenSpaceUI();
    std::string FormatWithLeadingZeroes(int number, int width);

    void NextLevel();
    int GetNumBallBounces();

    bool firstTimeGameStart;
    bool isFirstFrameAfterReset;
    bool isInExitMenu;
    bool paused;
    bool lostWindowFocus;
    bool gameOver;
    static bool isMobile;

private:
    Game();                                 // private constructor
    Game(const Game &) = delete;            // remove copy constructor
    Game &operator=(const Game &) = delete; // remove copy assignment operator
    Game(Game &&) = delete;                 // remove move constructor
    Game &operator=(Game &&) = delete;      // remove move assigment operator

    static Game *instance;

public:
    int player_score;
    int oponent_score;
    //bool firstRun;
    bool playerScored;
    bool oponentScored;
    bool playerWins;
    bool levelComplete;
    int level;
    int numBounces;
    Sound sndBallBounce;
    Sound sndBallBounceWall;
    Sound sndScore;  // Add score sound effect
    Music backgroundMusic;
    bool musicMuted;  // Add flag for music mute state

private:
    Ball ball;
    Paddle player;
    CPUPaddle oponent;

    float screenScale;
    RenderTexture2D targetRenderTex;
    Font font;

    // Mobile controls
    Rectangle upButton;
    Rectangle downButton;
    float collisionScale = 3.0f;
    Rectangle upButtonExpanded;
    Rectangle downButtonExpanded;
    int buttonSize = 120;
    int buttonSpacingX = 260;
    int buttonSpacingY = 80;
    bool upButtonPressed;
    bool downButtonPressed;
    float enterKeyDebounceTimer;  // Timer to prevent rapid Enter key presses
};
