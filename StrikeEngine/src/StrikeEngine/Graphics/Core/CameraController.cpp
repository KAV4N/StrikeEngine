#include "strikepch.h"
#include "CameraController.h"

namespace StrikeEngine {

    void CameraController::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(CameraController::OnMouseMoved));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(CameraController::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(CameraController::OnMouseButtonReleased));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(CameraController::OnMouseScrolled));
    }

    void CameraController::OnUpdate(float deltaTime) {
        if (!m_Camera) return;

        glm::vec3 moveDirection(0.0f);
        if (Input::IsKeyPressed(STRIKE_KEY_W))
            moveDirection += m_Camera->GetForwardDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_S))
            moveDirection -= m_Camera->GetForwardDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_A))
            moveDirection -= m_Camera->GetRightDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_D))
            moveDirection += m_Camera->GetRightDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_SPACE))
            moveDirection += m_Camera->GetUpDirection();
        if (Input::IsKeyPressed(STRIKE_KEY_LEFT_SHIFT))
            moveDirection -= m_Camera->GetUpDirection();

        if (glm::length(moveDirection) > 0.0f)
            moveDirection = glm::normalize(moveDirection);

        m_Camera->SetPosition(m_Camera->GetPosition() + moveDirection * m_MoveSpeed * deltaTime);
    }

    void CameraController::MoveForward(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetForwardDirection() * distance);
    }

    void CameraController::MoveBackward(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetForwardDirection() * distance);
    }

    void CameraController::MoveLeft(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetRightDirection() * distance);
    }

    void CameraController::MoveRight(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetRightDirection() * distance);
    }

    void CameraController::MoveUp(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetUpDirection() * distance);
    }

    void CameraController::MoveDown(float distance) {
        if (m_Camera) m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetUpDirection() * distance);
    }

    void CameraController::RotateYaw(float angle) {
        if (!m_Camera) return;
        glm::vec3 rotation = m_Camera->GetRotation();
        rotation.y += angle;
        m_Camera->SetRotation(rotation);
    }

    void CameraController::RotatePitch(float angle) {
        if (!m_Camera) return;
        glm::vec3 rotation = m_Camera->GetRotation();
        rotation.x += angle;
        rotation.x = glm::clamp(rotation.x, -89.0f, 89.0f);
        m_Camera->SetRotation(rotation);
    }

    bool CameraController::OnMouseMoved(MouseMovedEvent& e) {
        if (!m_IsRightMouseButtonPressed || !m_Camera) {
            return false;
        }

        float xOffset = e.GetX() - m_LastMouseX;
        float yOffset = m_LastMouseY - e.GetY();

        m_LastMouseX = e.GetX();
        m_LastMouseY = e.GetY();

        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;

        RotateYaw(xOffset);
        RotatePitch(yOffset);

        return false;
    }

    bool CameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            m_IsRightMouseButtonPressed = true;
            m_LastMouseX = Input::GetMouseX();
            m_LastMouseY = Input::GetMouseY();
            Input::SetCursorMode(CursorMode::Locked);
            return true;
        }
        return false;
    }

    bool CameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            m_IsRightMouseButtonPressed = false;
            Input::SetCursorMode(CursorMode::Normal);
            return true;
        }
        return false;
    }

    bool CameraController::OnMouseScrolled(MouseScrolledEvent& e) {
        if (!m_Camera) return false;
        float zoomAmount = e.GetYOffset() * m_ZoomSpeed;
        if (m_IsRightMouseButtonPressed) {
            ZoomInCameraDirection(zoomAmount);
        }
        else {
            ZoomTowardsCursor(zoomAmount);
        }
        return false;
    }

    glm::vec3 CameraController::ScreenToWorldRay(float screenX, float screenY) {
        if (!m_Camera) return glm::vec3(0.0f);

        //TODO: REPLACE 1280:720 WITH ACTUAL SIZE
        float ndcX = (2.0f * screenX) / 1280 - 1.0f;
        float ndcY = 1.0f - (2.0f * screenY) / 720;

        glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(m_Camera->GetProjectionMatrix()) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec3 rayWorld = glm::vec3(glm::inverse(m_Camera->GetViewMatrix()) * rayEye);
        rayWorld = glm::normalize(rayWorld);

        return rayWorld;
    }

    void CameraController::ZoomTowardsCursor(float zoomAmount) {
        if (!m_Camera) return;
        glm::vec3 rayDirection = ScreenToWorldRay(Input::GetMouseX(), Input::GetMouseY());
        m_Camera->SetPosition(m_Camera->GetPosition() + rayDirection * zoomAmount);
    }

    void CameraController::ZoomInCameraDirection(float zoomAmount) {
        if (!m_Camera) return;
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetForwardDirection() * zoomAmount);
    }

}