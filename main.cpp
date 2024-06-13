#include <iostream>
#include <raylib.h>

using namespace std;

int player_score = 0;
int oponent_score = 0;

const Color boardColor = DARKGREEN;
const Color markingColor = GREEN;
const Color ballColor = WHITE;

class Ball
{
public:
    Ball()
    {
        radius = cRadius;
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;
        speed_x = cSpeedX;
        speed_y = cSpeedY;
        color = ballColor;
    }

    void Draw()
    {
        DrawCircle(x, y, radius, color);
    }

    void Update(float dt)
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
        
        
        if( x >= GetScreenWidth() - cRadius) {
            oponent_score++;
            ResetBall();
        }

        if( x < cRadius ) {
            player_score++;
            ResetBall();
        }
        
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speeds[] = {-1, 1};
        speed_x *= speeds[GetRandomValue(0, 1)];
        speed_y *= speeds[GetRandomValue(0, 1)];
    }

    float x, y;
    int speed_x;
    int speed_y;
    int radius;
    Color color;

private:
    const int cSpeedX = 300;
    const int cSpeedY = 300;
    const int cRadius = 20;
};

class Paddle
{
public:
    float x, y;
    float width, height;
    int speed;
    Color color;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, color);
    }

    void Update(float dt)
    {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            y = y - speed * dt;
        }

        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            y = y + speed * dt;
        }

        LimitPaddleBounds();
    }



protected:
    void LimitPaddleBounds()
    {
        if (y <= 0)
        {
            y = 0;
        }

        if (y > GetScreenHeight() - height)
        {
            y = GetScreenHeight() - height;
        }
    }



};

class CPUPaddle : public Paddle
{
public:
    float cpuLag;  // 1.0 means no lag, 0.1 means the cpu will update only 10% of the time, done to add basic skill scalling
    float currentFrameSkipTime;

    void Update(float dt, float ball_y)
    {
        currentFrameSkipTime = 0.0f;

        if (y + height / 2 > ball_y)
        {
            y -= speed * dt;
        }
        if (y + height / 2 <= ball_y)
        {
            y += speed * dt;
        }

        LimitPaddleBounds();
    }
};

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(144);

    Ball ball;

    Paddle player;
    player.width = 25;
    player.height = 120;
    player.x = screenWidth - player.width - 10;
    player.y = screenHeight / 2 - player.height / 2;
    player.speed = 500;
    player.color = BLUE;

    CPUPaddle oponent;
    oponent.width = 25;
    oponent.currentFrameSkipTime = 0;
    oponent.height = 120;
    oponent.x = 10;
    oponent.cpuLag = 0.5;
    oponent.speed = 250;
    oponent.y = screenHeight / 2 - oponent.height / 2;
    oponent.color = Color{ 255, 41, 55, 255 };

    float dt = GetFrameTime();

    while (WindowShouldClose() == false)
    {
        ball.Update(dt);
        player.Update(dt);
        oponent.Update(dt, ball.y);

        // Check collisions
        if( CheckCollisionCircleRec( Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height} ) ) 
        {
            ball.speed_x *= -1;
            ball.Update(dt);
        }

        if( CheckCollisionCircleRec( Vector2{ball.x, ball.y}, ball.radius, Rectangle{oponent.x, oponent.y, oponent.width, oponent.height} ) ) 
        {
            ball.speed_x *= -1;
            ball.Update(dt);
        }

        BeginDrawing();
        ClearBackground(boardColor);

        Color centerLineColor = markingColor;
        centerLineColor.a = 90;

        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, centerLineColor);
        //DrawLine(0, screenHeight/2, screenWidth, screenHeight/2, markingColor);

        Color centerColor = markingColor;
        centerColor.a = 100;
        DrawCircle(screenWidth/2, screenHeight/2, 100, centerColor);
       
        ball.Draw();
        player.Draw();
        oponent.Draw();

        DrawText(TextFormat("%i", oponent_score), screenWidth/4 - 20, 80, 80, oponent.color);
        DrawText(TextFormat("%i", player_score),  3*screenWidth/4 - 20, 80, 80, player.color);
        
        EndDrawing();
        dt = GetFrameTime();
    }

    CloseWindow();
    return 0;
}