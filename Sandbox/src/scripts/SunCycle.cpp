#include "SunCycle.h"
#include <cmath>

static glm::uvec3 lerpColor(const glm::uvec3& a, const glm::uvec3& b, float t)
{
    return glm::uvec3(
        static_cast<unsigned int>(a.r + t * (static_cast<float>(b.r) - a.r)),
        static_cast<unsigned int>(a.g + t * (static_cast<float>(b.g) - a.g)),
        static_cast<unsigned int>(a.b + t * (static_cast<float>(b.b) - a.b))
    );
}

static float smoothStep(float t) { return t * t * (3.0f - 2.0f * t); }

void SunCycle::onStart()
{
    auto scene = Strike::World::get().getScene();
    if (!scene) {
        STRIKE_ERROR("SunCycle: no active scene found");
        return;
    }

    mSun = &scene->getSun();
    mSun->setCastShadows(true);

    if (scene->hasSkybox()) {
        mSkyboxId      = scene->getSkyboxId();
        mSkyboxVisible = true;
        STRIKE_INFO("SunCycle: captured skybox id '{}'", mSkyboxId);
    } else {
        STRIKE_WARN("SunCycle: no skybox found in scene - night sky toggle disabled");
    }

    applySunState();
    updateSkybox(mTimeOfDay);
    updateFog(mTimeOfDay);

    STRIKE_INFO("SunCycle: started. Day duration = {:.1f}s  Start hour = {:.1f}",
                mDayDuration, mTimeOfDay);
}

void SunCycle::onUpdate(float deltaTime)
{
    if (mPaused || !mSun) return;

    float hoursPerSecond = 24.0f / mDayDuration;
    mTimeOfDay += hoursPerSecond * deltaTime;
    if (mTimeOfDay >= 24.0f)
        mTimeOfDay -= 24.0f;

    applySunState();
    updateSkybox(mTimeOfDay);
    updateFog(mTimeOfDay);
}

void SunCycle::onEvent(Strike::Event& e)
{
    if (e.getEventType() != Strike::EventType::KeyPressed) return;

    auto& key = static_cast<Strike::KeyPressedEvent&>(e);
    if (key.getRepeatCount() != 0) return;

    if (key.getKeyCode() == STRIKE_KEY_P) {
        mPaused = !mPaused;
        STRIKE_INFO("SunCycle: {}", mPaused ? "paused" : "resumed");
        e.handled = true;
    }
}

void SunCycle::updateSkybox(float hours)
{
    if (mSkyboxId.empty()) return;

    float pitch = hoursToAngleDeg(hours);
    float normalizedPitch = std::fmod(pitch + 360.0f, 360.0f);
    bool sunAboveHorizon = (normalizedPitch >= 10.0f && normalizedPitch <= 170.0f);

    auto scene = Strike::World::get().getScene();
    if (!scene) return;

    if (!sunAboveHorizon && mSkyboxVisible) {
        scene->setSkybox("");
        mSkyboxVisible = false;
    }
    else if (sunAboveHorizon && !mSkyboxVisible) {
        scene->setSkybox(mSkyboxId);
        mSkyboxVisible = true;
    }
}

void SunCycle::updateFog(float hours)
{
    static const glm::uvec3 kNightFogColor = { 5, 5, 20 };
    static const glm::uvec3 kZeroColor     = { 0, 0,  0 };

    float pitch           = hoursToAngleDeg(hours);
    float normalizedPitch = std::fmod(pitch + 360.0f, 360.0f);
    bool  sunAboveHorizon = (normalizedPitch >= 10.0f && normalizedPitch <= 170.0f);

    float density = sunAboveHorizon ? fogDensityForHour(hours) : kMaxFogDensity;

    if (density <= 0.0f) {
        Strike::World::get().setFog(0.0f, 0.0f, 0.0f, kZeroColor);
        return;
    }

    float t    = density / kMaxFogDensity;
    float nearP = 800.0f  - t * 650.0f;
    float farP  = 2000.0f - t * 1400.0f;

    Strike::World::get().setFog(nearP, farP, density, kNightFogColor);
}

float SunCycle::fogDensityForHour(float hours) const
{
    struct Stop { float hour; float density; };
    static const Stop kStops[] = {
        {  0.0f, kMaxFogDensity },
        {  5.0f, kMaxFogDensity },
        {  8.0f, 0.002f         },
        { 11.0f, 0.0f           },
        { 13.0f, 0.0f           },
        { 16.0f, 0.002f         },
        { 18.5f, 0.005f         },
        { 20.5f, kMaxFogDensity },
        { 24.0f, kMaxFogDensity },
    };
    constexpr int kCount = static_cast<int>(sizeof(kStops) / sizeof(kStops[0]));

    for (int i = 0; i < kCount - 1; ++i) {
        if (hours >= kStops[i].hour && hours < kStops[i + 1].hour) {
            float t = (hours - kStops[i].hour) / (kStops[i + 1].hour - kStops[i].hour);
            return kStops[i].density + smoothStep(t) * (kStops[i + 1].density - kStops[i].density);
        }
    }
    return kMaxFogDensity;
}

float SunCycle::hoursToAngleDeg(float hours) const
{
    float normalized = (hours - 6.0f) / 24.0f;
    return normalized * 360.0f - 90.0f;
}

glm::uvec3 SunCycle::sunColorForHour(float hours) const
{
    static const glm::uvec3 kNight     = {  10,  10,  40 };
    static const glm::uvec3 kDawn      = { 255, 100,  30 };
    static const glm::uvec3 kMorning   = { 255, 200, 140 };
    static const glm::uvec3 kNoon      = { 255, 245, 210 };
    static const glm::uvec3 kAfternoon = { 255, 200, 140 };
    static const glm::uvec3 kDusk      = { 255,  80,  20 };
    static const glm::uvec3 kTwilight  = {  80,  30,  80 };

    struct Stop { float hour; glm::uvec3 color; };
    static const Stop kStops[] = {
        {  0.0f, kNight     },
        {  5.0f, kNight     },
        {  6.0f, kDawn      },
        {  8.0f, kMorning   },
        { 12.0f, kNoon      },
        { 16.0f, kAfternoon },
        { 18.5f, kDusk      },
        { 20.0f, kTwilight  },
        { 21.0f, kNight     },
        { 24.0f, kNight     },
    };
    constexpr int kCount = static_cast<int>(sizeof(kStops) / sizeof(kStops[0]));

    for (int i = 0; i < kCount - 1; ++i) {
        if (hours >= kStops[i].hour && hours < kStops[i + 1].hour) {
            float t = (hours - kStops[i].hour) / (kStops[i + 1].hour - kStops[i].hour);
            return lerpColor(kStops[i].color, kStops[i + 1].color, smoothStep(t));
        }
    }
    return kNight;
}

float SunCycle::sunIntensityForHour(float hours) const
{
    if (hours < 5.0f || hours >= 21.0f) return 0.02f;
    if (hours < 6.0f)  return smoothStep((hours - 5.0f)) * 0.4f;
    if (hours < 8.0f)  return 0.4f + smoothStep((hours - 6.0f) / 2.0f) * 0.6f;
    if (hours <= 16.0f) return 1.0f;
    if (hours < 18.5f) return 1.0f - smoothStep((hours - 16.0f) / 2.5f) * 0.6f;
    if (hours < 20.0f) return 0.4f - smoothStep((hours - 18.5f) / 1.5f) * 0.38f;
    return 0.02f + smoothStep(1.0f - (hours - 20.0f)) * 0.02f;
}

void SunCycle::applySunState()
{
    if (!mSun) return;

    float pitchDeg = hoursToAngleDeg(mTimeOfDay);
    mSun->setRotationEuler(glm::vec3(pitchDeg, 30.0f, 0.0f));
    mSun->setColor(sunColorForHour(mTimeOfDay));
    mSun->setIntensity(sunIntensityForHour(mTimeOfDay));

    bool daytime = (mTimeOfDay >= 5.5f && mTimeOfDay < 20.5f);
    mSun->setCastShadows(daytime);
}

REGISTER_SCRIPT(SunCycle)
