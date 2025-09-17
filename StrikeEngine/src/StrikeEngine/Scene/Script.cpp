#include "Script.h"

namespace StrikeEngine {

    Script::Script() = default;

    Script::~Script() {
        onDestroy();
    }

    void Script::onCreate() {}
    void Script::onStart() {}
    void Script::onUpdate(float deltaTime) {}
    void Script::onDestroy() {}


    const Entity& Script::getEntity() const {
        return mEntity;
    }

    bool Script::isActive() const {
        return mActive;
    }

    void Script::setActive(bool active) {
        mActive = active;
    }

    bool Script::isStarted() const {
        return mStarted;
    }

    void Script::setEntity(entt::entity entity, Scene* scene) {
        if (mStarted == false) {
            mEntity = Entity{ *scene, entity };
        }
    }

    void Script::markStarted() {
        mStarted = true;
    }

}
