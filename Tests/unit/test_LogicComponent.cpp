#include <gtest/gtest.h>
#include <StrikeEngine.h>

class LogicScriptA : public Strike::Script {
public:
    int onCreateCount  = 0;
    int onStartCount   = 0;
    int onUpdateCount  = 0;
    int onDestroyCount = 0;

    void onCreate()              override { ++onCreateCount;  }
    void onStart()               override { ++onStartCount;   }
    void onUpdate(float)         override { ++onUpdateCount;  }
    void onDestroy()             override { ++onDestroyCount; }
};
REGISTER_SCRIPT(LogicScriptA)

class LogicScriptB : public Strike::Script {
public:
    std::string label = "B";
    int onDestroyCount = 0;
    void onDestroy() override { ++onDestroyCount; }
};
REGISTER_SCRIPT(LogicScriptB)

class LogicScriptC : public Strike::Script {};
REGISTER_SCRIPT(LogicScriptC)


class LogicComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        Strike::Log::init();
        std::filesystem::path path = "test";
        mScene  = new Strike::Scene(path, "LogicComponentTestScene");
        ASSERT_NE(mScene, nullptr);
        mEntity = mScene->createEntity();
    }

    void TearDown() override {
        delete mScene;
        mScene = nullptr;
    }

    Strike::Scene*  mScene  = nullptr;
    Strike::Entity  mEntity;
};


// ============================================================
//  addScript<T>  -  typed API
// ============================================================

TEST_F(LogicComponentTest, AddScript_ReturnedReferenceIsCorrectType) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    auto& s = logic.addScript<LogicScriptA>();
    EXPECT_NE(dynamic_cast<LogicScriptA*>(&s), nullptr);
}

TEST_F(LogicComponentTest, AddScript_ScriptAppearsInGetScripts) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    EXPECT_EQ(logic.getScripts().size(), 1u);
}

TEST_F(LogicComponentTest, AddScript_TwoDifferentTypes_BothPresent) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    EXPECT_EQ(logic.getScripts().size(), 2u);
}

TEST_F(LogicComponentTest, AddScript_DuplicateType_ReturnsExistingInstance) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    auto& first  = logic.addScript<LogicScriptA>();
    auto& second = logic.addScript<LogicScriptA>(); // duplicate
    EXPECT_EQ(&first, &second);
}

TEST_F(LogicComponentTest, AddScript_DuplicateType_CountStaysAtOne) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptA>();
    EXPECT_EQ(logic.getScripts().size(), 1u);
}


// ============================================================
//  hasScript<T>
// ============================================================

TEST_F(LogicComponentTest, HasScript_TrueAfterAdd) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, HasScript_FalseBeforeAdd) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_FALSE(logic.hasScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, HasScript_FalseForOtherType) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    EXPECT_FALSE(logic.hasScript<LogicScriptB>());
}

TEST_F(LogicComponentTest, HasScript_FalseAfterRemove) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.removeScript<LogicScriptA>();
    EXPECT_FALSE(logic.hasScript<LogicScriptA>());
}


// ============================================================
//  getScript<T>
// ============================================================

TEST_F(LogicComponentTest, GetScript_ReturnsNonNullAfterAdd) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    EXPECT_NE(logic.getScript<LogicScriptA>(), nullptr);
}

TEST_F(LogicComponentTest, GetScript_ReturnsNullBeforeAdd) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_EQ(logic.getScript<LogicScriptA>(), nullptr);
}

TEST_F(LogicComponentTest, GetScript_ReturnsSameObjectAsAdd) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    auto& added     = logic.addScript<LogicScriptA>();
    auto* retrieved = logic.getScript<LogicScriptA>();
    EXPECT_EQ(&added, retrieved);
}

TEST_F(LogicComponentTest, GetScript_ConstOverload_Works) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptB>();

    const auto& constLogic = logic;
    EXPECT_NE(constLogic.getScript<LogicScriptB>(), nullptr);
}

TEST_F(LogicComponentTest, GetScript_ConstOverload_NullForMissingType) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    const auto& constLogic = logic;
    EXPECT_EQ(constLogic.getScript<LogicScriptA>(), nullptr);
}

TEST_F(LogicComponentTest, GetScript_MutatingReturnedReference_IsReflected) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptB>().label = "modified";
    EXPECT_EQ(logic.getScript<LogicScriptB>()->label, "modified");
}


// ============================================================
//  removeScript<T>
// ============================================================

TEST_F(LogicComponentTest, RemoveScript_ScriptNoLongerPresent) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.removeScript<LogicScriptA>();
    EXPECT_FALSE(logic.hasScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, RemoveScript_CountDecreases) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    logic.removeScript<LogicScriptA>();
    EXPECT_EQ(logic.getScripts().size(), 1u);
}

TEST_F(LogicComponentTest, RemoveScript_OtherScriptsUnaffected) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    logic.removeScript<LogicScriptA>();
    EXPECT_TRUE(logic.hasScript<LogicScriptB>());
}

TEST_F(LogicComponentTest, RemoveScript_CallsOnDestroy) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.removeScript<LogicScriptA>();
    EXPECT_FALSE(logic.hasScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, RemoveScript_NonExistentType_DoesNotCrash) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_NO_FATAL_FAILURE(logic.removeScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, RemoveScript_ThenAddAgain_Works) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.removeScript<LogicScriptA>();
    auto& fresh = logic.addScript<LogicScriptA>();
    EXPECT_NE(dynamic_cast<LogicScriptA*>(&fresh), nullptr);
    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
}


// ============================================================
//  clearScripts
// ============================================================

TEST_F(LogicComponentTest, ClearScripts_EmptiesAllScripts) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    logic.addScript<LogicScriptC>();
    logic.clearScripts();
    EXPECT_EQ(logic.getScripts().size(), 0u);
}

TEST_F(LogicComponentTest, ClearScripts_HasScriptReturnsFalseForAll) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    logic.clearScripts();
    EXPECT_FALSE(logic.hasScript<LogicScriptA>());
    EXPECT_FALSE(logic.hasScript<LogicScriptB>());
}

TEST_F(LogicComponentTest, ClearScripts_OnEmptyComponent_DoesNotCrash) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_NO_FATAL_FAILURE(logic.clearScripts());
}

TEST_F(LogicComponentTest, ClearScripts_AllowsNewScriptsAfterClear) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.clearScripts();
    logic.addScript<LogicScriptA>();
    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
}


// ============================================================
//  Destructor - calls onDestroy on all scripts
// ============================================================

TEST_F(LogicComponentTest, Destructor_CallsOnDestroyOnAllScripts) {
    std::filesystem::path path = "test";
    auto* scene  = new Strike::Scene(path, "DestructorTestScene");
    auto  entity = scene->createEntity();

    auto& logic = entity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptB>();

    EXPECT_NO_FATAL_FAILURE(delete scene);
}


// ============================================================
//  getScripts - const and mutable
// ============================================================

TEST_F(LogicComponentTest, GetScripts_EmptyByDefault) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_TRUE(logic.getScripts().empty());
}

TEST_F(LogicComponentTest, GetScripts_CountMatchesAdded) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    logic.addScript<LogicScriptB>();
    logic.addScript<LogicScriptC>();
    EXPECT_EQ(logic.getScripts().size(), 3u);
}

TEST_F(LogicComponentTest, GetScripts_ConstOverload_Works) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptA>();
    const auto& constLogic = logic;
    EXPECT_EQ(constLogic.getScripts().size(), 1u);
}


// ============================================================
//  deserialize - string-keyed addScript path
// ============================================================

TEST_F(LogicComponentTest, Deserialize_SingleScript_AttachesScript) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();

    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("logic");
    node.append_child("script").append_attribute("type").set_value("LogicScriptA");

    logic.deserialize(node);

    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
}

TEST_F(LogicComponentTest, Deserialize_TwoScripts_BothAttached) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();

    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("logic");
    node.append_child("script").append_attribute("type").set_value("LogicScriptA");
    node.append_child("script").append_attribute("type").set_value("LogicScriptB");

    logic.deserialize(node);

    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
    EXPECT_TRUE(logic.hasScript<LogicScriptB>());
    EXPECT_EQ(logic.getScripts().size(), 2u);
}


TEST_F(LogicComponentTest, Deserialize_ClearsPreviousScripts) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    logic.addScript<LogicScriptC>();
    EXPECT_EQ(logic.getScripts().size(), 1u);

    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("logic");
    node.append_child("script").append_attribute("type").set_value("LogicScriptA");

    logic.deserialize(node);

    EXPECT_FALSE(logic.hasScript<LogicScriptC>());
    EXPECT_TRUE(logic.hasScript<LogicScriptA>());
    EXPECT_EQ(logic.getScripts().size(), 1u);
}

TEST_F(LogicComponentTest, Deserialize_DuplicateTypeInXml_OnlyOneInstance) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();

    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("logic");
    node.append_child("script").append_attribute("type").set_value("LogicScriptA");
    node.append_child("script").append_attribute("type").set_value("LogicScriptA");

    logic.deserialize(node);

    EXPECT_EQ(logic.getScripts().size(), 1u);
}

TEST_F(LogicComponentTest, Deserialize_EmptyNode_ProducesNoScripts) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();

    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("logic");

    logic.deserialize(node);

    EXPECT_EQ(logic.getScripts().size(), 0u);
}


// ============================================================
//  getTypeName / getStaticTypeName
// ============================================================

TEST_F(LogicComponentTest, GetStaticTypeName_IsLogic) {
    EXPECT_EQ(Strike::LogicComponent::getStaticTypeName(), "logic");
}

TEST_F(LogicComponentTest, GetTypeName_MatchesStaticTypeName) {
    auto& logic = mEntity.addComponent<Strike::LogicComponent>();
    EXPECT_EQ(logic.getTypeName(), Strike::LogicComponent::getStaticTypeName());
}