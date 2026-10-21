#ifndef SUN_HPP
#define SUN_HPP

#include "Entity.hpp"

class Sun : public Entity {
private:
    float m_targetY;
    float m_lifeTimer;
    const float LIFE_TIME = 8.0f;

public:
    Sun(float x, float startY, float targetY)
        : Entity(x, startY, 1), m_targetY(targetY), m_lifeTimer(0.0f) {}

    void Update(float deltaTime) override {
        if (m_y < m_targetY) {
            m_y += 120.0f * deltaTime;
        } else {
            m_lifeTimer += deltaTime;
            if (m_lifeTimer >= LIFE_TIME) m_isAlive = false;
        }
    }

    bool IsClicked(float worldX, float worldY) {
        return (worldX >= m_x && worldX <= m_x + 35.0f &&
                worldY >= m_y && worldY <= m_y + 35.0f);
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset) override {
        SDL_FRect rect = { m_x - cameraXOffset, m_y, 35.0f, 35.0f };
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Vàng tươi
        SDL_RenderFillRect(renderer, &rect);
    }
};

#endif