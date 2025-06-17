#include "game.h"
#include "cpupaddle.h"
#include <cmath>

// Helper function to clamp a value between min and max
static float Clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

CPUPaddle::CPUPaddle()
{
    Init();
}

void CPUPaddle::Init()
{
    width = 25;
    height = 120;
    x = 10;
    speed = 1200;  // Increased from 1000
    y = gameScreenHeight / 2 - height / 2;
    color = Color{255, 41, 55, 255};
    
    // Initialize AI parameters with higher base values (previous level 4 values)
    predictionAccuracy = 0.92f;    // Previous level 4 value
    reactionSpeed = 0.92f;         // Previous level 4 value
    mistakeChance = 0.06f;         // Previous level 4 value
    personalityAggressiveness = 0.92f; // Previous level 4 value
    
    // Initialize state
    targetY = y;
    lastBallY = 0;
    lastBallX = 0;
    lastBallVelocityX = 0;
    lastBallVelocityY = 0;
    decisionTimer = 0;
    isMakingMistake = false;
    mistakeTimer = 0;
    
    // Movement smoothing
    currentVelocity = 0;
    targetVelocity = 0;
    smoothingFactor = 0.2f;  // Increased from 0.15f for faster response
}

void CPUPaddle::Update(float dt, float ball_y, float ball_x, float ball_velocity_x, float ball_velocity_y)
{
    Game* game = Game::GetInstance();
    int level = game->level;
    
    // Update AI parameters with more aggressive scaling
    // Level 1: 92% accuracy -> Level 6: 99.5% accuracy
    predictionAccuracy = 0.92f + (level - 1) * 0.015f;
    
    // Level 1: 92% reaction -> Level 6: 99.5% reaction
    reactionSpeed = 0.92f + (level - 1) * 0.015f;
    
    // Level 1: 6% mistakes -> Level 6: 0.5% mistakes
    mistakeChance = 0.06f - (level - 1) * 0.011f;
    
    // Level 1: 92% aggressive -> Level 6: 99.5% aggressive
    personalityAggressiveness = 0.92f + (level - 1) * 0.015f;
    
    // Update decision timer with level-based reaction speed
    decisionTimer += dt;
    float decisionInterval = 0.08f / reactionSpeed;  // Reduced from 0.1f for faster decisions
    
    if (decisionTimer >= decisionInterval) {
        decisionTimer = 0;
        
        float centerY = gameScreenHeight / 2 - height / 2;
        
        // Calculate ball's predicted position with improved accuracy at higher levels
        float timeToReachPaddle = (x - ball_x) / ball_velocity_x;
        float predictedY = ball_y + (ball_velocity_y * timeToReachPaddle);
        
        // Add prediction error based on accuracy (reduced error range at higher levels)
        float error = (1.0f - predictionAccuracy) * 40.0f; // Reduced from 50.0f
        predictedY += GetRandomValue(-error, error);
        
        // Calculate distance to center
        float distanceToCenter = predictedY - centerY;
        
        // Apply a bias towards center position (reduced at higher levels)
        float centerBias = -distanceToCenter * (0.1f - (level - 1) * 0.015f); // Reduced from 0.15f
        
        // Add strategic positioning based on ball's vertical velocity
        float strategicOffset = 0;
        if (abs(ball_velocity_y) > 100.0f) {
            // Move more aggressively at higher levels
            strategicOffset = ball_velocity_y * (0.1f + (level - 1) * 0.02f) * personalityAggressiveness; // Increased multipliers
            strategicOffset = Clamp(strategicOffset, -60.0f, 60.0f); // Increased from 50.0f
        }
        
        // Calculate final position with center bias and strategic offset
        float finalY = predictedY + centerBias + strategicOffset;
        
        // Ensure the final position isn't too far from center (increased range at higher levels)
        float maxOffsetFromCenter = 250.0f + (level - 1) * 40.0f; // Increased from 200.0f + 30.0f
        if (abs(finalY - centerY) > maxOffsetFromCenter) {
            finalY = centerY + (finalY > centerY ? maxOffsetFromCenter : -maxOffsetFromCenter);
        }
        
        // Check for mistakes with reduced duration at higher levels
        if (!isMakingMistake && GetRandomValue(0, 100) < mistakeChance * 100) {
            isMakingMistake = true;
            mistakeTimer = GetRandomValue(1, 2) * (0.04f - (level - 1) * 0.005f); // Reduced from 0.06f
        }
        
        if (isMakingMistake) {
            mistakeTimer -= dt;
            if (mistakeTimer <= 0) {
                isMakingMistake = false;
            } else {
                // During mistake, move in wrong direction but still respect center bias
                float mistakeOffset = GetRandomValue(-40, 40); // Reduced from 50
                finalY = centerY + mistakeOffset;
            }
        }
        
        // Update target position
        targetY = finalY - height / 2;
        
        // Calculate target velocity based on distance to target (faster at higher levels)
        float distanceToTarget = targetY - y;
        float speedMultiplier = 1.0f + (level - 1) * 0.2f; // Increased from 0.15f
        targetVelocity = Clamp(distanceToTarget * 3.0f * speedMultiplier, -speed, speed); // Increased from 2.5f
    }
    
    // Smoothly interpolate current velocity towards target velocity
    // Faster smoothing at higher levels
    float currentSmoothingFactor = smoothingFactor * (1.0f + (level - 1) * 0.2f); // Increased from 0.15f
    currentVelocity = currentVelocity + (targetVelocity - currentVelocity) * currentSmoothingFactor;
    
    // Apply velocity to position
    y += currentVelocity * dt;
    
    LimitPaddleBounds();
    
    // Store current ball state for next frame
    lastBallY = ball_y;
    lastBallX = ball_x;
    lastBallVelocityX = ball_velocity_x;
    lastBallVelocityY = ball_velocity_y;
}

void CPUPaddle::SetSpeed(int spd)
{
    speed = spd;
}

void CPUPaddle::ResetPosition()
{
    x = 10;
    y = gameScreenHeight / 2 - height / 2;
    targetY = y;
    isMakingMistake = false;
    mistakeTimer = 0;
    currentVelocity = 0;
    targetVelocity = 0;
}
