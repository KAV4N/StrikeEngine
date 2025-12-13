#include "strikepch.h"
#include "Timer.h"
#include <thread>

namespace StrikeEngine {

    Timer::Timer()
        : mDeltaTime(0.0f)
        , mTargetFPS(60.0f)
        , mTargetFrameTime(1.0f / 60.0f)
        , mCurrentFPS(0.0f)
    {
        mLastFrameTime = Clock::now();
        mCurrentFrameTime = mLastFrameTime;
    }

    void Timer::tick() {
        mCurrentFrameTime = Clock::now();
        
        std::chrono::duration<float> elapsed = mCurrentFrameTime - mLastFrameTime;
        mDeltaTime = elapsed.count();
        
        // Calculate current FPS
        if (mDeltaTime > 0.0f) {
            mCurrentFPS = 1.0f / mDeltaTime;
        }
        
        mLastFrameTime = mCurrentFrameTime;
    }

    float Timer::getDeltaTime() const {
        return mDeltaTime;
    }

    void Timer::setTargetFPS(float fps) {
        mTargetFPS = fps;
        if (fps > 0.0f) {
            mTargetFrameTime = 1.0f / fps;
        } else {
            mTargetFrameTime = 0.0f;
        }
    }

    float Timer::getTargetFPS() const {
        return mTargetFPS;
    }

    float Timer::getCurrentFPS() const {
        return mCurrentFPS;
    }

    void Timer::limitFrameRate() {
        if (mTargetFPS <= 0.0f) {
            return; // Unlimited FPS
        }
        
        auto frameEnd = Clock::now();
        std::chrono::duration<float> frameTime = frameEnd - mCurrentFrameTime;
        
        float sleepTime = mTargetFrameTime - frameTime.count();
        
        if (sleepTime > 0.0f) {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(sleepTime)
            );
        }
    }

}