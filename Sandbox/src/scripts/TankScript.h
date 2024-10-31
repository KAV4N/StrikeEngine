#pragma once
#include "StrikeEngine.h"

class TankScript : public StrikeEngine::ScriptableEntity {
public:
    void OnScriptStart() override {
        auto& transform = GetComponent<StrikeEngine::TransformComponent>();
        transform.position = glm::vec3(1.f, 0.f, 0.f);
        transform.rotation = glm::vec3(270.f, 0.f, 0.f);
    }

    void OnScriptUpdate(float t_DeltaTime) override {
        auto& transform = GetComponent<StrikeEngine::TransformComponent>();
        transform.rotation += glm::vec3(1.f, 0.f, 0.f);  
    }

    void OnScriptDestroy() override {
        std::cout << "death has come";
    }
};
