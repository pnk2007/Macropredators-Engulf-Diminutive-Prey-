#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL3/SDL.h>

class Entity {
protected:
    float m_x, m_y;
    int m_health;
    bool m_isAlive;

public:
    Entity(float x, float y, int health) 
        : m_x(x), m_y(y), m_health(health), m_isAlive(true) {}
    
    virtual ~Entity() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer, float cameraXOffset) = 0;

    bool IsAlive() const { return m_isAlive; }
    void Destroy() { m_isAlive = false; }
    
    void TakeDamage(int damage) {
        m_health -= damage;
        if (m_health <= 0) m_isAlive = false;
    }

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
};

#endif