#include <gtest/gtest.h>
#include <StrikeEngine.h>

// ============================================================
class GameDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        Strike::Log::init();
        Strike::GameData::get().deleteAll();
    }

    void TearDown() override {
        Strike::GameData::get().deleteAll();
    }

    Strike::GameData& data = Strike::GameData::get();
};

// ============================================================
//  setInt / getInt
// ============================================================

TEST_F(GameDataTest, SetInt_StoresValue) {
    data.setInt("score", 100);
    EXPECT_EQ(data.getInt("score"), 100);
}

TEST_F(GameDataTest, SetInt_ReturnsStoredValue) {
    int returned = data.setInt("score", 42);
    EXPECT_EQ(returned, 42);
}

TEST_F(GameDataTest, SetInt_OverwritesExistingKey) {
    data.setInt("score", 10);
    data.setInt("score", 99);
    EXPECT_EQ(data.getInt("score"), 99);
}

TEST_F(GameDataTest, SetInt_NegativeValue) {
    data.setInt("temp", -500);
    EXPECT_EQ(data.getInt("temp"), -500);
}

TEST_F(GameDataTest, SetInt_ZeroValue) {
    data.setInt("zero", 0);
    EXPECT_EQ(data.getInt("zero"), 0);
}

TEST_F(GameDataTest, GetInt_MissingKeyAsserts) {
    EXPECT_DEATH(data.getInt("nonexistent"), "");
}

TEST_F(GameDataTest, GetInt_TypeMismatchAsserts) {
    data.setFloat("key", 1.0f);
    EXPECT_DEATH(data.getInt("key"), "");
}


// ============================================================
//  setFloat / getFloat
// ============================================================

TEST_F(GameDataTest, SetFloat_StoresValue) {
    data.setFloat("health", 0.75f);
    EXPECT_FLOAT_EQ(data.getFloat("health"), 0.75f);
}

TEST_F(GameDataTest, SetFloat_ReturnsStoredValue) {
    float returned = data.setFloat("health", 0.5f);
    EXPECT_FLOAT_EQ(returned, 0.5f);
}

TEST_F(GameDataTest, SetFloat_OverwritesExistingKey) {
    data.setFloat("health", 1.0f);
    data.setFloat("health", 0.25f);
    EXPECT_FLOAT_EQ(data.getFloat("health"), 0.25f);
}

TEST_F(GameDataTest, SetFloat_NegativeValue) {
    data.setFloat("temp", -3.14f);
    EXPECT_FLOAT_EQ(data.getFloat("temp"), -3.14f);
}

TEST_F(GameDataTest, SetFloat_ZeroValue) {
    data.setFloat("zero", 0.0f);
    EXPECT_FLOAT_EQ(data.getFloat("zero"), 0.0f);
}

TEST_F(GameDataTest, GetFloat_MissingKeyAsserts) {
    EXPECT_DEATH(data.getFloat("nonexistent"), "");
}

TEST_F(GameDataTest, GetFloat_TypeMismatchAsserts) {
    data.setInt("key", 1);
    EXPECT_DEATH(data.getFloat("key"), "");
}


// ============================================================
//  setDouble / getDouble
// ============================================================

TEST_F(GameDataTest, SetDouble_StoresValue) {
    data.setDouble("precision", 3.141592653589793);
    EXPECT_DOUBLE_EQ(data.getDouble("precision"), 3.141592653589793);
}

TEST_F(GameDataTest, SetDouble_ReturnsStoredValue) {
    double returned = data.setDouble("precision", 2.718281828);
    EXPECT_DOUBLE_EQ(returned, 2.718281828);
}

TEST_F(GameDataTest, SetDouble_OverwritesExistingKey) {
    data.setDouble("val", 1.1);
    data.setDouble("val", 2.2);
    EXPECT_DOUBLE_EQ(data.getDouble("val"), 2.2);
}

TEST_F(GameDataTest, GetDouble_MissingKeyAsserts) {
    EXPECT_DEATH(data.getDouble("nonexistent"), "");
}

TEST_F(GameDataTest, GetDouble_TypeMismatchAsserts) {
    data.setFloat("key", 1.0f);
    EXPECT_DEATH(data.getDouble("key"), "");
}


// ============================================================
//  setBool / getBool
// ============================================================

TEST_F(GameDataTest, SetBool_StoreTrueValue) {
    data.setBool("active", true);
    EXPECT_TRUE(data.getBool("active"));
}

TEST_F(GameDataTest, SetBool_StoreFalseValue) {
    data.setBool("active", false);
    EXPECT_FALSE(data.getBool("active"));
}

TEST_F(GameDataTest, SetBool_ReturnsStoredValue) {
    bool returned = data.setBool("flag", true);
    EXPECT_TRUE(returned);
}

TEST_F(GameDataTest, SetBool_OverwritesExistingKey) {
    data.setBool("flag", true);
    data.setBool("flag", false);
    EXPECT_FALSE(data.getBool("flag"));
}

TEST_F(GameDataTest, GetBool_MissingKeyAsserts) {
    EXPECT_DEATH(data.getBool("nonexistent"), "");
}

TEST_F(GameDataTest, GetBool_TypeMismatchAsserts) {
    data.setInt("key", 1);
    EXPECT_DEATH(data.getBool("key"), "");
}


// ============================================================
//  setString / getString
// ============================================================

TEST_F(GameDataTest, SetString_StoresValue) {
    data.setString("name", "Hero");
    EXPECT_EQ(data.getString("name"), "Hero");
}

TEST_F(GameDataTest, SetString_ReturnsStoredValue) {
    std::string returned = data.setString("name", "Hero");
    EXPECT_EQ(returned, "Hero");
}

TEST_F(GameDataTest, SetString_OverwritesExistingKey) {
    data.setString("name", "Hero");
    data.setString("name", "Villain");
    EXPECT_EQ(data.getString("name"), "Villain");
}

TEST_F(GameDataTest, SetString_EmptyString) {
    data.setString("empty", "");
    EXPECT_EQ(data.getString("empty"), "");
}

TEST_F(GameDataTest, SetString_SpecialCharacters) {
    data.setString("special", "Hello, World! @#$%");
    EXPECT_EQ(data.getString("special"), "Hello, World! @#$%");
}

TEST_F(GameDataTest, GetString_MissingKeyAsserts) {
    EXPECT_DEATH(data.getString("nonexistent"), "");
}

TEST_F(GameDataTest, GetString_TypeMismatchAsserts) {
    data.setInt("key", 1);
    EXPECT_DEATH(data.getString("key"), "");
}


// ============================================================
//  hasKey
// ============================================================

TEST_F(GameDataTest, HasKey_ReturnsTrueForExistingKey) {
    data.setInt("score", 10);
    EXPECT_TRUE(data.hasKey("score"));
}

TEST_F(GameDataTest, HasKey_ReturnsFalseForMissingKey) {
    EXPECT_FALSE(data.hasKey("nonexistent"));
}

TEST_F(GameDataTest, HasKey_ReturnsFalseAfterDelete) {
    data.setInt("score", 10);
    data.deleteKey("score");
    EXPECT_FALSE(data.hasKey("score"));
}

TEST_F(GameDataTest, HasKey_WorksForAllTypes) {
    data.setInt("i", 1);
    data.setFloat("f", 1.0f);
    data.setDouble("d", 1.0);
    data.setBool("b", true);
    data.setString("s", "text");

    EXPECT_TRUE(data.hasKey("i"));
    EXPECT_TRUE(data.hasKey("f"));
    EXPECT_TRUE(data.hasKey("d"));
    EXPECT_TRUE(data.hasKey("b"));
    EXPECT_TRUE(data.hasKey("s"));
}


// ============================================================
//  deleteKey
// ============================================================

TEST_F(GameDataTest, DeleteKey_RemovesExistingKey) {
    data.setInt("score", 10);
    data.deleteKey("score");
    EXPECT_FALSE(data.hasKey("score"));
}

TEST_F(GameDataTest, DeleteKey_NonExistentKeyDoesNotCrash) {
    EXPECT_NO_FATAL_FAILURE(data.deleteKey("ghost"));
}

TEST_F(GameDataTest, DeleteKey_OnlyRemovesTargetKey) {
    data.setInt("a", 1);
    data.setInt("b", 2);
    data.deleteKey("a");

    EXPECT_FALSE(data.hasKey("a"));
    EXPECT_TRUE(data.hasKey("b"));
    EXPECT_EQ(data.getInt("b"), 2);
}


// ============================================================
//  deleteAll
// ============================================================

TEST_F(GameDataTest, DeleteAll_ClearsAllEntries) {
    data.setInt("a", 1);
    data.setFloat("b", 2.0f);
    data.setString("c", "three");

    data.deleteAll();

    EXPECT_FALSE(data.hasKey("a"));
    EXPECT_FALSE(data.hasKey("b"));
    EXPECT_FALSE(data.hasKey("c"));
    EXPECT_EQ(data.getDataCount(), 0u);
}

TEST_F(GameDataTest, DeleteAll_OnEmptyDataDoesNotCrash) {
    EXPECT_NO_FATAL_FAILURE(data.deleteAll());
}

TEST_F(GameDataTest, DeleteAll_AllowsNewDataAfterClear) {
    data.setInt("score", 100);
    data.deleteAll();
    data.setInt("score", 200);

    EXPECT_EQ(data.getInt("score"), 200);
}


// ============================================================
//  getDataCount
// ============================================================

TEST_F(GameDataTest, GetDataCount_ZeroOnEmpty) {
    EXPECT_EQ(data.getDataCount(), 0u);
}

TEST_F(GameDataTest, GetDataCount_IncrementsOnAdd) {
    data.setInt("a", 1);
    EXPECT_EQ(data.getDataCount(), 1u);

    data.setFloat("b", 2.0f);
    EXPECT_EQ(data.getDataCount(), 2u);
}

TEST_F(GameDataTest, GetDataCount_NoChangeOnOverwrite) {
    data.setInt("a", 1);
    data.setInt("a", 2);
    EXPECT_EQ(data.getDataCount(), 1u);
}

TEST_F(GameDataTest, GetDataCount_DecrementsOnDelete) {
    data.setInt("a", 1);
    data.setInt("b", 2);
    data.deleteKey("a");
    EXPECT_EQ(data.getDataCount(), 1u);
}


// ============================================================
//  getAllKeys
// ============================================================

TEST_F(GameDataTest, GetAllKeys_EmptyOnFreshInstance) {
    EXPECT_TRUE(data.getAllKeys().empty());
}

TEST_F(GameDataTest, GetAllKeys_ContainsAllAddedKeys) {
    data.setInt("x", 1);
    data.setFloat("y", 2.0f);
    data.setString("z", "three");

    auto keys = data.getAllKeys();

    EXPECT_EQ(keys.size(), 3u);
    EXPECT_NE(std::find(keys.begin(), keys.end(), "x"), keys.end());
    EXPECT_NE(std::find(keys.begin(), keys.end(), "y"), keys.end());
    EXPECT_NE(std::find(keys.begin(), keys.end(), "z"), keys.end());
}

TEST_F(GameDataTest, GetAllKeys_EmptyAfterDeleteAll) {
    data.setInt("a", 1);
    data.deleteAll();
    EXPECT_TRUE(data.getAllKeys().empty());
}


// ============================================================
//  Cross-type isolation
// ============================================================

TEST_F(GameDataTest, MultipleTypes_StoredAndRetrievedIndependently) {
    data.setInt("i", 42);
    data.setFloat("f", 3.14f);
    data.setDouble("d", 2.718);
    data.setBool("b", true);
    data.setString("s", "hello");

    EXPECT_EQ(data.getInt("i"), 42);
    EXPECT_FLOAT_EQ(data.getFloat("f"), 3.14f);
    EXPECT_DOUBLE_EQ(data.getDouble("d"), 2.718);
    EXPECT_TRUE(data.getBool("b"));
    EXPECT_EQ(data.getString("s"), "hello");
    EXPECT_EQ(data.getDataCount(), 5u);
}

TEST_F(GameDataTest, OverwriteWithDifferentType_StoresNewType) {
    data.setInt("key", 1);
    data.setString("key", "overwritten");

    EXPECT_EQ(data.getString("key"), "overwritten");
    EXPECT_DEATH(data.getInt("key"), "");
}


// ============================================================
//  printAllData
// ============================================================

TEST_F(GameDataTest, PrintAllData_DoesNotCrashWhenEmpty) {
    EXPECT_NO_FATAL_FAILURE(data.printAllData());
}

TEST_F(GameDataTest, PrintAllData_DoesNotCrashWithData) {
    data.setInt("score", 100);
    data.setFloat("health", 0.9f);
    data.setBool("alive", true);
    data.setString("name", "Hero");
    EXPECT_NO_FATAL_FAILURE(data.printAllData());
}