#include "SunMovement.h"
#include <glm/glm.hpp>

void SunMovement::onStart() {
    auto scene = StrikeEngine::World::get().getScene();
    mSun = &scene->getSun();  
    mTimeOfDay = 6.0f;
}


void SunMovement::onUpdate(float deltaTime) {
    auto scene = StrikeEngine::World::get().getScene();
    auto entity = scene->getEntity("textEntity");


    int width = StrikeEngine::Application::get().getWindow().getWidth();
    int height = StrikeEngine::Application::get().getWindow().getHeight();
    
    if (entity.isValid()){
        entity.setPosition(glm::vec3(width/2, height/2, 0.0f)); 
        entity.getComponent<StrikeEngine::TextComponent>().setText( 
            "Time of Day: " + std::to_string(static_cast<int>(mTimeOfDay)) + ":00"
        );
    }

    // Rotate the sun
    float rotationSpeed = 10.0f;
    mSun->rotate(glm::vec3(rotationSpeed * deltaTime, 0.0f, 0.0f));

    // Simulate time of day (0 to 24 hours)
    float daySpeed = 0.1f; // Adjust for faster/slower cycle
    mTimeOfDay += deltaTime * daySpeed;
    if (mTimeOfDay >= 24.0f)
        mTimeOfDay -= 24.0f;

    // Get sun color based on time
    glm::vec3 color = getSunColor(mTimeOfDay);
    mSun->setColor(color);
}

// Helper function to interpolate between two colors
glm::vec3 lerpColor(const glm::vec3& start, const glm::vec3& end, float t) {
    return start + (end - start) * t;
}

// Helper function to return sun color by hour with smooth transitions
glm::vec3 SunMovement::getSunColor(float hour) {
    // Define key times and colors for the day cycle
    struct TimeColor {
        float time;
        glm::vec3 color;
    };

    // Keyframe colors (RGB values normalized to 0-1 for smoother interpolation)
    TimeColor keyframes[] = {
        { 5.0f,  glm::vec3(15.0f / 255.0f, 15.0f / 255.0f, 40.0f / 255.0f) },  // Night
        { 7.0f,  glm::vec3(255.0f / 255.0f, 100.0f / 255.0f, 50.0f / 255.0f) }, // Sunrise
        { 11.0f, glm::vec3(255.0f / 255.0f, 220.0f / 255.0f, 120.0f / 255.0f) }, // Morning
        { 15.0f, glm::vec3(255.0f / 255.0f, 255.0f / 255.0f, 200.0f / 255.0f) }, // Noon
        { 18.0f, glm::vec3(255.0f / 255.0f, 200.0f / 255.0f, 100.0f / 255.0f) }, // Afternoon
        { 20.0f, glm::vec3(255.0f / 255.0f, 100.0f / 255.0f, 50.0f / 255.0f) }, // Sunset
        { 22.0f, glm::vec3(30.0f / 255.0f, 20.0f / 255.0f, 50.0f / 255.0f) }   // Dusk
    };

    // Handle night wrap-around (22:00 to 5:00 next day)
    if (hour >= 22.0f || hour < 5.0f) {
        float t;
        glm::vec3 startColor, endColor;
        if (hour >= 22.0f) {
            // Transition from Dusk (22:00) to Night (5:00 next day)
            t = (hour - 22.0f) / (24.0f - 22.0f + 5.0f);
            startColor = keyframes[6].color; // Dusk
            endColor = keyframes[0].color;   // Night
        } else {
            // Transition within Night (0:00 to 5:00)
            t = hour / 5.0f;
            startColor = keyframes[0].color; // Night
            endColor = keyframes[0].color;   // Night (same color, no change)
        }
        return lerpColor(startColor, endColor, t) * 255.0f;
    }

    // Find the two keyframes to interpolate between
    for (int i = 0; i < 6; ++i) {
        if (hour >= keyframes[i].time && hour < keyframes[i + 1].time) {
            float t = (hour - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
            return lerpColor(keyframes[i].color, keyframes[i + 1].color, t) * 255.0f;
        }
    }

    // Default case (should not reach here with proper keyframe setup)
    return keyframes[0].color * 255.0f; // Fallback to Night color
}

REGISTER_SCRIPT(SunMovement);