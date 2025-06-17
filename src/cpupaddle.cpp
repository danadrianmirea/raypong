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
    speed = 1300;  // Reduced from 1400
    y = gameScreenHeight / 2 - height / 2;
    color = Color{255, 41, 55, 255};
    
    // Initialize AI parameters with balanced base values
    predictionAccuracy = 0.93f;    // Reduced from 0.95f
    reactionSpeed = 0.93f;         // Reduced from 0.95f
    mistakeChance = 0.08f;         // Increased from 0.04f
    personalityAggressiveness = 0.93f; // Reduced from 0.95f
    
    // Initialize state
    targetY = y;
    lastBallY = 0;
    lastBallX = 0;
    lastBallVelocityX = 0;
    lastBallVelocityY = 0;
    decisionTimer = 0;
    isMakingMistake = false;
    mistakeTimer = 0;
    lastMistakeTime = 0;
    
    // Movement smoothing
    currentVelocity = 0;
    targetVelocity = 0;
    smoothingFactor = 0.22f;  // Reduced from 0.25f
}

void CPUPaddle::Update(float dt, float ball_y, float ball_x, float ball_velocity_x, float ball_velocity_y)
{
    Game* game = Game::GetInstance();
    int level = game->level;
    
    // Update AI parameters with more gradual scaling
    // Level 1: 93% accuracy -> Level 6: 98% accuracy
    predictionAccuracy = 0.93f + (level - 1) * 0.01f;
    
    // Level 1: 93% reaction -> Level 6: 98% reaction
    reactionSpeed = 0.93f + (level - 1) * 0.01f;
    
    // Level 1: 8% mistakes -> Level 6: 3% mistakes
    mistakeChance = 0.08f - (level - 1) * 0.01f;
    
    // Level 1: 93% aggressive -> Level 6: 98% aggressive
    personalityAggressiveness = 0.93f + (level - 1) * 0.01f;
    
    // Update decision timer with balanced reaction speed
    decisionTimer += dt;
    float decisionInterval = 0.07f / reactionSpeed;  // Increased from 0.06f
    
    if (decisionTimer >= decisionInterval) {
        decisionTimer = 0;
        
        float centerY = gameScreenHeight / 2 - height / 2;
        
        // Calculate ball's predicted position with balanced accuracy
        float timeToReachPaddle = (x - ball_x) / ball_velocity_x;
        float predictedY = ball_y + (ball_velocity_y * timeToReachPaddle);
        
        // Add balanced prediction error
        float error = (1.0f - predictionAccuracy) * 30.0f; // Increased from 25.0f
        predictedY += GetRandomValue(-error, error);
        
        // Calculate distance to center
        float distanceToCenter = predictedY - centerY;
        
        // Balanced center bias
        float centerBias = -distanceToCenter * (0.13f - (level - 1) * 0.015f);
        
        // Balanced strategic positioning
        float strategicOffset = 0;
        if (abs(ball_velocity_y) > 100.0f) {
            // Balanced strategic movement
            float velocityFactor = Clamp(ball_velocity_y / 450.0f, -1.0f, 1.0f); // Increased from 400.0f
            strategicOffset = velocityFactor * 55.0f * personalityAggressiveness; // Reduced from 60.0f
            strategicOffset = Clamp(strategicOffset, -55.0f, 55.0f); // Reduced from 60.0f
        }
        
        // Calculate final position with balanced constraints
        float finalY = predictedY + centerBias + strategicOffset;
        
        // Balanced position limits
        float maxOffsetFromCenter = 220.0f + (level - 1) * 35.0f; // Reduced from 250.0f + 40.0f
        if (abs(finalY - centerY) > maxOffsetFromCenter) {
            finalY = centerY + (finalY > centerY ? maxOffsetFromCenter : -maxOffsetFromCenter);
        }
        
        // Improved mistake handling
        lastMistakeTime += dt;
        float minTimeBetweenMistakes = 2.0f - (level - 1) * 0.2f; // Minimum time between mistakes
        
        if (!isMakingMistake && lastMistakeTime >= minTimeBetweenMistakes) {
            // Only make mistakes when the ball is far enough away
            float distanceToBall = abs(ball_x - x);
            if (distanceToBall > 300.0f && GetRandomValue(0, 100) < mistakeChance * 100) {
                isMakingMistake = true;
                mistakeTimer = 0.045f - (level - 1) * 0.005f;
                lastMistakeTime = 0;
            }
        }
        
        if (isMakingMistake) {
            mistakeTimer -= dt;
            if (mistakeTimer <= 0) {
                isMakingMistake = false;
            } else {
                // Strategic mistake movement - move slightly off from optimal position
                float optimalDirection = (predictedY > centerY) ? 1.0f : -1.0f;
                float mistakeOffset = optimalDirection * -25.0f; // Move in opposite direction of optimal
                finalY = centerY + mistakeOffset;
            }
        }
        
        // Update target position
        targetY = finalY - height / 2;
        
        // Balanced velocity calculation
        float distanceToTarget = targetY - y;
        float speedMultiplier = 1.0f + (level - 1) * 0.15f; // Reduced from 0.2f
        targetVelocity = Clamp(distanceToTarget * 2.75f * speedMultiplier, -speed, speed); // Reduced from 3.0f
    }
    
    // Balanced movement smoothing
    float currentSmoothingFactor = smoothingFactor * (1.0f + (level - 1) * 0.15f); // Reduced from 0.2f
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
    lastMistakeTime = 0;
    currentVelocity = 0;
    targetVelocity = 0;
}
