#include <gtest/gtest.h>
#include <StrikeEngine.h>


static constexpr float kEps = 0.001f;

class SceneWorldTransformTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path path = "test";
        mScene = new Strike::Scene(path, "WorldTransformTestScene");
        ASSERT_NE(mScene, nullptr);
    }

    void TearDown() override {
        delete mScene;
        mScene = nullptr;
    }

    Strike::Entity makeAt(glm::vec3 pos) {
        auto e = mScene->createEntity();
        e.setPosition(pos);
        return e;
    }

    Strike::Scene* mScene = nullptr;
};


// ============================================================
//  Single parent child position propagation
// ============================================================

TEST_F(SceneWorldTransformTest, Child_WorldPosition_IsParentPlusLocal) {
    auto parent = makeAt(glm::vec3(10.0f, 0.0f, 0.0f));
    auto child  = makeAt(glm::vec3(5.0f,  0.0f, 0.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x, 15.0f, kEps);
    EXPECT_NEAR(wp.y,  0.0f, kEps);
    EXPECT_NEAR(wp.z,  0.0f, kEps);
}

TEST_F(SceneWorldTransformTest, Child_WorldPosition_AllAxes) {
    auto parent = makeAt(glm::vec3(1.0f, 2.0f, 3.0f));
    auto child  = makeAt(glm::vec3(4.0f, 5.0f, 6.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x, 5.0f, kEps);
    EXPECT_NEAR(wp.y, 7.0f, kEps);
    EXPECT_NEAR(wp.z, 9.0f, kEps);
}

TEST_F(SceneWorldTransformTest, Child_WorldPosition_NegativeOffsets) {
    auto parent = makeAt(glm::vec3(10.0f, 10.0f, 10.0f));
    auto child  = makeAt(glm::vec3(-3.0f, -3.0f, -3.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x, 7.0f, kEps);
    EXPECT_NEAR(wp.y, 7.0f, kEps);
    EXPECT_NEAR(wp.z, 7.0f, kEps);
}


// ============================================================
//  Three-level deep hierarchy
// ============================================================

TEST_F(SceneWorldTransformTest, Grandchild_WorldPosition_IsTransitiveSum) {
    auto grandparent = makeAt(glm::vec3(1.0f, 0.0f, 0.0f));
    auto parent      = makeAt(glm::vec3(2.0f, 0.0f, 0.0f));
    auto child       = makeAt(glm::vec3(3.0f, 0.0f, 0.0f));

    parent.setParent(grandparent);
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x, 6.0f, kEps);
    EXPECT_NEAR(wp.y, 0.0f, kEps);
    EXPECT_NEAR(wp.z, 0.0f, kEps);
}


// ============================================================
//  Parent scale propagates to child world scale
// ============================================================

TEST_F(SceneWorldTransformTest, Child_WorldScale_InheritsParentScale) {
    auto parent = mScene->createEntity();
    parent.setScale(glm::vec3(2.0f));

    auto child = mScene->createEntity();
    child.setScale(glm::vec3(3.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 ws = child.getWorldScale();
    EXPECT_NEAR(ws.x, 6.0f, kEps);
    EXPECT_NEAR(ws.y, 6.0f, kEps);
    EXPECT_NEAR(ws.z, 6.0f, kEps);
}


// ============================================================
//  Parent rotation rotates child world position
// ============================================================

TEST_F(SceneWorldTransformTest, Child_WorldPosition_RotatedParent_90DegY) {
    auto parent = mScene->createEntity();
    parent.setPosition(glm::vec3(0.0f));
    parent.setEulerAngles(glm::vec3(0.0f, 90.0f, 0.0f));

    auto child = mScene->createEntity();
    child.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x,  0.0f, kEps);
    EXPECT_NEAR(wp.y,  0.0f, kEps);
    EXPECT_NEAR(wp.z, -1.0f, kEps);
}

TEST_F(SceneWorldTransformTest, Child_WorldPosition_RotatedParent_90DegX) {
    auto parent = mScene->createEntity();
    parent.setPosition(glm::vec3(0.0f));
    parent.setEulerAngles(glm::vec3(90.0f, 0.0f, 0.0f));

    auto child = mScene->createEntity();
    child.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    child.setParent(parent);
    mScene->flushTransforms();

    glm::vec3 wp = child.getWorldPosition();
    EXPECT_NEAR(wp.x, 0.0f, kEps);
    EXPECT_NEAR(wp.y, 0.0f, kEps);
    EXPECT_NEAR(wp.z, 1.0f, kEps);
}


// ============================================================
//  Reparenting world transform updates when parent changes
// ============================================================

TEST_F(SceneWorldTransformTest, Reparent_WorldPositionUpdates) {
    auto parentA = makeAt(glm::vec3(10.0f, 0.0f, 0.0f));
    auto parentB = makeAt(glm::vec3(0.0f,  0.0f, 0.0f));
    auto child   = makeAt(glm::vec3(1.0f,  0.0f, 0.0f));

    child.setParent(parentA);
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 11.0f, kEps);

    child.setParent(parentB);
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 1.0f, kEps);
}

TEST_F(SceneWorldTransformTest, RemoveFromParent_BecomesRoot_WorldEqualsLocal) {
    auto parent = makeAt(glm::vec3(10.0f, 0.0f, 0.0f));
    auto child  = makeAt(glm::vec3(2.0f,  0.0f, 0.0f));
    child.setParent(parent);
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 12.0f, kEps);

    child.removeFromParent();
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 2.0f, kEps);
}


// ============================================================
//  Moving the parent propagates to child world position
// ============================================================

TEST_F(SceneWorldTransformTest, MoveParent_ChildWorldPositionUpdates) {
    auto parent = makeAt(glm::vec3(0.0f, 0.0f, 0.0f));
    auto child  = makeAt(glm::vec3(1.0f, 0.0f, 0.0f));
    child.setParent(parent);
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 1.0f, kEps);

    parent.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    mScene->flushTransforms();
    EXPECT_NEAR(child.getWorldPosition().x, 6.0f, kEps);
}


// ============================================================
//  Destroy parent child graph node must not dangle
// ============================================================

TEST_F(SceneWorldTransformTest, DestroyParent_ChildIsAlsoDestroyed) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    child.setParent(parent);

    entt::entity childHandle = child.getHandle();
    parent.destroy();

    EXPECT_FALSE(mScene->getRegistry().valid(childHandle));
}

TEST_F(SceneWorldTransformTest, DestroyParent_GrandchildIsAlsoDestroyed) {
    auto grandparent = mScene->createEntity();
    auto parent      = mScene->createEntity();
    auto child       = mScene->createEntity();
    parent.setParent(grandparent);
    child.setParent(parent);

    entt::entity childHandle = child.getHandle();
    grandparent.destroy();

    EXPECT_FALSE(mScene->getRegistry().valid(childHandle));
}


// ============================================================
//  isAncestor / isDescendant
// ============================================================

TEST_F(SceneWorldTransformTest, IsAncestor_TrueForDirectParent) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    child.setParent(parent);

    EXPECT_TRUE(mScene->isAncestor(parent, child));
}

TEST_F(SceneWorldTransformTest, IsAncestor_TrueForGrandparent) {
    auto grandparent = mScene->createEntity();
    auto parent      = mScene->createEntity();
    auto child       = mScene->createEntity();
    parent.setParent(grandparent);
    child.setParent(parent);

    EXPECT_TRUE(mScene->isAncestor(grandparent, child));
}

TEST_F(SceneWorldTransformTest, IsAncestor_FalseForSibling) {
    auto parent   = mScene->createEntity();
    auto siblingA = mScene->createEntity();
    auto siblingB = mScene->createEntity();
    siblingA.setParent(parent);
    siblingB.setParent(parent);

    EXPECT_FALSE(mScene->isAncestor(siblingA, siblingB));
}

TEST_F(SceneWorldTransformTest, IsDescendant_TrueForDirectChild) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    child.setParent(parent);

    EXPECT_TRUE(mScene->isDescendant(child, parent));
}

TEST_F(SceneWorldTransformTest, IsDescendant_FalseForParent) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    child.setParent(parent);

    EXPECT_FALSE(mScene->isDescendant(parent, child));
}