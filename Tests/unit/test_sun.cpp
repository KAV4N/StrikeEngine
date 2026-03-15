#include <gtest/gtest.h>
#include <StrikeEngine.h>


static constexpr float kEps  = 1e-4f;
static constexpr float kDeg  = 0.01f;
static constexpr float kDir  = 0.001f;


class SunTest : public ::testing::Test {
protected:
    void SetUp() override {
        Strike::Log::init();
    }
};

// ============================================================
//  Default state
// ============================================================

TEST(SunTest, DefaultColor_IsWhite) {
    Strike::Sun sun;
    EXPECT_EQ(sun.getColor(), glm::uvec3(255, 255, 255));
}

TEST(SunTest, DefaultIntensity_IsOne) {
    Strike::Sun sun;
    EXPECT_FLOAT_EQ(sun.getIntensity(), 1.0f);
}

TEST(SunTest, DefaultRotation_IsIdentityQuaternion) {
    Strike::Sun sun;
    glm::quat r = sun.getRotation();
    EXPECT_NEAR(r.w, 1.0f, kEps);
    EXPECT_NEAR(r.x, 0.0f, kEps);
    EXPECT_NEAR(r.y, 0.0f, kEps);
    EXPECT_NEAR(r.z, 0.0f, kEps);
}

TEST(SunTest, DefaultCastShadows_IsFalse) {
    Strike::Sun sun;
    EXPECT_FALSE(sun.getCastShadows());
}


// ============================================================
//  setColor / getColor
// ============================================================

TEST(SunTest, SetColor_Roundtrip) {
    Strike::Sun sun;
    sun.setColor(glm::uvec3(100, 150, 200));
    EXPECT_EQ(sun.getColor(), glm::uvec3(100, 150, 200));
}

TEST(SunTest, SetColor_Overwrite) {
    Strike::Sun sun;
    sun.setColor(glm::uvec3(10, 20, 30));
    sun.setColor(glm::uvec3(50, 60, 70));
    EXPECT_EQ(sun.getColor(), glm::uvec3(50, 60, 70));
}

TEST(SunTest, SetColor_ClampsAbove255) {
    Strike::Sun sun;
    sun.setColor(glm::uvec3(300, 400, 500));
    EXPECT_EQ(sun.getColor(), glm::uvec3(255, 255, 255));
}

TEST(SunTest, SetColor_Zero_IsBlack) {
    Strike::Sun sun;
    sun.setColor(glm::uvec3(0, 0, 0));
    EXPECT_EQ(sun.getColor(), glm::uvec3(0, 0, 0));
}


// ============================================================
//  setIntensity / getIntensity
// ============================================================

TEST(SunTest, SetIntensity_Roundtrip) {
    Strike::Sun sun;
    sun.setIntensity(3.5f);
    EXPECT_FLOAT_EQ(sun.getIntensity(), 3.5f);
}

TEST(SunTest, SetIntensity_Zero_IsAllowed) {
    Strike::Sun sun;
    sun.setIntensity(0.0f);
    EXPECT_FLOAT_EQ(sun.getIntensity(), 0.0f);
}

TEST(SunTest, SetIntensity_NegativeClampsToZero) {
    Strike::Sun sun;
    sun.setIntensity(-5.0f);
    EXPECT_FLOAT_EQ(sun.getIntensity(), 0.0f);
}

TEST(SunTest, SetIntensity_Overwrite) {
    Strike::Sun sun;
    sun.setIntensity(1.0f);
    sun.setIntensity(7.0f);
    EXPECT_FLOAT_EQ(sun.getIntensity(), 7.0f);
}


// ============================================================
//  setCastShadows / getCastShadows
// ============================================================

TEST(SunTest, SetCastShadows_EnableRoundtrip) {
    Strike::Sun sun;
    sun.setCastShadows(true);
    EXPECT_TRUE(sun.getCastShadows());
}

TEST(SunTest, SetCastShadows_DisableRoundtrip) {
    Strike::Sun sun;
    sun.setCastShadows(true);
    sun.setCastShadows(false);
    EXPECT_FALSE(sun.getCastShadows());
}

TEST(SunTest, SetCastShadows_MultipleFlips) {
    Strike::Sun sun;
    for (int i = 0; i < 5; ++i) {
        sun.setCastShadows(true);
        EXPECT_TRUE(sun.getCastShadows());
        sun.setCastShadows(false);
        EXPECT_FALSE(sun.getCastShadows());
    }
}


// ============================================================
//  setRotation / getRotation (quaternion)
// ============================================================

TEST(SunTest, SetRotation_IdentityRoundtrip) {
    Strike::Sun sun;
    sun.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    glm::quat r = sun.getRotation();
    EXPECT_NEAR(r.w, 1.0f, kEps);
    EXPECT_NEAR(r.x, 0.0f, kEps);
    EXPECT_NEAR(r.y, 0.0f, kEps);
    EXPECT_NEAR(r.z, 0.0f, kEps);
}

TEST(SunTest, SetRotation_NormalizesInput) {
    Strike::Sun sun;
    sun.setRotation(glm::quat(2.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(glm::length(sun.getRotation()), 1.0f, kEps);
}

TEST(SunTest, SetRotation_Overwrite) {
    Strike::Sun sun;
    glm::quat q1 = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 1, 0));
    glm::quat q2 = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)));
    sun.setRotation(q1);
    sun.setRotation(q2);
    glm::quat r = sun.getRotation();
    EXPECT_NEAR(r.w, q2.w, kEps);
    EXPECT_NEAR(r.x, q2.x, kEps);
    EXPECT_NEAR(r.y, q2.y, kEps);
    EXPECT_NEAR(r.z, q2.z, kEps);
}


// ============================================================
//  setRotationEuler / getRotationEuler
// ============================================================

TEST(SunTest, SetRotationEuler_ZeroRoundtrip) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(0.0f));
    glm::vec3 e = sun.getRotationEuler();
    EXPECT_NEAR(e.x, 0.0f, kDeg);
    EXPECT_NEAR(e.y, 0.0f, kDeg);
    EXPECT_NEAR(e.z, 0.0f, kDeg);
}

TEST(SunTest, SetRotationEuler_PitchRoundtrip) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(45.0f, 0.0f, 0.0f));
    EXPECT_NEAR(sun.getRotationEuler().x, 45.0f, kDeg);
}

TEST(SunTest, SetRotationEuler_YawRoundtrip) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(0.0f, 90.0f, 0.0f));
    EXPECT_NEAR(sun.getRotationEuler().y, 90.0f, kDeg);
}

TEST(SunTest, SetRotationEuler_ResultIsNormalizedQuaternion) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(30.0f, 60.0f, 45.0f));
    EXPECT_NEAR(glm::length(sun.getRotation()), 1.0f, kEps);
}

TEST(SunTest, SetRotationEuler_ResetToIdentity) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(45.0f, 30.0f, 20.0f));
    sun.setRotationEuler(glm::vec3(0.0f));
    glm::quat r = sun.getRotation();
    EXPECT_NEAR(std::abs(r.w), 1.0f, kEps);
    EXPECT_NEAR(r.x, 0.0f, kEps);
    EXPECT_NEAR(r.y, 0.0f, kEps);
    EXPECT_NEAR(r.z, 0.0f, kEps);
}


// ============================================================
//  getDirection
// ============================================================

TEST(SunTest, GetDirection_DefaultPointsPositiveZ) {
    Strike::Sun sun;
    glm::vec3 d = sun.getDirection();
    EXPECT_NEAR(d.x, 0.0f, kEps);
    EXPECT_NEAR(d.y, 0.0f, kEps);
    EXPECT_NEAR(d.z, 1.0f, kEps);
}

TEST(SunTest, GetDirection_After90DegYawPointsPositiveX) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(0.0f, 90.0f, 0.0f));
    glm::vec3 d = sun.getDirection();
    EXPECT_NEAR(d.x,  1.0f, kDir);
    EXPECT_NEAR(d.y,  0.0f, kDir);
    EXPECT_NEAR(d.z,  0.0f, kDir);
}

TEST(SunTest, GetDirection_After90DegPitchPointsNegativeY) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(90.0f, 0.0f, 0.0f));
    glm::vec3 d = sun.getDirection();
    EXPECT_NEAR(d.x,  0.0f, kDir);
    EXPECT_NEAR(d.y, -1.0f, kDir);
    EXPECT_NEAR(d.z,  0.0f, kDir);
}

TEST(SunTest, GetDirection_IsNormalized) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(30.0f, 60.0f, 0.0f));
    EXPECT_NEAR(glm::length(sun.getDirection()), 1.0f, kEps);
}

TEST(SunTest, GetDirection_ConsistentWithSetRotation) {
    Strike::Sun sun;
    glm::quat q = glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1, 1, 0)));
    sun.setRotation(q);
    glm::vec3 expected = q * glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 d = sun.getDirection();
    EXPECT_NEAR(d.x, expected.x, kDir);
    EXPECT_NEAR(d.y, expected.y, kDir);
    EXPECT_NEAR(d.z, expected.z, kDir);
}


// ============================================================
//  rotate (incremental Euler)
// ============================================================

TEST(SunTest, Rotate_ZeroDoesNotChangeRotation) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(30.0f, 45.0f, 0.0f));
    glm::quat before = sun.getRotation();
    sun.rotate(glm::vec3(0.0f));
    glm::quat after = sun.getRotation();
    EXPECT_NEAR(after.w, before.w, kEps);
    EXPECT_NEAR(after.x, before.x, kEps);
    EXPECT_NEAR(after.y, before.y, kEps);
    EXPECT_NEAR(after.z, before.z, kEps);
}

TEST(SunTest, Rotate_AccumulatesWithPreviousRotation) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(0.0f, 45.0f, 0.0f));
    glm::vec3 dirBefore = sun.getDirection();
    sun.rotate(glm::vec3(0.0f, 45.0f, 0.0f));
    glm::vec3 dirAfter = sun.getDirection();
    bool changed = std::abs(dirAfter.x - dirBefore.x) > kDir
                || std::abs(dirAfter.y - dirBefore.y) > kDir
                || std::abs(dirAfter.z - dirBefore.z) > kDir;
    EXPECT_TRUE(changed);
}

TEST(SunTest, Rotate_ResultRemainsNormalized) {
    Strike::Sun sun;
    sun.rotate(glm::vec3(30.0f, 60.0f, 15.0f));
    EXPECT_NEAR(glm::length(sun.getRotation()), 1.0f, kEps);
}

TEST(SunTest, Rotate_360DegreesYawReturnsToStart) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(0.0f, 30.0f, 0.0f));
    glm::quat before = sun.getRotation();
    for (int i = 0; i < 4; ++i)
        sun.rotate(glm::vec3(0.0f, 90.0f, 0.0f));
    glm::quat after = sun.getRotation();
    EXPECT_NEAR(std::abs(glm::dot(before, after)), 1.0f, kEps);
}


// ============================================================
//  rotateQuaternion (axis-angle)
// ============================================================

TEST(SunTest, RotateQuaternion_ZeroAngle_NoChange) {
    Strike::Sun sun;
    sun.setRotationEuler(glm::vec3(20.0f, 10.0f, 0.0f));
    glm::quat before = sun.getRotation();
    sun.rotateQuaternion(0.0f, glm::vec3(0, 1, 0));
    glm::quat after = sun.getRotation();
    EXPECT_NEAR(after.w, before.w, kEps);
    EXPECT_NEAR(after.x, before.x, kEps);
    EXPECT_NEAR(after.y, before.y, kEps);
    EXPECT_NEAR(after.z, before.z, kEps);
}

TEST(SunTest, RotateQuaternion_ResultIsNormalized) {
    Strike::Sun sun;
    sun.rotateQuaternion(37.0f, glm::vec3(1, 1, 0));
    EXPECT_NEAR(glm::length(sun.getRotation()), 1.0f, kEps);
}

TEST(SunTest, RotateQuaternion_YAxisPremultiplies) {
    Strike::Sun sunY, sunX;
    sunY.setRotationEuler(glm::vec3(45.0f, 0.0f, 0.0f));
    sunX.setRotationEuler(glm::vec3(45.0f, 0.0f, 0.0f));
    sunY.rotateQuaternion(90.0f, glm::vec3(0, 1, 0));
    sunX.rotateQuaternion(90.0f, glm::vec3(1, 0, 0));
    glm::vec3 dY = sunY.getDirection();
    glm::vec3 dX = sunX.getDirection();
    bool different = std::abs(dY.x - dX.x) > kDir
                  || std::abs(dY.y - dX.y) > kDir
                  || std::abs(dY.z - dX.z) > kDir;
    EXPECT_TRUE(different);
}