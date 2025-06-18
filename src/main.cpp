#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
// move these somewhere else
#include "EditorUI.h"
#include "GameObject.h"
#include "Components/TestComponent.h"
#include "Components/ScriptComponent.h"
#include "LuaInstance.h"
#include "SceneManager.h"
#include "Window.h"
#include "Renderer.h"
#include <Components/ScriptSwitcherComponent.h>

// SDL
void InitSDL()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}

void QuitSDL()
{
    SDL_Quit();
}

void Render(spark::Renderer &renderer, spark::SceneManager &sceneManager, spark::EditorUI &editorUI)
{
    // Clear first
    renderer.SetDrawColor(135, 206, 235, 255);
    renderer.Clear();

    // Render scene content
    sceneManager.Render();

    // Render UI on top
    editorUI.BeginFrame();
    editorUI.Render(sceneManager);
    editorUI.EndFrame(renderer.GetSDLRenderer());

    // Present final result
    renderer.Present();
}
int main(int argc, char *argv[])
{
    InitSDL();
    auto &window = spark::Window::GetInstance();
    auto &renderer = spark::Renderer::GetInstance();

    auto &lua = spark::LuaInstance::GetInstance();
    auto &sceneManager = spark::SceneManager::GetInstance();
    lua.Init();

    spark::EditorUI editorUI;
    editorUI.Init(window.GetSDLWindow(), renderer.GetSDLRenderer());

    auto scene = sceneManager.GetCurrentScene();
    if (scene)
    {
        auto go = scene->EmplaceGameObject("ScriptRunner");
        go->AddComponent<spark::ScriptComponent>("res/test.lua");

        auto scriptSwitcher = scene->EmplaceGameObject("ScriptSwitcher");
        std::vector<std::string> availableScripts = {
            "res/draggable_physics_sim.lua",
            "res/gravity_simulation.lua",
            "res/particles.lua",
            "res/wave_painter.lua",
            "res/flocking_boids.lua",
            "res/orbiting_particles.lua",
            "res/particle_fountain.lua",
        };
        scriptSwitcher->AddComponent<spark::ScriptSwitcherComponent>(availableScripts, go);

        auto go1 = scene->EmplaceGameObject("Hello");
        auto go2 = scene->EmplaceGameObject("World");
        go2->SetParent(go1);
    }

    // This is the equivalent of the "Start()" function in UNity
    sceneManager.Init();
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
            if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window.GetSDLWindow()))
                running = false;
            if (e.type == SDL_EVENT_WINDOW_RESIZED && e.window.windowID == SDL_GetWindowID(window.GetSDLWindow()))
            {
                window.OnResize(e.window.data1, e.window.data2);
            }
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
    QuitSDL();
    return 0;
}