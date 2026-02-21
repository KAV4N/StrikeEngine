#pragma once
#include "StrikeEngine.h"

class HierarchyExample : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

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

    Strike::Entity mRoot;
    Strike::Entity mChild;
    Strike::Entity mLabel;
};
