#pragma once
#include <chrono>

namespace StrikeEngine {

    class Timer {
    public:
        Timer();
        
        // Core timing functions
        void tick();                    // Call this at the start of each frame
        float getDeltaTime() const;     // Get time since last frame in seconds
        
        // FPS control
        void setTargetFPS(float fps);   // Set desired FPS (0 = unlimited)
        float getTargetFPS() const;     // Get target FPS
        float getCurrentFPS() const;    // Get actual FPS
        
        // Frame limiting
        void limitFrameRate();          // Call at end of frame to enforce target FPS
        
    private:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        
        TimePoint mLastFrameTime;
        TimePoint mCurrentFrameTime;
        
        float mDeltaTime;               // Time between frames in seconds
        float mTargetFPS;               // Desired FPS (0 = unlimited)
        float mTargetFrameTime;         // Target frame time in seconds
        float mCurrentFPS;              // Actual FPS
    };

}