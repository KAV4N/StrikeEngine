#include "HierarchyExample.h"

// ─────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────

static std::string vec3Str(const glm::vec3& v) {
    return "(" + std::to_string(v.x) + ", " +
                 std::to_string(v.y) + ", " +
                 std::to_string(v.z) + ")";
}

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────

void HierarchyExample::onStart() {
    auto* scene = getEntity().getScene();

    mLabel = scene->createEntity();
    mLabel.setTag("showcase_label");

    auto& text = mLabel.addComponent<Strike::TextComponent>();
    text.setText("Entity Showcase — Press e btn to start");
    text.setColor(glm::uvec3(255, 220, 60));
    text.setPosition(glm::vec2(0.05f, 0.05f));
    text.setPivot(glm::vec2(0.0f, 0.0f));

    STRIKE_INFO("=== Entity Showcase ready. Press e btn to step through ===");
}

void HierarchyExample::onUpdate(float dt) {

}

void HierarchyExample::onEvent(Strike::Event& event) {
       if (event.getEventType() == Strike::EventType::KeyTyped) {
        auto& mouseEvent = static_cast<Strike::KeyEvent&>(event);
        if (mouseEvent.getKeyCode() == STRIKE_KEY_E) {
            advance();
        } 
        event.handled = true;
    }
}


// ─────────────────────────────────────────────────────────────
// Step Dispatcher
// ─────────────────────────────────────────────────────────────

void HierarchyExample::advance() {
    mStep++;
    printStep();

    switch (mStep) {
        case 0:  spawnRoot();       break;
        case 1:  changeColor();     break;
        case 2:  spawnChild();      break;
        case 3:  moveRoot();        break;
        case 4:  rotateRoot();      break;
        case 5:  scaleRoot();       break;
        case 6:  mRoot.setActive(false); break;
        case 7:  mRoot.setActive(true); break;
        case 8:  readHierarchy();   break;
        case 9:  reparentChild();   break;
        case 10: mChild.setActive(false); break;
        case 11: mChild.setActive(true); break;
        case 12: worldVsLocal();    break;
        case 13: tagLookup();       break;
        case 14: destroyEntities(); break;
        default:
            STRIKE_INFO("All steps complete!");
            updateLabel();
            mStep = -1;
            return;
    }

    updateLabel();
}

void HierarchyExample::printStep() {
    static const char* titles[] = {
        "Spawn root cube",
        "Change color",
        "Spawn child",
        "Move root",
        "Rotate root",
        "Scale root",
        "Toggle root active (off)",
        "Toggle root active (on)",
        "Read hierarchy",
        "Re-parent child (SWAP PARENT-CHILD)",
        "Toggle child active (off)",
        "Toggle child active (on)",
        "World vs Local",
        "Tag lookup",
        "Destroy entities"
    };

    if (mStep < 15)
        STRIKE_INFO("--- Step {0}: {1} ---", mStep, titles[mStep]);
}

void HierarchyExample::updateLabel() {
    if (!mLabel.isValid()) return;

    static const char* labels[] = {
        "Spawned root cube",
        "Color changed",
        "Spawned child",
        "Moved root",
        "Rotated root",
        "Scaled root",
        "Root deactivated",
        "Root activated",
        "Hierarchy printed",
        "Child re-parented (SWAPPED!)",
        "Child deactivated",
        "Child activated",
        "World vs local printed",
        "Tag lookup complete",
        "Destroyed entities",
        "=== Done ==="
    };

    int idx = glm::clamp(mStep, 0, 15);

    mLabel.getComponent<Strike::TextComponent>().setText(
        std::string("Entity Showcase  [e btn = next]\nStep ")
        + std::to_string(mStep) + ": "
        + labels[idx]
    );
}

// ─────────────────────────────────────────────────────────────
// Steps
// ─────────────────────────────────────────────────────────────

void HierarchyExample::spawnRoot() {
    auto* scene = getEntity().getScene();
    mRoot = scene->createEntity();

    mRoot.setPosition(glm::vec3(0));
    mRoot.setScale(glm::vec3(1));
    mRoot.setEulerAngles(glm::vec3(0));

    auto& renderer = mRoot.addComponent<Strike::RendererComponent>();
    renderer.setMesh("box", 0);
    renderer.setColor(glm::uvec3(255,255,255));

    STRIKE_INFO("Root spawned at origin.");
}

void HierarchyExample::changeColor() {
    if (!mRoot.isValid()) return;

    mRoot.getComponent<Strike::RendererComponent>()
         .setColor(glm::uvec3(220,50,50));

    STRIKE_INFO("Color changed to red.");
}

void HierarchyExample::spawnChild() {
    if (!mRoot.isValid()) return;

    auto* scene = getEntity().getScene();
    mChild = scene->createEntity(mRoot);

    mChild.setPosition(glm::vec3(3,0,0));
    mChild.setScale(glm::vec3(0.5f));

    auto& renderer = mChild.addComponent<Strike::RendererComponent>();
    renderer.setMesh("box", 0);
    renderer.setColor(glm::uvec3(60,180,255));

    STRIKE_INFO("Child spawned as child of root.");
}

void HierarchyExample::moveRoot() {
    if (!mRoot.isValid()) return;

    mRoot.setPosition(glm::vec3(5,0,0));
    mRoot.move(glm::vec3(0,2,0));

    STRIKE_INFO("Moved to {0}", vec3Str(mRoot.getPosition()));
}

void HierarchyExample::rotateRoot() {
    if (!mRoot.isValid()) return;

    mRoot.rotate(glm::vec3(0,45,0));
    mRoot.rotateQuaternion(15.0f, glm::vec3(1,0,0));

    STRIKE_INFO("Root rotated.");
}

void HierarchyExample::scaleRoot() {
    if (!mRoot.isValid()) return;

    mRoot.setScale(glm::vec3(2));
    STRIKE_INFO("Scaled to {0}", vec3Str(mRoot.getScale()));
}

void HierarchyExample::readHierarchy() {
    if (!mRoot.isValid() || !mChild.isValid()) return;

    STRIKE_INFO("Root children count: {0}", mRoot.getChildren().size());
    STRIKE_INFO("Child parent valid: {0}",
        mChild.getParent().isValid() ? "YES" : "NO");
}

void HierarchyExample::reparentChild() {
    if (!mRoot.isValid() || !mChild.isValid()) return;

    STRIKE_INFO("=== SWAPPING PARENT-CHILD RELATIONSHIP ===");
    
    // Store current world positions before swap
    glm::vec3 rootWorldPos = mRoot.getWorldPosition();
    glm::vec3 childWorldPos = mChild.getWorldPosition();
    
    STRIKE_INFO("Before swap - Root world pos: {0}", vec3Str(rootWorldPos));
    STRIKE_INFO("Before swap - Child world pos: {0}", vec3Str(childWorldPos));
    
    // Remove child from root's hierarchy
    mChild.removeFromParent();
    
    // Make root a child of the old child (swap relationship)
    mRoot.setParent(mChild);
    
    // Change colors to show the swap
    // Old parent (now child) becomes green
    mRoot.getComponent<Strike::RendererComponent>()
         .setColor(glm::uvec3(60, 255, 60));
    
    // Old child (now parent) becomes purple
    mChild.getComponent<Strike::RendererComponent>()
         .setColor(glm::uvec3(180, 60, 255));
    
    // Move the new parent (old child) to a new visible location
    mChild.setPosition(glm::vec3(-5, 3, 0));
    
    // Adjust the new child (old root) local position to make it visible
    mRoot.setPosition(glm::vec3(4, 0, 0));
    
    STRIKE_INFO("After swap - Child is now PARENT (purple) at: {0}", 
                vec3Str(mChild.getWorldPosition()));
    STRIKE_INFO("After swap - Root is now CHILD (green) at: {0}", 
                vec3Str(mRoot.getWorldPosition()));
    STRIKE_INFO("Parent-child relationship swapped successfully!");
}

void HierarchyExample::worldVsLocal() {
    if (!mChild.isValid() || !mRoot.isValid()) return;

    STRIKE_INFO("=== NEW PARENT (old child, purple) ===");
    STRIKE_INFO("Local: {0}", vec3Str(mChild.getPosition()));
    STRIKE_INFO("World: {0}", vec3Str(mChild.getWorldPosition()));
    
    STRIKE_INFO("=== NEW CHILD (old root, green) ===");
    STRIKE_INFO("Local: {0}", vec3Str(mRoot.getPosition()));
    STRIKE_INFO("World: {0}", vec3Str(mRoot.getWorldPosition()));
}

void HierarchyExample::tagLookup() {
    if (!mRoot.isValid()) return;

    mRoot.setTag("showcase_root");
    mChild.setTag("showcase_child");

    auto* scene = getEntity().getScene();
    auto found = scene->getEntity("showcase_root");

    STRIKE_INFO("Lookup success: {0}", found.isValid() ? "YES":"NO");
}

void HierarchyExample::destroyEntities() {
    if (mChild.isValid()) mChild.destroy();
    if (mRoot.isValid())  mRoot.destroy();

    STRIKE_INFO("Entities destroyed.");
}

REGISTER_SCRIPT(HierarchyExample);