#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL3/SDL.h>

enum CurrentView { FRONT_LAWN, BACK_LAWN };

class Camera {
private:
    CurrentView m_view;
    float m_currentXOffset;
    float m_targetXOffset;

public:
    Camera(float screenWidth) 
        : m_view(FRONT_LAWN), m_currentXOffset(screenWidth), m_targetXOffset(screenWidth) {}

    void ToggleView(float screenWidth) {
        if (m_view == FRONT_LAWN) {
            m_view = BACK_LAWN;
            m_targetXOffset = 0.0f;
        } else {
            m_view = FRONT_LAWN;
            m_targetXOffset = screenWidth;
        }
    }

    void Update(float deltaTime) {
        m_currentXOffset += (m_targetXOffset - m_currentXOffset) * 12.0f * deltaTime;
    }

    float GetXOffset() const { return m_currentXOffset; }
    CurrentView GetView() const { return m_view; }
};

#endif