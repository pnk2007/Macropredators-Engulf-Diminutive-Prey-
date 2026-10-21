#ifndef LAWNSCENE_HPP
#define LAWNSCENE_HPP

#include "Plant.hpp"
#include "Zombie.hpp"
#include "Projectile.hpp"
#include "Sun.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdlib>

const int NUM_ROWS = 5;
const int NUM_COLS = 9;
const float CELL_WIDTH = 70.0f;
const float CELL_HEIGHT = 90.0f;
const float GRID_OFFSET_Y = 100.0f;

class LawnScene {
private:
    float m_startWorldX;
    bool m_isFrontLawn;
    std::shared_ptr<Plant> m_grid[NUM_ROWS][NUM_COLS];
    std::vector<std::shared_ptr<Plant>> m_plants;
    std::vector<std::shared_ptr<Zombie>> m_zombies;
    std::vector<std::shared_ptr<Projectile>> m_projectiles;
    std::vector<std::shared_ptr<Sun>> m_suns;

    float m_zombieSpawnTimer;
    float m_naturalSunTimer;

public:
    LawnScene(float startWorldX, bool isFrontLawn) 
        : m_startWorldX(startWorldX), m_isFrontLawn(isFrontLawn), 
          m_zombieSpawnTimer(0.0f), m_naturalSunTimer(0.0f) {
        for (int r = 0; r < NUM_ROWS; ++r) {
            for (int c = 0; c < NUM_COLS; ++c) {
                m_grid[r][c] = nullptr;
            }
        }
    }

    // Tương tác Click Chuột
    bool HandleClick(float worldX, float worldY, PlantType activeTool, int& globalSun) {
        // 1. Nhặt Mặt Trời
        for (auto& sun : m_suns) {
            if (sun->IsAlive() && sun->IsClicked(worldX, worldY)) {
                sun->Destroy();
                globalSun += 25;
                return true;
            }
        }

        // 2. Kiểm tra ô lưới
        if (worldX < m_startWorldX || worldX >= m_startWorldX + NUM_COLS * CELL_WIDTH) return false;
        if (worldY < GRID_OFFSET_Y || worldY >= GRID_OFFSET_Y + NUM_ROWS * CELL_HEIGHT) return false;

        int col = static_cast<int>((worldX - m_startWorldX) / CELL_WIDTH);
        int row = static_cast<int>((worldY - GRID_OFFSET_Y) / CELL_HEIGHT);

        // 3. Sử dụng Xẻng xóa cây
        if (activeTool == SHOVEL_TOOL) {
            if (m_grid[row][col] != nullptr) {
                m_grid[row][col]->Destroy();
                m_grid[row][col] = nullptr;
                return true;
            }
            return false;
        }

        // 4. Trồng cây
        if (m_grid[row][col] != nullptr) return false;

        int cost = (activeTool == PEASHOOTER) ? 100 : 50;
        if (globalSun < cost) return false;

        float posX = m_startWorldX + col * CELL_WIDTH;
        float posY = GRID_OFFSET_Y + row * CELL_HEIGHT;

        std::shared_ptr<Plant> newPlant = nullptr;
        if (activeTool == PEASHOOTER) {
            newPlant = std::make_shared<Peashooter>(posX, posY, row, col, m_isFrontLawn);
        } else if (activeTool == SUNFLOWER) {
            newPlant = std::make_shared<Sunflower>(posX, posY, row, col);
        }

        if (newPlant) {
            globalSun -= cost;
            m_grid[row][col] = newPlant;
            m_plants.push_back(newPlant);
            return true;
        }
        return false;
    }

    void SpawnZombie() {
        int row = rand() % NUM_ROWS;
        float spawnX = m_isFrontLawn ? (m_startWorldX + NUM_COLS * CELL_WIDTH) : (m_startWorldX - CELL_WIDTH);
        float spawnY = GRID_OFFSET_Y + row * CELL_HEIGHT;
        float speed = m_isFrontLawn ? -30.0f : 30.0f;

        m_zombies.push_back(std::make_shared<Zombie>(spawnX, spawnY, row, speed));
    }

    void SpawnNaturalSun() {
        int col = rand() % NUM_COLS;
        float spawnX = m_startWorldX + col * CELL_WIDTH + 15.0f;
        float targetY = GRID_OFFSET_Y + (rand() % NUM_ROWS) * CELL_HEIGHT + 20.0f;
        m_suns.push_back(std::make_shared<Sun>(spawnX, 0.0f, targetY));
    }

    bool HasDanger() const {
        for (const auto& zombie : m_zombies) {
            if (m_isFrontLawn && zombie->GetX() < m_startWorldX + 100.0f) return true;
            if (!m_isFrontLawn && zombie->GetX() > m_startWorldX + (NUM_COLS - 2) * CELL_WIDTH) return true;
        }
        return false;
    }

    void Update(float deltaTime) {
        // Sinh Zombie
        m_zombieSpawnTimer += deltaTime;
        if (m_zombieSpawnTimer >= 6.0f) {
            m_zombieSpawnTimer = 0.0f;
            SpawnZombie();
        }

        // Sinh Mặt trời tự nhiên
        m_naturalSunTimer += deltaTime;
        if (m_naturalSunTimer >= 9.0f) {
            m_naturalSunTimer = 0.0f;
            SpawnNaturalSun();
        }

        // Update Cây & Sinh Đạn/Mặt trời
        for (auto& plant : m_plants) {
            plant->Update(deltaTime);
            if (plant->GetType() == PEASHOOTER) {
                auto peashooter = std::static_pointer_cast<Peashooter>(plant);
                if (peashooter->CanShoot()) {
                    m_projectiles.push_back(peashooter->CreateProjectile(CELL_WIDTH, CELL_HEIGHT));
                }
            } else if (plant->GetType() == SUNFLOWER) {
                auto sunflower = std::static_pointer_cast<Sunflower>(plant);
                if (sunflower->CanProduceSun()) {
                    m_suns.push_back(sunflower->CreateSun());
                }
            }
        }

        // Update Zombie & Ăn cây
        for (auto& zombie : m_zombies) {
            bool hitPlant = false;
            for (auto& plant : m_plants) {
                if (plant->IsAlive() && zombie->GetRow() == plant->GetRow()) {
                    if (std::abs(zombie->GetX() - plant->GetX()) < 30.0f) {
                        hitPlant = true;
                        zombie->SetEating(true);
                        if (zombie->CanAttack(deltaTime)) {
                            plant->TakeDamage(zombie->GetAttackDamage());
                        }
                        break;
                    }
                }
            }
            if (!hitPlant) zombie->SetEating(false);
            zombie->Update(deltaTime);
        }

        // Update Đạn & Mặt trời
        for (auto& proj : m_projectiles) proj->Update(deltaTime);
        for (auto& sun : m_suns) sun->Update(deltaTime);

        // Va chạm Đạn - Zombie
        for (auto& proj : m_projectiles) {
            if (!proj->IsAlive()) continue;
            for (auto& zombie : m_zombies) {
                if (!zombie->IsAlive()) continue;
                if (proj->GetRow() == zombie->GetRow()) {
                    if (std::abs(proj->GetX() - zombie->GetX()) < 25.0f) {
                        zombie->TakeDamage(proj->GetDamage());
                        proj->Destroy();
                        break;
                    }
                }
            }
        }

        CleanupDeadEntities();
    }

    void CleanupDeadEntities() {
        for (auto it = m_plants.begin(); it != m_plants.end(); ) {
            if (!(*it)->IsAlive()) {
                m_grid[(*it)->GetRow()][(*it)->GetCol()] = nullptr;
                it = m_plants.erase(it);
            } else {
                ++it;
            }
        }

        m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
            [](const std::shared_ptr<Zombie>& z) { return !z->IsAlive(); }), m_zombies.end());

        m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::shared_ptr<Projectile>& p) { return !p->IsAlive(); }), m_projectiles.end());

        m_suns.erase(std::remove_if(m_suns.begin(), m_suns.end(),
            [](const std::shared_ptr<Sun>& s) { return !s->IsAlive(); }), m_suns.end());
    }

    void Render(SDL_Renderer* renderer, float cameraXOffset, SDL_Color bgColor, SDL_Color cellColor) {
        SDL_FRect bgRect = { m_startWorldX - 35.0f - cameraXOffset, 50.0f, 700.0f, 500.0f };
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &bgRect);

        for (int r = 0; r < NUM_ROWS; ++r) {
            for (int c = 0; c < NUM_COLS; ++c) {
                SDL_FRect cellRect = {
                    (m_startWorldX + c * CELL_WIDTH) - cameraXOffset,
                    GRID_OFFSET_Y + r * CELL_HEIGHT,
                    CELL_WIDTH - 2.0f, CELL_HEIGHT - 2.0f
                };
                SDL_SetRenderDrawColor(renderer, cellColor.r, cellColor.g, cellColor.b, cellColor.a);
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }

        for (auto& plant : m_plants) plant->Render(renderer, cameraXOffset);
        for (auto& zombie : m_zombies) zombie->Render(renderer, cameraXOffset);
        for (auto& proj : m_projectiles) proj->Render(renderer, cameraXOffset);
        for (auto& sun : m_suns) sun->Render(renderer, cameraXOffset);
    }
};

#endif