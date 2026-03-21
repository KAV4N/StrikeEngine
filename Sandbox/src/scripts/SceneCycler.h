// SceneCycler.h
#pragma once
#include <StrikeEngine.h>

class SceneCycler : public Strike::Script {
public:
    void onStart() override;
    void onEvent(Strike::Event& e) override;

private:
    std::vector<std::string> mScenes;
    int mCurrentIndex = 0;
};