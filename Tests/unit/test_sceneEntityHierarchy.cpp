#include <gtest/gtest.h>
#include <StrikeEngine.h>

class SceneEntityHierarchyTest : public ::testing::Test {
protected:
    void SetUp() override {
        Strike::Log::init();
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
//  Creating entities in the scene
// ============================================================

TEST_F(SceneEntityHierarchyTest, CreateEntity_IsValidInScene) {
    auto entity = mScene->createEntity();
    EXPECT_TRUE(entity.isValid());
    EXPECT_EQ(entity.getScene(), mScene);
}

TEST_F(SceneEntityHierarchyTest, CreateEntity_TwoEntitiesAreDistinct) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    EXPECT_NE(a, b);
    EXPECT_NE(a.getHandle(), b.getHandle());
}

TEST_F(SceneEntityHierarchyTest, CreateEntity_DefaultConstructedIsInvalid) {
    Strike::Entity empty;
    EXPECT_FALSE(empty.isValid());
    EXPECT_FALSE(static_cast<bool>(empty));
}

TEST_F(SceneEntityHierarchyTest, CreateEntityWithParent_AppearsInParentsChildList) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    auto children = parent.getChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children[0], child);
}

TEST_F(SceneEntityHierarchyTest, CreateEntityWithInvalidParent_CreatesRootEntity) {
    Strike::Entity invalid;
    auto entity = mScene->createEntity(invalid);
    EXPECT_TRUE(entity.isValid());
    EXPECT_TRUE(entity.isRoot());
}

TEST_F(SceneEntityHierarchyTest, GetEntityFromHandle_ReturnsEquivalentEntity) {
    auto entity = mScene->createEntity();
    auto found  = mScene->getEntityFromHandle(entity.getHandle());
    EXPECT_EQ(entity, found);
}

// ============================================================
//  Tag-based lookup
// ============================================================

TEST_F(SceneEntityHierarchyTest, GetEntityByTag_FindsCorrectEntity) {
    auto entity = mScene->createEntity();
    entity.setTag("Player");
    auto found = mScene->getEntity("Player");
    EXPECT_EQ(entity, found);
}

TEST_F(SceneEntityHierarchyTest, GetEntityByTag_MissingTagReturnsInvalid) {
    auto found = mScene->getEntity("DoesNotExist");
    EXPECT_FALSE(found.isValid());
}

TEST_F(SceneEntityHierarchyTest, GetEntityByTag_AfterTagChange_OldTagNoLongerFinds) {
    auto entity = mScene->createEntity();
    entity.setTag("OldName");
    entity.setTag("NewName");

    EXPECT_FALSE(mScene->getEntity("OldName").isValid());
    EXPECT_EQ(mScene->getEntity("NewName"), entity);
}

TEST_F(SceneEntityHierarchyTest, GetEntityByTag_MultipleEntities_FindsAMatch) {
    auto a = mScene->createEntity(); a.setTag("Shared");
    auto b = mScene->createEntity(); b.setTag("Shared");

    auto found = mScene->getEntity("Shared");
    EXPECT_TRUE(found.isValid());
    EXPECT_EQ(found.getTag(), "Shared");
}

// ============================================================
//  Destroying entities (using Scene::destroy)
// ============================================================

TEST_F(SceneEntityHierarchyTest, Destroy_HandleBecomesInvalidInRegistry) {
    auto entity = mScene->createEntity();
    auto handle = entity.getHandle();

    mScene->destroy(handle);

    EXPECT_FALSE(mScene->getRegistry().valid(handle));
}

TEST_F(SceneEntityHierarchyTest, Destroy_EntityNoLongerFoundByTag) {
    auto entity = mScene->createEntity();
    entity.setTag("Temp");
    
    mScene->destroy(entity.getHandle());

    EXPECT_FALSE(mScene->getEntity("Temp").isValid());
}

TEST_F(SceneEntityHierarchyTest, Destroy_ChildrenAreAlsoRemovedFromRegistry) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    auto childHandle = child.getHandle();
    mScene->destroy(parent.getHandle());

    EXPECT_FALSE(mScene->getRegistry().valid(childHandle));
}

TEST_F(SceneEntityHierarchyTest, Destroy_GrandchildrenAreAlsoRemovedFromRegistry) {
    auto grandparent = mScene->createEntity();
    auto parent      = mScene->createEntity(grandparent);
    auto child       = mScene->createEntity(parent);

    auto childHandle  = child.getHandle();
    auto parentHandle = parent.getHandle();
    mScene->destroy(grandparent.getHandle());

    EXPECT_FALSE(mScene->getRegistry().valid(parentHandle));
    EXPECT_FALSE(mScene->getRegistry().valid(childHandle));
}

TEST_F(SceneEntityHierarchyTest, Destroy_OnlyChild_ParentChildListBecomesEmpty) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    mScene->destroy(child.getHandle());

    EXPECT_TRUE(parent.getChildren().empty());
}

TEST_F(SceneEntityHierarchyTest, Destroy_OneOfMultipleChildren_OthersRemain) {
    auto parent = mScene->createEntity();
    auto c1 = mScene->createEntity(parent);
    auto c2 = mScene->createEntity(parent);
    auto c3 = mScene->createEntity(parent);

    mScene->destroy(c2.getHandle());

    auto remaining = parent.getChildren();
    ASSERT_EQ(remaining.size(), 2u);

    bool c1Found = false, c3Found = false;
    for (auto& c : remaining) {
        if (c == c1) c1Found = true;
        if (c == c3) c3Found = true;
    }
    EXPECT_TRUE(c1Found);
    EXPECT_TRUE(c3Found);
}

TEST_F(SceneEntityHierarchyTest, Destroy_SiblingIsUnaffected) {
    auto parent  = mScene->createEntity();
    auto sibling = mScene->createEntity(parent);
    auto target  = mScene->createEntity(parent);

    mScene->destroy(target.getHandle());

    EXPECT_TRUE(sibling.isValid());
}

// ============================================================
//  setParent / removeFromParent (using Scene::setParent)
// ============================================================

TEST_F(SceneEntityHierarchyTest, SetParent_ChildIsNoLongerRoot) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    
    mScene->setParent(child, parent);
    
    EXPECT_FALSE(child.isRoot());
}

TEST_F(SceneEntityHierarchyTest, SetParent_GetParentReturnsCorrectEntity) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    
    mScene->setParent(child, parent);
    
    EXPECT_EQ(child.getParent(), parent);
}

TEST_F(SceneEntityHierarchyTest, SetParent_AppearsInParentsChildList) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    
    mScene->setParent(child, parent);

    auto children = parent.getChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children[0], child);
}

TEST_F(SceneEntityHierarchyTest, SetParent_ToInvalidEntity_Asserts) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    EXPECT_DEATH(mScene->setParent(child, Strike::Entity{}), "");

    EXPECT_FALSE(child.isRoot());
    EXPECT_EQ(child.getParent(), parent);
}

TEST_F(SceneEntityHierarchyTest, SetParent_ToItself_Asserts) {
    auto entity = mScene->createEntity();
    EXPECT_DEATH(mScene->setParent(entity, entity), "");
}

TEST_F(SceneEntityHierarchyTest, SetParent_Circular_Asserts) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    mScene->setParent(b, a);
    EXPECT_DEATH(mScene->setParent(a, b), "");
}

TEST_F(SceneEntityHierarchyTest, SetParent_ReparentToNewParent_OldParentChildListUpdates) {
    auto oldParent = mScene->createEntity();
    auto newParent = mScene->createEntity();
    auto child     = mScene->createEntity();

    mScene->setParent(child, oldParent);
    mScene->setParent(child, newParent);

    EXPECT_TRUE(oldParent.getChildren().empty());
    ASSERT_EQ(newParent.getChildren().size(), 1u);
    EXPECT_EQ(newParent.getChildren()[0], child);
}

TEST_F(SceneEntityHierarchyTest, RemoveFromParent_EntityBecomesRoot) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    child.removeFromParent();

    EXPECT_TRUE(child.isRoot());
}

TEST_F(SceneEntityHierarchyTest, RemoveFromParent_GetParentReturnsInvalid) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    child.removeFromParent();

    EXPECT_FALSE(child.getParent().isValid());
}

TEST_F(SceneEntityHierarchyTest, RemoveFromParent_RemovedFromParentsChildList) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);

    child.removeFromParent();

    EXPECT_TRUE(parent.getChildren().empty());
}


// ============================================================
//  addChild (using Scene::addChild)
// ============================================================

TEST_F(SceneEntityHierarchyTest, AddChild_ChildAppearsInChildList) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    
    mScene->addChild(parent, child);

    auto children = parent.getChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children[0], child);
}

TEST_F(SceneEntityHierarchyTest, AddChild_MultipleChildren_AllPresent) {
    auto parent = mScene->createEntity();
    auto c1 = mScene->createEntity();
    auto c2 = mScene->createEntity();
    auto c3 = mScene->createEntity();
    
    mScene->addChild(parent, c1);
    mScene->addChild(parent, c2);
    mScene->addChild(parent, c3);

    EXPECT_EQ(parent.getChildren().size(), 3u);
}

TEST_F(SceneEntityHierarchyTest, AddChild_ChildIsNoLongerRoot) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity();
    
    mScene->addChild(parent, child);
    
    EXPECT_FALSE(child.isRoot());
}

TEST_F(SceneEntityHierarchyTest, GetChildren_EmptyForNewEntity) {
    auto entity = mScene->createEntity();
    EXPECT_TRUE(entity.getChildren().empty());
}

// ============================================================
//  isAncestor / isDescendant
// ============================================================

TEST_F(SceneEntityHierarchyTest, IsAncestor_TrueForDirectParent) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);
    
    EXPECT_TRUE(mScene->isAncestor(parent, child));
}

TEST_F(SceneEntityHierarchyTest, IsAncestor_TrueForGrandparent) {
    auto grandparent = mScene->createEntity();
    auto parent      = mScene->createEntity(grandparent);
    auto child       = mScene->createEntity(parent);

    EXPECT_TRUE(mScene->isAncestor(grandparent, child));
}

TEST_F(SceneEntityHierarchyTest, IsAncestor_FalseForUnrelatedEntities) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    EXPECT_FALSE(mScene->isAncestor(a, b));
}

TEST_F(SceneEntityHierarchyTest, IsAncestor_FalseForChildToParentDirection) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);
    
    EXPECT_FALSE(mScene->isAncestor(child, parent));
}

TEST_F(SceneEntityHierarchyTest, IsDescendant_TrueForDirectChild) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);
    
    EXPECT_TRUE(mScene->isDescendant(child, parent));
}

TEST_F(SceneEntityHierarchyTest, IsDescendant_TrueForGrandchild) {
    auto grandparent = mScene->createEntity();
    auto parent      = mScene->createEntity(grandparent);
    auto child       = mScene->createEntity(parent);

    EXPECT_TRUE(mScene->isDescendant(child, grandparent));
}

TEST_F(SceneEntityHierarchyTest, IsDescendant_FalseForParent) {
    auto parent = mScene->createEntity();
    auto child  = mScene->createEntity(parent);
    
    EXPECT_FALSE(mScene->isDescendant(parent, child));
}

TEST_F(SceneEntityHierarchyTest, IsDescendant_FalseForUnrelatedEntities) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    EXPECT_FALSE(mScene->isDescendant(a, b));
}

// ============================================================
//  Operators & identity
// ============================================================

TEST_F(SceneEntityHierarchyTest, EqualityOperator_CopiedEntityIsEqual) {
    auto a = mScene->createEntity();
    Strike::Entity b = a;
    EXPECT_TRUE(a == b);
}

TEST_F(SceneEntityHierarchyTest, InequalityOperator_DifferentEntitiesAreNotEqual) {
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    EXPECT_TRUE(a != b);
}

TEST_F(SceneEntityHierarchyTest, EntityUsableAsUnorderedMapKey) {
    std::unordered_map<Strike::Entity, int> map;
    auto a = mScene->createEntity();
    auto b = mScene->createEntity();
    map[a] = 10;
    map[b] = 20;

    EXPECT_EQ(map[a], 10);
    EXPECT_EQ(map[b], 20);
    EXPECT_EQ(map.size(), 2u);
}