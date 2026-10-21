#ifndef PLANT_HPP
#define PLANT_HPP

#include "Entity.hpp"
#include "Projectile.hpp"
#include "Sun.hpp"
#include <memory>

enum PlantType { PEASHOOTER, SUNFLOWER, SHOVEL_TOOL };

class Plant : public Entity {
protected:
    int m_row, m_col;
    PlantType m_type;

public:
    Plant(float x, float y, int row, int col, int health, PlantType type)
        : Entity(x, y, health), m_row(row), m_col(col), m_type(type) {}

    int GetRow() const { return m_row; }
    int GetCol() const { return m_col; }
    PlantType GetType() const { return m_type; }
};

class Peashooter : public Plant {
private:
    float m_shootTimer;
    const float SHOOT_INTERVAL = 1.5f;
    bool m_isFrontLawn;

public:
    Peashooter(float x, float y, int row, int col, bool isFrontLawn)
        : Plant(x, y, row, col, 300, PEASHOOTER), m_shootTimer(0.0f), m_isFrontLawn(isFrontLawn) {}

    void Update(float deltaTime) override {
        m_shootTimer += deltaTime;
    }

    bool CanShoot() {
        if (m_shootTimer >= SHOOT_INTERVAL) {
            m_shootTimer = 0.0f;
            return true;
        }
        return false;
    }

    std::shared_ptr<Projectile> CreateProjectile(float cellWidth, float cellHeight) {
        float bulletX = m_x + (m_isFrontLawn ? cellWidth : 0.0f);
        float bulletY = m_y + cellHeight / 2.0f - 6.0f;
        float speed = m_isFrontLawn ? 250.0f : -250.0f;
        return std::make_shared<Projectile>(bulletX, bulletY, m_row, speed);
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset) override {
        SDL_FRect rect = { m_x - cameraXOffset + 10.0f, m_y + 10.0f, 50.0f, 70.0f };
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

class Sunflower : public Plant {
private:
    float m_sunTimer;
    const float SUN_INTERVAL = 8.0f;

public:
    Sunflower(float x, float y, int row, int col)
        : Plant(x, y, row, col, 300, SUNFLOWER), m_sunTimer(0.0f) {}

    void Update(float deltaTime) override {
        m_sunTimer += deltaTime;
    }

    bool CanProduceSun() {
        if (m_sunTimer >= SUN_INTERVAL) {
            m_sunTimer = 0.0f;
            return true;
        }
        return false;
    }

    std::shared_ptr<Sun> CreateSun() {
        return std::make_shared<Sun>(m_x + 15.0f, m_y, m_y + 20.0f);
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset) override {
        SDL_FRect rect = { m_x - cameraXOffset + 10.0f, m_y + 10.0f, 50.0f, 70.0f };
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

#endif