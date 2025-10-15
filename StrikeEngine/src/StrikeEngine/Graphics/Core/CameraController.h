#pragma once
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Core/Input.h"
#include "StrikeEngine/Events/KeyCodes.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/MouseEvent.h"

namespace StrikeEngine {
    class CameraController {
    public:
        CameraController(Camera* camera = nullptr, float mouseSensitivity = 0.1f, float moveSpeed = 5.0f, float zoomSpeed = 1.0f)
            : m_Camera(camera),
            m_MouseSensitivity(mouseSensitivity),
            m_MoveSpeed(moveSpeed),
            m_ZoomSpeed(zoomSpeed),
            m_LastMouseX(0.0f),
            m_LastMouseY(0.0f),
            m_IsRightMouseButtonPressed(false)
        {
            if (m_Camera) {
                m_Camera->SetPosition(glm::vec3(0.0f, 6.0f, 4.0f));
            }
        }

        void OnEvent(Event& e);
        void OnUpdate(float deltaTime);

        // Getter for the camera
        Camera* GetCamera() { return m_Camera; }
        const Camera* GetCamera() const { return m_Camera; }

        // Setter for the camera
        void SetCamera(Camera* camera) {
            m_Camera = camera;
            if (m_Camera) {
                m_Camera->SetPosition(glm::vec3(0.0f, 6.0f, 4.0f));
            }
        }

        // Camera Movement Methods
        void MoveForward(float distance);
        void MoveBackward(float distance);
        void MoveLeft(float distance);
        void MoveRight(float distance);
        void MoveUp(float distance);
        void MoveDown(float distance);

        // Camera Rotation Methods
        void RotateYaw(float angle);
        void RotatePitch(float angle);

        // Mouse Events
        bool OnMouseScrolled(MouseScrolledEvent& e);
        glm::vec3 ScreenToWorldRay(float screenX, float screenY);

        // Zoom Methods
        void ZoomTowardsCursor(float zoomAmount);
        void ZoomInCameraDirection(float zoomAmount);

    private:
        bool OnMouseMoved(MouseMovedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

        Camera* m_Camera;  // Now a pointer instead of a reference
        float m_MouseSensitivity;
        float m_MoveSpeed;
        float m_ZoomSpeed;
        float m_LastMouseX;
        float m_LastMouseY;
        bool m_IsRightMouseButtonPressed;
    };
}