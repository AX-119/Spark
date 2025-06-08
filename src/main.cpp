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
#include "Window.h"
#include "Renderer.h"

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

    editorUI.BeginFrame();
    editorUI.Render(sceneManager);

    renderer.SetDrawColor(100, 149, 237, 255);
    renderer.Clear();

    // render game or whatever sdl has to handle here
    sceneManager.Render();

    editorUI.EndFrame(renderer.GetSDLRenderer());

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
        auto go = scene->EmplaceGameObject();
        go->AddComponent<spark::ScriptComponent>("res/test.lua");

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