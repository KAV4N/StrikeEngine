#include "Sandbox3D.h"
#include "scripts/TankScript.h"
#include "scripts/PenguinScript.h"



Sandbox3D::Sandbox3D()
    : m_World(nullptr), m_LastFrameTime(0.0f)
{
    auto* world = StrikeEngine::World::Get();
    auto* scene = world->GetActiveScene();
    
    StrikeEngine::Model* tankModel = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj");
    StrikeEngine::Model* penguinModel = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/penguin/PenguinBaseMesh.obj");
   
    m_TestEntity1 = scene->CreateRenderableEntity(tankModel);
    auto& scriptTank = m_TestEntity1.AddComponent<StrikeEngine::NativeScriptComponent>();
    scriptTank.AddScript<TankScript>();
    
    m_TestEntity1.Destroy();
    /*
    m_TestEntity2 = scene->CreateRenderableEntity(penguinModel);
    auto& scriptPen = m_TestEntity2.AddComponent<StrikeEngine::NativeScriptComponent>();
    scriptPen.AddScript<PenguinScript>();
    */



    //m_TestEntity1.Destroy();
    /*
    StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(2.0f, 2.0f, 1.0f), glm::vec3(-0.5f, 0.2f, -0.5f),
        cos(glm::radians(60.0f)), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f
    );

    StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(0.0f, 2.0f, 1.0f), glm::vec3(0.0f, 0.2f, -1.f),
        cos(glm::radians(60.f)), glm::vec3(1.0f, 0.0f, 0.0f), 1000.0f
    );

    StrikeEngine::LightManager::Get()->CreateDirectionalLight(
        glm::vec3(0.f, 4.f, 6.f), glm::vec3(1.0f, 1.0f, 1.0f), 2.f
    );*/

    //TestEntityMethods();
}

void Sandbox3D::TestEntityMethods() {
    /*
    // Create entities
    StrikeEngine::Entity parentEntity = m_World->GetActiveScene()->CreateEntity();
    StrikeEngine::Entity childEntity1 = m_World->GetActiveScene()->CreateEntity();
    StrikeEngine::Entity childEntity2 = m_World->GetActiveScene()->CreateEntity();

    // 1. Set parent
    parentEntity.SetParent(parentEntity); // Test self-parenting (should do nothing)
    childEntity1.SetParent(parentEntity); // Set childEntity1's parent to parentEntity
    childEntity2.SetParent(parentEntity); // Set childEntity2's parent to parentEntity

    // 2. Get children
    std::vector<StrikeEngine::Entity> children = parentEntity.GetChildren();
    STRIKE_CORE_INFO(children.size() == 2); // Expect 2 children

    // 3. Check ancestors
    std::vector<StrikeEngine::Entity> ancestors = childEntity1.GetAncestors();
    STRIKE_CORE_INFO(ancestors.size() == 1 && ancestors[0] == parentEntity); // Expect 1 ancestor

    // 4. Remove child
 

    // 5. Check if is ancestor/descendant
    STRIKE_CORE_INFO(parentEntity.IsAncestorOf(childEntity2)); // parentEntity is an ancestor of childEntity2
    STRIKE_CORE_INFO(!childEntity1.IsAncestorOf(parentEntity)); // childEntity1 is not an ancestor of parentEntity
    STRIKE_CORE_INFO(childEntity2.IsDescendantOf(parentEntity)); // childEntity2 is a descendant of parentEntity

    // 6. Get all descendants
    std::vector<StrikeEngine::Entity> descendants = parentEntity.GetAllDescendants();
    STRIKE_CORE_INFO(descendants.size() == 1 && descendants[0] == childEntity2); // Only childEntity2 should be a descendant


    // 8. Set Parent again for further testing
    childEntity1.SetParent(parentEntity); // Add childEntity1 back
    childEntity2.SetParent(childEntity1); // Set childEntity2 as a child of childEntity1


    children = childEntity1.GetChildren();
    STRIKE_CORE_INFO(children.size() == 1 && children[0] == childEntity2); 

    // 10. Cleanup
    parentEntity.Destroy();
    */
}


void Sandbox3D::OnAttach() {
    m_World = StrikeEngine::World::Get();
    StrikeEngine::ScreenPanel* activeView = m_World->GetActiveScene()->GetActiveView();
    StrikeEngine::Camera* activeCamera = activeView->GetCamera();

    activeCamera->SetPosition(glm::vec3(0.0f, 2.f, 6.f));
    activeCamera->SetRotation(glm::vec3(0.f, -60.f, 0.f));
}

void Sandbox3D::OnDetach() {
    m_World = nullptr;
}

void Sandbox3D::OnUpdate(float deltaTime) {
   
}

void Sandbox3D::OnImGuiRender() {
    
}

void Sandbox3D::OnEvent(StrikeEngine::Event& event) {
   
}
