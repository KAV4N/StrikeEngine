#pragma once
#include "StrikeEngine.h"

class HierarchyExample : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onEvent(StrikeEngine::Event& e) override;

private:
    void advance();
    void printStep();
    void updateLabel();

    // Steps
    void spawnRoot();
    void changeColor();
    void moveRoot();
    void rotateRoot();
    void scaleRoot();
    void spawnChild();
    void readHierarchy();
    void reparentChild();
    void worldVsLocal();
    void tagLookup();
    void destroyEntities();

    int  mStep = -1;
    bool mSpaceWasDown = false;

    StrikeEngine::Entity mRoot;
    StrikeEngine::Entity mChild;
    StrikeEngine::Entity mLabel;
};
