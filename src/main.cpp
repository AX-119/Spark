#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
// move these somewhere else
#include "EditorUI.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "Components/ScriptComponent.h"
#include "LuaInstance.h"
#include "SceneManager.h"

// SDL
void InitSDL()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}

SDL_Window *CreateWindow()
{
    return SDL_CreateWindow("Spark", 1920, 1080, 0);
}
SDL_Renderer *CreateRenderer(SDL_Window *window, const char *name = nullptr)
{
    return SDL_CreateRenderer(window, name);
}
void QuitSDL(SDL_Window *window, SDL_Renderer *renderer)
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void Render(SDL_Renderer *renderer, spark::SceneManager &sceneManager, spark::EditorUI &editorUI)
{

    editorUI.BeginFrame();
    editorUI.Render(sceneManager);

    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    // render game or whatever sdl has to handle here
    sceneManager.Render();

    editorUI.EndFrame(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    InitSDL();
    auto window = CreateWindow();
    auto renderer = CreateRenderer(window);

    if (!window || !renderer)
    {
        std::cerr << "Failed to initialize SDL window or renderer: " << SDL_GetError() << std::endl;
        QuitSDL(window, renderer);
        return 1;
    }

    auto &lua = spark::LuaInstance::GetInstance();
    auto &sceneManager = spark::SceneManager::GetInstance();
    lua.Init();

    spark::EditorUI editorUI;
    editorUI.Init(window, renderer);

    auto scene = sceneManager.GetCurrentScene();
    if (scene)
    {
        auto go = scene->EmplaceGameObject();
        go->AddComponent<spark::TestComponent>();
        go->AddComponent<spark::ScriptComponent>("res/test.lua")->Init();

        auto go1 = scene->EmplaceGameObject("Hello");
        auto go2 = scene->EmplaceGameObject("World");
        go2->SetParent(go1);
    }

    // game loop
    bool running = true;
    Uint64 lastTime = SDL_GetPerformanceCounter();
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            editorUI.ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT)
                running = false;
            if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        // input

        // update
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = (currentTime - lastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
        lastTime = currentTime;
        sceneManager.Update(dt);
        // render
        Render(renderer, sceneManager, editorUI);
    }

    editorUI.Shutdown();
    QuitSDL(window, renderer);
    return 0;
}