#include "strikepch.h"
#include "CameraSystem.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Renderer/Renderer3D/Renderer.h"
namespace StrikeEngine {
    float CameraSystem::s_MouseSensitivity = 0.1f;
    float CameraSystem::s_MoveSpeed = 5.0f;
    float CameraSystem::s_ZoomSpeed = 1.0f;
    float CameraSystem::s_LastMouseX = 0.0f;
    float CameraSystem::s_LastMouseY = 0.0f;
    bool CameraSystem::s_IsRightMouseButtonPressed = false;

    void CameraSystem::OnEvent(Entity entity, Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>([entity](MouseMovedEvent& event) -> bool {
            return CameraSystem::OnMouseMoved(event, entity);
            });
        dispatcher.Dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& event) -> bool {
            return OnMouseButtonPressed(event);
            });
        dispatcher.Dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& event) -> bool {
            return OnMouseButtonReleased(event);
            });
        dispatcher.Dispatch<MouseScrolledEvent>([entity](MouseScrolledEvent& event) -> bool {
            return OnMouseScrolled(event, entity);
            });
    }

    void CameraSystem::OnUpdate(Entity entity, float deltaTime) {
        auto& camera = entity.GetComponent<CameraComponent>();
        auto& position = entity.GetComponent<PositionComponent>();
        auto& rotation = entity.GetComponent<RotationComponent>();


        glm::vec3 moveDirection(0.0f);
        if (Input::IsKeyPressed(STRIKE_KEY_W))
            moveDirection += camera.GetForwardDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_S))
            moveDirection -= camera.GetForwardDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_A))
            moveDirection -= camera.GetRightDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_D))
            moveDirection += camera.GetRightDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_SPACE))
            moveDirection += camera.GetUpDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_LEFT_SHIFT))
            moveDirection -= camera.GetUpDirection();

        if (glm::length(moveDirection) > 0.0f)
            moveDirection = glm::normalize(moveDirection);

        position.position += moveDirection * s_MoveSpeed * deltaTime;
        

        UpdateCamera(entity);
    }

    bool CameraSystem::OnMouseMoved(MouseMovedEvent& e, Entity entity) {
        if (!s_IsRightMouseButtonPressed) {
            return false;
        }

        float xOffset = e.GetX() - s_LastMouseX;
        float yOffset = s_LastMouseY - e.GetY();

        s_LastMouseX = e.GetX();
        s_LastMouseY = e.GetY();

        xOffset *= s_MouseSensitivity;
        yOffset *= s_MouseSensitivity;

        auto& rotation = entity.GetComponent<RotationComponent>();
        rotation.rotation.y += xOffset;
        rotation.rotation.x += yOffset;

        // Clamp the pitch to avoid flipping
        if (rotation.rotation.x > 89.0f)
            rotation.rotation.x = 89.0f;
        if (rotation.rotation.x < -89.0f)
            rotation.rotation.x = -89.0f;

        return false;
    }

    bool CameraSystem::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            s_IsRightMouseButtonPressed = true;
            s_LastMouseX = Input::GetMouseX();
            s_LastMouseY = Input::GetMouseY();
            Input::SetCursorMode(CursorMode::Locked);
            return true;
        }
        return false;
    }

    bool CameraSystem::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            s_IsRightMouseButtonPressed = false;
            Input::SetCursorMode(CursorMode::Normal);
            return true;
        }
        return false;
    }


    void CameraSystem::UpdateCamera(Entity entity) {
        auto& camera = entity.GetComponent<CameraComponent>();
        auto& position = entity.GetComponent<PositionComponent>();
        auto& rotation = entity.GetComponent<RotationComponent>();
        camera.UpdateViewMatrix(position.position, rotation.rotation);
        camera.UpdateClusters(position.position);
    }

    void CameraSystem::RotatePitch(Entity entity, float angle) {
        auto& rotation = entity.GetComponent<RotationComponent>();
        rotation.rotation.x += angle;
        // Clamp the pitch to avoid flipping
        if (rotation.rotation.x > 89.0f)
            rotation.rotation.x = 89.0f;
        if (rotation.rotation.x < -89.0f)
            rotation.rotation.x = -89.0f;
    }
    bool CameraSystem::OnMouseScrolled(MouseScrolledEvent& e, Entity entity) {
        float zoomAmount = e.GetYOffset() * s_ZoomSpeed;
        if (s_IsRightMouseButtonPressed) {
            ZoomInCameraDirection(entity, zoomAmount);
        }
        else {
            ZoomTowardsCursor(entity, zoomAmount);
        }
        return false;
    }

    glm::vec3 CameraSystem::ScreenToWorldRay(Entity entity, float screenX, float screenY) {
        auto& camera = entity.GetComponent<CameraComponent>();
        auto& position = entity.GetComponent<PositionComponent>();

        // Convert screen coordinates to normalized device coordinates
        float ndcX = (2.0f * screenX) / Renderer::GetWidth() - 1.0f;
        float ndcY = 1.0f - (2.0f * screenY) / Renderer::GetHeight();

        // Create the ray in view space
        glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(camera.ProjectionMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        // Transform the ray to world space
        glm::vec3 rayWorld = glm::vec3(glm::inverse(camera.ViewMatrix) * rayEye);
        rayWorld = glm::normalize(rayWorld);

        return rayWorld;
    }

    void CameraSystem::ZoomTowardsCursor(Entity entity, float zoomAmount) {
        auto& camera = entity.GetComponent<CameraComponent>();
        auto& position = entity.GetComponent<PositionComponent>();

        glm::vec3 rayDirection = ScreenToWorldRay(entity, Input::GetMouseX(), Input::GetMouseY());

        position.position += rayDirection * zoomAmount;

        UpdateCamera(entity);
    }
    void CameraSystem::ZoomInCameraDirection(Entity entity, float zoomAmount) {
        auto& camera = entity.GetComponent<CameraComponent>();
        auto& position = entity.GetComponent<PositionComponent>();

        glm::vec3 forwardDirection = camera.GetForwardDirection();

        position.position += forwardDirection * zoomAmount;

        UpdateCamera(entity);
    }
}