#pragma once
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include "StrikeEngine/Core/Input.h"
#include "StrikeEngine/Events/KeyCodes.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/MouseEvent.h"

namespace StrikeEngine {
    class CameraSystem {
    public:
        static void OnEvent(Entity entity, Event& e);
        static void OnUpdate(Entity entity, float deltaTime);
        static void UpdateCamera(Entity entity);

        // Movement methods
        static void MoveForward(Entity entity, float distance);
        static void MoveBackward(Entity entity, float distance);
        static void MoveLeft(Entity entity, float distance);
        static void MoveRight(Entity entity, float distance);
        static void MoveUp(Entity entity, float distance);
        static void MoveDown(Entity entity, float distance);


        // Rotation methods
        static void RotateYaw(Entity entity, float angle);
        static void RotatePitch(Entity entity, float angle);

       
        static bool OnMouseScrolled(MouseScrolledEvent& e, Entity entity);

        static glm::vec3 ScreenToWorldRay(Entity entity, float screenX, float screenY);

        static void ZoomTowardsCursor(Entity entity, float zoomAmount);
        static void ZoomInCameraDirection(Entity entity, float zoomAmount);
        static void PanCamera(Entity entity);
    private:
        static bool OnMouseMoved(MouseMovedEvent& e, Entity entity);
        static bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        static bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);


        static float s_MouseSensitivity;
        static float s_MoveSpeed;
        static float s_LastMouseX;
        static float s_LastMouseY;
        static float s_ZoomSpeed;
        static bool s_IsRightMouseButtonPressed;
    };
}