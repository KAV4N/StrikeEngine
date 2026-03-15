#include <gtest/gtest.h>
#include <StrikeEngine.h>

class ComponentRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        Strike::Log::init();
        std::filesystem::path path = "test";
        mScene = new Strike::Scene(path, "ComponentRegistryTestScene");
        ASSERT_NE(mScene, nullptr);
        mEntity = mScene->createEntity();
    }

    void TearDown() override {
        delete mScene;
        mScene = nullptr;
    }

    Strike::Scene* mScene = nullptr;
    Strike::Entity mEntity;
};


// ============================================================
//  hasComponentFactory
// ============================================================

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredCamera) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("camera"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredRenderer) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("renderer"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredPhysics) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("physics"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredLight) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("light"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredText) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("text"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredAudioSource) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("audiosource"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredAudioListener) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("audiolistener"));
}

TEST_F(ComponentRegistryTest, HasFactory_TrueForRegisteredLogic) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory("logic"));
}

TEST_F(ComponentRegistryTest, HasFactory_FalseForUnknownType) {
    EXPECT_FALSE(Strike::ComponentRegistry::hasComponentFactory("doesnotexist"));
}

TEST_F(ComponentRegistryTest, HasFactory_FalseForEmptyString) {
    EXPECT_FALSE(Strike::ComponentRegistry::hasComponentFactory(""));
}

TEST_F(ComponentRegistryTest, HasFactory_CaseSensitive_UppercaseFails) {
    EXPECT_FALSE(Strike::ComponentRegistry::hasComponentFactory("Camera"));
}

TEST_F(ComponentRegistryTest, HasFactory_CaseSensitive_MixedCaseFails) {
    EXPECT_FALSE(Strike::ComponentRegistry::hasComponentFactory("LIGHT"));
}


// ============================================================
//  getRegisteredComponents
// ============================================================

TEST_F(ComponentRegistryTest, GetRegisteredComponents_IsNotEmpty) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    EXPECT_FALSE(components.empty());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsCamera) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "camera");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsRenderer) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "renderer");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsPhysics) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "physics");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsLight) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "light");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsText) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "text");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsAudioSource) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "audiosource");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsAudioListener) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "audiolistener");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_ContainsLogic) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(components.begin(), components.end(), "logic");
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_NoDuplicates) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto sorted = components;
    std::sort(sorted.begin(), sorted.end());
    auto unique_end = std::unique(sorted.begin(), sorted.end());
    EXPECT_EQ(unique_end, sorted.end());
}

TEST_F(ComponentRegistryTest, GetRegisteredComponents_CountMatchesHasFactory) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    for (const auto& name : components) {
        EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory(name))
            << "Factory missing for component: " << name;
    }
}


// ============================================================
//  addComponentToEntity
// ============================================================

TEST_F(ComponentRegistryTest, AddComponentToEntity_Light_AddsComponent) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(mEntity, "light", node);
    EXPECT_TRUE(mEntity.hasComponent<Strike::LightComponent>());
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_Text_AddsComponent) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(mEntity, "text", node);
    EXPECT_TRUE(mEntity.hasComponent<Strike::TextComponent>());
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_AudioListener_AddsComponent) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(mEntity, "audiolistener", node);
    EXPECT_TRUE(mEntity.hasComponent<Strike::AudioListenerComponent>());
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_Logic_AddsComponent) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(mEntity, "logic", node);
    EXPECT_TRUE(mEntity.hasComponent<Strike::LogicComponent>());
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_UnknownType_DoesNotCrash) {
    pugi::xml_node node;
    EXPECT_NO_FATAL_FAILURE(
        Strike::ComponentRegistry::addComponentToEntity(mEntity, "doesnotexist", node)
    );
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_UnknownType_NoComponentAdded) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(mEntity, "doesnotexist", node);
    EXPECT_FALSE(mEntity.hasComponent<Strike::LightComponent>());
    EXPECT_FALSE(mEntity.hasComponent<Strike::LogicComponent>());
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_EmptyTypeName_DoesNotCrash) {
    pugi::xml_node node;
    EXPECT_NO_FATAL_FAILURE(
        Strike::ComponentRegistry::addComponentToEntity(mEntity, "", node)
    );
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_WithLightXmlAttributes_SetsValues) {
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("light");
    node.append_attribute("intensity").set_value("7.5");
    node.append_attribute("radius").set_value("20.0");
    node.append_attribute("color").set_value("255,0,0");

    Strike::ComponentRegistry::addComponentToEntity(mEntity, "light", node);

    ASSERT_TRUE(mEntity.hasComponent<Strike::LightComponent>());
    auto& light = mEntity.getComponent<Strike::LightComponent>();
    EXPECT_FLOAT_EQ(light.getIntensity(), 7.5f);
    EXPECT_FLOAT_EQ(light.getRadius(), 20.0f);
    EXPECT_EQ(light.getColor(), glm::uvec3(255, 0, 0));
}

TEST_F(ComponentRegistryTest, AddComponentToEntity_WithTextXmlAttributes_SetsValues) {
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("text");
    node.append_attribute("text").set_value("Hello");
    node.append_attribute("color").set_value("255,255,0");
    node.append_attribute("x").set_value("0.5");
    node.append_attribute("y").set_value("0.5");

    Strike::ComponentRegistry::addComponentToEntity(mEntity, "text", node);

    ASSERT_TRUE(mEntity.hasComponent<Strike::TextComponent>());
    auto& text = mEntity.getComponent<Strike::TextComponent>();
    EXPECT_EQ(text.getText(), "Hello");
    EXPECT_EQ(text.getColor(), glm::uvec3(255, 255, 0));
}


// ============================================================
//  registerComponentFactory - registering a custom component
// ============================================================

class TestRegistrationComponent : public Strike::Component {
public:
    float value = 0.0f;

    static const std::string& getStaticTypeName() {
        static std::string name = "test_registration_component";
        return name;
    }

    const std::string& getTypeName() const override {
        return getStaticTypeName();
    }

    void deserialize(const pugi::xml_node& node) override {
        value = node.attribute("value").as_float(0.0f);
    }
};

REGISTER_COMPONENT(TestRegistrationComponent)

TEST_F(ComponentRegistryTest, RegisterComponent_Macro_HasFactoryReturnsTrue) {
    EXPECT_TRUE(Strike::ComponentRegistry::hasComponentFactory(
        TestRegistrationComponent::getStaticTypeName()
    ));
}

TEST_F(ComponentRegistryTest, RegisterComponent_Macro_AppearsInGetRegisteredComponents) {
    auto components = Strike::ComponentRegistry::getRegisteredComponents();
    auto it = std::find(
        components.begin(), components.end(),
        TestRegistrationComponent::getStaticTypeName()
    );
    EXPECT_NE(it, components.end());
}

TEST_F(ComponentRegistryTest, RegisterComponent_Macro_AddComponentToEntityWorks) {
    pugi::xml_node node;
    Strike::ComponentRegistry::addComponentToEntity(
        mEntity,
        TestRegistrationComponent::getStaticTypeName(),
        node
    );
    EXPECT_TRUE(mEntity.hasComponent<TestRegistrationComponent>());
}

TEST_F(ComponentRegistryTest, RegisterComponent_Macro_DeserializePassesThroughXmlAttributes) {
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child(
        TestRegistrationComponent::getStaticTypeName().c_str()
    );
    node.append_attribute("value").set_value("42.5");

    Strike::ComponentRegistry::addComponentToEntity(
        mEntity,
        TestRegistrationComponent::getStaticTypeName(),
        node
    );

    ASSERT_TRUE(mEntity.hasComponent<TestRegistrationComponent>());
    EXPECT_FLOAT_EQ(
        mEntity.getComponent<TestRegistrationComponent>().value,
        42.5f
    );
}

TEST_F(ComponentRegistryTest, RegisterComponent_Macro_DeserializeDefaultsToZeroOnEmptyNode) {
    pugi::xml_node node;

    Strike::ComponentRegistry::addComponentToEntity(
        mEntity,
        TestRegistrationComponent::getStaticTypeName(),
        node
    );

    ASSERT_TRUE(mEntity.hasComponent<TestRegistrationComponent>());
    EXPECT_FLOAT_EQ(
        mEntity.getComponent<TestRegistrationComponent>().value,
        0.0f
    );
}


// ============================================================
//  registerComponentFactory - duplicate registration asserts
// ============================================================

TEST_F(ComponentRegistryTest, RegisterFactory_DuplicateTypeName_Asserts) {
    EXPECT_DEATH(
        Strike::ComponentRegistry::registerComponentFactory(
            "camera",
            [](Strike::Entity&, const pugi::xml_node&) -> Strike::Component* {
                return nullptr;
            }
        ),
        ""
    );
}