#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

#include "Camera.hpp"
#include "LawnScene.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return -1;

    SDL_Window* window = SDL_CreateWindow("PvZ Dual Front - Header Only Engine", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    Camera camera((float)SCREEN_WIDTH);
    LawnScene backLawn(85.0f, false);
    LawnScene frontLawn((float)SCREEN_WIDTH + 85.0f, true);

    PlantType selectedTool = PEASHOOTER;
    int globalSun = 150; // Tiền khởi tạo
    bool isRunning = true;
    Uint64 lastTime = SDL_GetTicks();

    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) isRunning = false;
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_TAB || event.key.key == SDLK_SPACE) {
                    camera.ToggleView((float)SCREEN_WIDTH);
                }
                else if (event.key.key == SDLK_1) selectedTool = PEASHOOTER;
                else if (event.key.key == SDLK_2) selectedTool = SUNFLOWER;
                else if (event.key.key == SDLK_3) selectedTool = SHOVEL_TOOL; // Bấm 3 lấy Xẻng
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float worldX = event.button.x + camera.GetXOffset();
                float worldY = event.button.y;

                if (worldX >= SCREEN_WIDTH) frontLawn.HandleClick(worldX, worldY, selectedTool, globalSun);
                else backLawn.HandleClick(worldX, worldY, selectedTool, globalSun);
            }
        }

        // UPDATE
        camera.Update(deltaTime);
        frontLawn.Update(deltaTime);
        backLawn.Update(deltaTime);

        // RENDER
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        backLawn.Render(renderer, camera.GetXOffset(), {30, 100, 180, 255}, {50, 130, 210, 255});
        frontLawn.Render(renderer, camera.GetXOffset(), {34, 139, 34, 255}, {60, 179, 113, 255});

        // --- VẼ UI THANH TÀI NGUYÊN (STICKY BAR) ---
        SDL_FRect uiBar = { 10.0f, 10.0f, 320.0f, 40.0f };
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_RenderFillRect(renderer, &uiBar);

        // Đèn báo công cụ đang chọn (1: Đậu, 2: Hướng dương, 3: Xẻng)
        SDL_FRect indicator = { 20.0f, 17.0f, 25.0f, 25.0f };
        if (selectedTool == PEASHOOTER) SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        else if (selectedTool == SUNFLOWER) SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // Đỏ = Xẻng
        SDL_RenderFillRect(renderer, &indicator);

        // ĐÈN BÁO ĐỘNG SÂN BÊN KÌA CÓ NGUY HIỂM (DANGER ALERT)
        bool offScreenDanger = (camera.GetView() == FRONT_LAWN) ? backLawn.HasDanger() : frontLawn.HasDanger();
        if (offScreenDanger && (SDL_GetTicks() / 300) % 2 == 0) { // Nhấp nháy đỏ
            SDL_FRect alertRect = { SCREEN_WIDTH - 50.0f, 10.0f, 40.0f, 40.0f };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &alertRect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}