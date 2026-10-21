#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Entity.hpp"

class Projectile : public Entity {
private:
    float m_speedX;
    int m_damage;
    int m_row;

public:
    Projectile(float x, float y, int row, float speedX, int damage = 20)
        : Entity(x, y, 1), m_speedX(speedX), m_damage(damage), m_row(row) {}

    void Update(float deltaTime) override {
        m_x += m_speedX * deltaTime;
        if (m_x < -100.0f || m_x > 2000.0f) m_isAlive = false;
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset) override {
        SDL_FRect rect = { m_x - cameraXOffset, m_y, 12.0f, 12.0f };
        SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    int GetRow() const { return m_row; }
    int GetDamage() const { return m_damage; }
};

#endif