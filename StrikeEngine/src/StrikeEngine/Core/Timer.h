#pragma once
#include <chrono>
#include <functional>

namespace StrikeEngine {
    class Timer {
    public:
        Timer();
        ~Timer() = default;

        // FPS Control
        void setTargetFPS(float fps);
        float getTargetFPS() const { return mTargetFPS; }
        float getCurrentFPS() const { return mCurrentFPS; }
        float getDeltaTime() const { return mDeltaTime; }
        double getElapsedTime() const;

        // Update methods
        void updateFrame();
        void limitFrameRate();
        void reset();

    private:
        void calculateFPS();

        // Timing variables
        std::chrono::high_resolution_clock::time_point mStartTime;
        std::chrono::high_resolution_clock::time_point mLastFrameTime;

        float mTargetFPS = 60.0f;
        float mCurrentFPS = 0.0f;
        float mDeltaTime = 0.0f;

        // FPS calculation
        float mFrameTimeAccumulator = 0.0f;
        int mFrameCount = 0;

        // Frame limiting
        std::chrono::nanoseconds mTargetFrameTime;
    };
}