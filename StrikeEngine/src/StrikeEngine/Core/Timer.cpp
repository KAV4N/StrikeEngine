#include "Timer.h"
#include <thread>
#include <algorithm>

namespace StrikeEngine {
    Timer::Timer() {
        reset();
        setTargetFPS(mTargetFPS);
    }

    void Timer::setTargetFPS(float fps) {
        mTargetFPS = std::max(0.0f, fps);
        if (mTargetFPS > 0.0f) {
            mTargetFrameTime = std::chrono::nanoseconds(static_cast<long long>(1000000000.0f / mTargetFPS));
        }
        else {
            mTargetFrameTime = std::chrono::nanoseconds(0);
        }
    }

    void Timer::updateFrame() {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = now - mLastFrameTime;
        mDeltaTime = deltaTimeDuration.count();
        mLastFrameTime = now;

        calculateFPS();
    }

    void Timer::limitFrameRate() {
        if (mTargetFrameTime.count() > 0) {
            auto now = std::chrono::high_resolution_clock::now();
            auto frameTime = now - mLastFrameTime;

            if (frameTime < mTargetFrameTime) {
                auto sleepTime = mTargetFrameTime - frameTime;
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }

    void Timer::calculateFPS() {
        mFrameTimeAccumulator += mDeltaTime;
        mFrameCount++;

        // Update FPS every second
        if (mFrameTimeAccumulator >= 1.0f) {
            mCurrentFPS = static_cast<float>(mFrameCount) / mFrameTimeAccumulator;
            mFrameTimeAccumulator = 0.0f;
            mFrameCount = 0;
        }
    }

    void Timer::reset() {
        auto now = std::chrono::high_resolution_clock::now();
        mStartTime = now;
        mLastFrameTime = now;
        mDeltaTime = 0.0f;
        mCurrentFPS = 0.0f;
        mFrameTimeAccumulator = 0.0f;
        mFrameCount = 0;
    }

    double Timer::getElapsedTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - mStartTime;
        return elapsed.count();
    }
}