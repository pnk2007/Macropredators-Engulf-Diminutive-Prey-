#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "Entity.hpp"

class Zombie : public Entity {
private:
    float m_speedX;
    int m_row;
    int m_attackDamage;
    float m_attackTimer;
    const float ATTACK_INTERVAL = 0.5f;
    bool m_isEating;

public:
    Zombie(float x, float y, int row, float speedX)
        : Entity(x, y, 200), m_speedX(speedX), m_row(row), 
          m_attackDamage(20), m_attackTimer(0.0f), m_isEating(false) {}

    void Update(float deltaTime) override {
        if (!m_isEating) {
            m_x += m_speedX * deltaTime;
        }
    }

    void SetEating(bool eating) { m_isEating = eating; }

    bool CanAttack(float deltaTime) {
        if (!m_isEating) return false;
        m_attackTimer += deltaTime;
        if (m_attackTimer >= ATTACK_INTERVAL) {
            m_attackTimer = 0.0f;
            return true;
        }
        return false;
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset) override {
        SDL_FRect rect = { m_x - cameraXOffset + 5.0f, m_y + 5.0f, 60.0f, 80.0f };
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    int GetRow() const { return m_row; }
    int GetAttackDamage() const { return m_attackDamage; }
};

#endif