#include <gtest/gtest.h>
#include <StrikeEngine.h>




constexpr float TRANSFORM_EPS = 0.01f;
constexpr float MATRIX_EPS    = 0.01f;
constexpr float DOT_EPS       = 0.001f;
constexpr float SCALE_EPS     = 0.001f;
constexpr float ROT_EPS       = 0.001f;




class EntityTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path path = "test";
        mScene = new Strike::Scene(path, "tagTestScene");
        ASSERT_NE(mScene, nullptr);
    }

    void TearDown() override {
        delete mScene;
        mScene = nullptr;
    }

    Strike::Scene* mScene = nullptr;
};


// ============================================================
//  Construction & isValid
// ============================================================

TEST_F(EntityTest, DefaultConstructed_IsInvalid) {
    Strike::Entity e;
    EXPECT_FALSE(e.isValid());
}

TEST_F(EntityTest, DefaultConstructed_BoolConversionFalse) {
    Strike::Entity e;
    EXPECT_FALSE(static_cast<bool>(e));
}

TEST_F(EntityTest, DefaultConstructed_HandleIsInvalidInRegistry) {
    Strike::Entity e;
    EXPECT_FALSE(e.isValid());
}

TEST_F(EntityTest, DefaultConstructed_SceneIsNull) {
    Strike::Entity e;
    EXPECT_EQ(e.getScene(), nullptr);
}

TEST_F(EntityTest, Created_IsValid) {
    auto e = mScene->createEntity();
    EXPECT_TRUE(e.isValid());
}

TEST_F(EntityTest, Created_BoolConversionTrue) {
    auto e = mScene->createEntity();
    EXPECT_TRUE(static_cast<bool>(e));
}

TEST_F(EntityTest, Created_HandleIsValidInRegistry) {
    auto e = mScene->createEntity();
    EXPECT_TRUE(mScene->getRegistry().valid(e.getHandle()));
}

TEST_F(EntityTest, Created_SceneMatchesOwner) {
    auto e = mScene->createEntity();
    EXPECT_EQ(e.getScene(), mScene);
}

TEST_F(EntityTest, TwoCreated_AreDistinct) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    EXPECT_NE(a, b);
    EXPECT_NE(a.getHandle(), b.getHandle());
}

TEST_F(EntityTest, Copied_IsEqualToOriginal) {
    auto a = mScene->createEntity();
    Strike::Entity b = a;
    EXPECT_EQ(a, b);
    EXPECT_EQ(a.getHandle(), b.getHandle());
}


// ============================================================
//  isActive / setActive
// ============================================================

TEST_F(EntityTest, NewEntity_IsActiveByDefault) {
    auto e = mScene->createEntity();
    EXPECT_TRUE(e.isActive());
}

TEST_F(EntityTest, SetActive_False_Deactivates) {
    auto e = mScene->createEntity();
    e.setActive(false);
    EXPECT_FALSE(e.isActive());
}

TEST_F(EntityTest, SetActive_True_Reactivates) {
    auto e = mScene->createEntity();
    e.setActive(false);
    e.setActive(true);
    EXPECT_TRUE(e.isActive());
}

TEST_F(EntityTest, SetActive_TogglesCorrectly) {
    auto e = mScene->createEntity();
    e.setActive(false);
    EXPECT_FALSE(e.isActive());
    e.setActive(true);
    EXPECT_TRUE(e.isActive());
    e.setActive(false);
    EXPECT_FALSE(e.isActive());
}


// ============================================================
//  setTag / getTag
// ============================================================

TEST_F(EntityTest, SetTag_CanBeReadBack) {
    auto e = mScene->createEntity();
    e.setTag("Player");
    EXPECT_EQ(e.getTag(), "Player");
}

TEST_F(EntityTest, SetTag_CanBeOverwritten) {
    auto e = mScene->createEntity();
    e.setTag("Player");
    e.setTag("Enemy");
    EXPECT_EQ(e.getTag(), "Enemy");
}

TEST_F(EntityTest, SetTag_EmptyString) {
    auto e = mScene->createEntity();
    e.setTag("");
    EXPECT_EQ(e.getTag(), "");
}

TEST_F(EntityTest, SetTag_SpecialCharacters) {
    auto e = mScene->createEntity();
    e.setTag("Entity_01 [Test]");
    EXPECT_EQ(e.getTag(), "Entity_01 [Test]");
}


// ============================================================
//  Local Position
// ============================================================

TEST_F(EntityTest, GetPosition_DefaultIsZero) {
    auto e = mScene->createEntity();
    EXPECT_EQ(e.getPosition(), glm::vec3(0.0f));
}

TEST_F(EntityTest, SetPosition_CanBeReadBack) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(e.getPosition(), glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST_F(EntityTest, SetPosition_NegativeValues) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(-5.0f, -10.0f, -3.0f));
    EXPECT_EQ(e.getPosition(), glm::vec3(-5.0f, -10.0f, -3.0f));
}

TEST_F(EntityTest, SetPosition_OverwritesPrevious) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(1.0f));
    e.setPosition(glm::vec3(9.0f, 8.0f, 7.0f));
    EXPECT_EQ(e.getPosition(), glm::vec3(9.0f, 8.0f, 7.0f));
}

TEST_F(EntityTest, Move_WithIdentityRotation_TranslatesAlongZ) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(0.0f));
    e.setEulerAngles(glm::vec3(0.0f));
    e.move(glm::vec3(0.0f, 0.0f, -1.0f));

    glm::vec3 pos = e.getPosition();
    EXPECT_NEAR(glm::length(pos), 1.0f, TRANSFORM_EPS);
}


// ============================================================
//  Local Rotation
// ============================================================

TEST_F(EntityTest, GetEulerAngles_DefaultIsZero) {
    auto e = mScene->createEntity();
    glm::vec3 a = e.getEulerAngles();

    EXPECT_NEAR(a.x, 0.0f, TRANSFORM_EPS);
    EXPECT_NEAR(a.y, 0.0f, TRANSFORM_EPS);
    EXPECT_NEAR(a.z, 0.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, SetEulerAngles_CanBeReadBack) {
    auto e = mScene->createEntity();
    e.setEulerAngles(glm::vec3(0.0f, 90.0f, 0.0f));

    glm::vec3 result = e.getEulerAngles();
    EXPECT_NEAR(result.y, 90.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, SetRotation_QuaternionRoundTrip) {
    auto e = mScene->createEntity();
    glm::quat q = glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f));

    e.setRotation(q);

    float dot = glm::abs(glm::dot(q, e.getRotation()));
    EXPECT_NEAR(dot, 1.0f, ROT_EPS);
}

TEST_F(EntityTest, RotateQuaternion_RotatesAroundAxis) {
    auto e = mScene->createEntity();

    e.setEulerAngles(glm::vec3(0.0f));
    e.rotateQuaternion(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    EXPECT_NEAR(e.getEulerAngles().y, 90.0f, TRANSFORM_EPS);
}


// ============================================================
//  Local Scale
// ============================================================

TEST_F(EntityTest, GetScale_DefaultIsOne) {
    auto e = mScene->createEntity();
    EXPECT_EQ(e.getScale(), glm::vec3(1.0f));
}

TEST_F(EntityTest, SetScale_CanBeReadBack) {
    auto e = mScene->createEntity();
    e.setScale(glm::vec3(2.0f, 3.0f, 4.0f));
    EXPECT_EQ(e.getScale(), glm::vec3(2.0f, 3.0f, 4.0f));
}

TEST_F(EntityTest, SetScale_UniformScale) {
    auto e = mScene->createEntity();
    e.setScale(glm::vec3(5.0f));
    EXPECT_EQ(e.getScale(), glm::vec3(5.0f));
}

TEST_F(EntityTest, SetScale_OverwritesPrevious) {
    auto e = mScene->createEntity();
    e.setScale(glm::vec3(2.0f));
    e.setScale(glm::vec3(0.5f));
    EXPECT_EQ(e.getScale(), glm::vec3(0.5f));
}


// ============================================================
//  World Transform
// ============================================================

TEST_F(EntityTest, SetWorldPosition_CanBeReadBack) {
    auto e = mScene->createEntity();
    e.setWorldPosition(glm::vec3(10.0f, 5.0f, 3.0f));

    glm::vec3 result = e.getWorldPosition();

    EXPECT_NEAR(result.x, 10.0f, TRANSFORM_EPS);
    EXPECT_NEAR(result.y,  5.0f, TRANSFORM_EPS);
    EXPECT_NEAR(result.z,  3.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, SetWorldRotation_QuaternionRoundTrip) {
    auto e = mScene->createEntity();

    glm::quat q = glm::quat(glm::vec3(0.0f, glm::radians(60.0f), 0.0f));
    e.setWorldRotation(q);

    float dot = glm::abs(glm::dot(q, e.getWorldRotation()));
    EXPECT_NEAR(dot, 1.0f, ROT_EPS);
}

TEST_F(EntityTest, GetWorldScale_DefaultIsOne) {
    auto e = mScene->createEntity();
    glm::vec3 ws = e.getWorldScale();

    EXPECT_NEAR(ws.x, 1.0f, SCALE_EPS);
    EXPECT_NEAR(ws.y, 1.0f, SCALE_EPS);
    EXPECT_NEAR(ws.z, 1.0f, SCALE_EPS);
}

TEST_F(EntityTest, RootEntity_WorldPositionMatchesLocalPosition) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(4.0f, 0.0f, 0.0f));

    EXPECT_NEAR(e.getWorldPosition().x, 4.0f, SCALE_EPS);
}


// ============================================================
//  Direction Vectors
// ============================================================

TEST_F(EntityTest, GetForward_DefaultFacesNegativeZ) {
    auto e = mScene->createEntity();
    e.setEulerAngles(glm::vec3(0.0f));

    EXPECT_NEAR(e.getForward().z, -1.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, GetRight_DefaultFacesPositiveX) {
    auto e = mScene->createEntity();
    e.setEulerAngles(glm::vec3(0.0f));

    EXPECT_NEAR(e.getRight().x, 1.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, GetUp_DefaultFacesPositiveY) {
    auto e = mScene->createEntity();
    e.setEulerAngles(glm::vec3(0.0f));

    EXPECT_NEAR(e.getUp().y, 1.0f, TRANSFORM_EPS);
}

TEST_F(EntityTest, DirectionVectors_AreOrthogonal) {
    auto e = mScene->createEntity();
    e.setEulerAngles(glm::vec3(30.0f, 45.0f, 0.0f));

    EXPECT_NEAR(glm::dot(e.getForward(), e.getRight()), 0.0f, DOT_EPS);
    EXPECT_NEAR(glm::dot(e.getForward(), e.getUp()),    0.0f, DOT_EPS);
    EXPECT_NEAR(glm::dot(e.getRight(),   e.getUp()),    0.0f, DOT_EPS);
}


// ============================================================
//  Matrices
// ============================================================

TEST_F(EntityTest, GetLocalMatrix_TranslationColumnReflectsPosition) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));

    const glm::mat4& m = e.getLocalMatrix();

    EXPECT_NEAR(m[3][0], 1.0f, MATRIX_EPS);
    EXPECT_NEAR(m[3][1], 2.0f, MATRIX_EPS);
    EXPECT_NEAR(m[3][2], 3.0f, MATRIX_EPS);
}

TEST_F(EntityTest, GetWorldMatrix_RootMatchesLocalMatrix) {
    auto e = mScene->createEntity();
    e.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));

    const glm::mat4& local = e.getLocalMatrix();
    const glm::mat4& world = e.getWorldMatrix();

    EXPECT_NEAR(local[3][0], world[3][0], MATRIX_EPS);
}