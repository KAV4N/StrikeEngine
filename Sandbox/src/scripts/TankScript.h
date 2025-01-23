#pragma once
#include "StrikeEngine.h"

class TankScript : public StrikeEngine::GameScript {
public:
    int health;

    TankScript(int t_Health) {
        health = t_Health;
    }

    void Start() override {
        auto& transform = GetComponent<StrikeEngine::TransformComponent>();
        transform.position = glm::vec3(1.f, 0.f, 0.f);
        transform.rotation = glm::vec3(270.f, 0.f, 0.f);
    }

    void Update(float t_DeltaTime) override {
        auto& transform = GetComponent<StrikeEngine::TransformComponent>();
        transform.rotation += glm::vec3(1.f, 0.f, 0.f);  
        std::cout << health << std::endl;
        health--;
        if (health <= 0) {
            GetComponent<StrikeEngine::NativeScriptComponent>().RemoveScript<TankScript>();
        }
    }

    void OnDestroy() override {
        std::cout << "death has come";
    }
};
