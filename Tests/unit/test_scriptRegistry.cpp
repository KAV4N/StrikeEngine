#include <gtest/gtest.h>
#include <StrikeEngine.h>

class TestScriptA : public Strike::Script {
public:
    int value = 0;
    void onCreate() override { value = 1; }
};
REGISTER_SCRIPT(TestScriptA)

class TestScriptB : public Strike::Script {
public:
    std::string tag = "b";
};
REGISTER_SCRIPT(TestScriptB)





// ============================================================
//  hasScriptFactory
// ============================================================

TEST(ScriptRegistryTest, HasFactory_TrueAfterMacroRegistration_A) {
    EXPECT_TRUE(Strike::ScriptRegistry::hasScriptFactory("TestScriptA"));
}

TEST(ScriptRegistryTest, HasFactory_TrueAfterMacroRegistration_B) {
    EXPECT_TRUE(Strike::ScriptRegistry::hasScriptFactory("TestScriptB"));
}

TEST(ScriptRegistryTest, HasFactory_FalseForUnknownClass) {
    EXPECT_FALSE(Strike::ScriptRegistry::hasScriptFactory("DoesNotExist"));
}

TEST(ScriptRegistryTest, HasFactory_FalseForEmptyString) {
    EXPECT_FALSE(Strike::ScriptRegistry::hasScriptFactory(""));
}

TEST(ScriptRegistryTest, HasFactory_CaseSensitive_LowercaseFails) {
    EXPECT_FALSE(Strike::ScriptRegistry::hasScriptFactory("testscripta"));
}

TEST(ScriptRegistryTest, HasFactory_CaseSensitive_UppercaseFails) {
    EXPECT_FALSE(Strike::ScriptRegistry::hasScriptFactory("TESTSCRIPTA"));
}


// ============================================================
//  createScript
// ============================================================

TEST(ScriptRegistryTest, CreateScript_ReturnsNonNullForRegisteredClass) {
    auto script = Strike::ScriptRegistry::createScript("TestScriptA");
    EXPECT_NE(script, nullptr);
}

TEST(ScriptRegistryTest, CreateScript_ReturnsCorrectType) {
    auto script = Strike::ScriptRegistry::createScript("TestScriptA");
    ASSERT_NE(script, nullptr);
    EXPECT_NE(dynamic_cast<TestScriptA*>(script.get()), nullptr);
}

TEST(ScriptRegistryTest, CreateScript_ReturnsNullptrForUnknownClass) {
    auto script = Strike::ScriptRegistry::createScript("DoesNotExist");
    EXPECT_EQ(script, nullptr);
}

TEST(ScriptRegistryTest, CreateScript_ReturnsNullptrForEmptyString) {
    auto script = Strike::ScriptRegistry::createScript("");
    EXPECT_EQ(script, nullptr);
}

TEST(ScriptRegistryTest, CreateScript_EachCallReturnsNewInstance) {
    auto a = Strike::ScriptRegistry::createScript("TestScriptA");
    auto b = Strike::ScriptRegistry::createScript("TestScriptA");
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a.get(), b.get());
}

TEST(ScriptRegistryTest, CreateScript_ReturnedInstanceIsDefaultConstructed) {
    auto script = Strike::ScriptRegistry::createScript("TestScriptA");
    ASSERT_NE(script, nullptr);
    auto* typed = dynamic_cast<TestScriptA*>(script.get());
    ASSERT_NE(typed, nullptr);
    EXPECT_EQ(typed->value, 0);
}

TEST(ScriptRegistryTest, CreateScript_ReturnsUniquePtr_OwnershipTransferred) {
    std::unique_ptr<Strike::Script> script =
        Strike::ScriptRegistry::createScript("TestScriptA");
    EXPECT_NE(script, nullptr);
}

TEST(ScriptRegistryTest, CreateScript_BothRegisteredClassesWork) {
    auto a = Strike::ScriptRegistry::createScript("TestScriptA");
    auto b = Strike::ScriptRegistry::createScript("TestScriptB");
    EXPECT_NE(dynamic_cast<TestScriptA*>(a.get()), nullptr);
    EXPECT_NE(dynamic_cast<TestScriptB*>(b.get()), nullptr);
}


// ============================================================
//  getRegisteredScripts
// ============================================================

TEST(ScriptRegistryTest, GetRegisteredScripts_IsNotEmpty) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    EXPECT_FALSE(scripts.empty());
}

TEST(ScriptRegistryTest, GetRegisteredScripts_ContainsTestScriptA) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    auto it = std::find(scripts.begin(), scripts.end(), "TestScriptA");
    EXPECT_NE(it, scripts.end());
}

TEST(ScriptRegistryTest, GetRegisteredScripts_ContainsTestScriptB) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    auto it = std::find(scripts.begin(), scripts.end(), "TestScriptB");
    EXPECT_NE(it, scripts.end());
}

TEST(ScriptRegistryTest, GetRegisteredScripts_NoDuplicates) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    auto sorted = scripts;
    std::sort(sorted.begin(), sorted.end());
    auto unique_end = std::unique(sorted.begin(), sorted.end());
    EXPECT_EQ(unique_end, sorted.end());
}

TEST(ScriptRegistryTest, GetRegisteredScripts_EveryEntryHasFactory) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    for (const auto& name : scripts) {
        EXPECT_TRUE(Strike::ScriptRegistry::hasScriptFactory(name))
            << "Factory missing for script: " << name;
    }
}

TEST(ScriptRegistryTest, GetRegisteredScripts_EveryEntryCanBeInstantiated) {
    auto scripts = Strike::ScriptRegistry::getRegisteredScripts();
    for (const auto& name : scripts) {
        auto instance = Strike::ScriptRegistry::createScript(name);
        EXPECT_NE(instance, nullptr)
            << "createScript returned nullptr for: " << name;
    }
}


// ============================================================
//  registerScriptFactory — duplicate registration asserts
// ============================================================

TEST(ScriptRegistryTest, RegisterFactory_DuplicateClassName_Asserts) {
    EXPECT_DEATH(
        Strike::ScriptRegistry::registerScriptFactory(
            "TestScriptA",
            []() -> std::unique_ptr<Strike::Script> {
                return std::make_unique<TestScriptA>();
            }
        ),
        ""
    );
}