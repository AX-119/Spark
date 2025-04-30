#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
// move these somewhere else
#include "GameObject.h"
#include "TestComponent.h"
#include "Components/ScriptComponent.h"
#include "LuaInstance.h"
// TODO: Create a concept of "scenes"
// TODO: Create a scene mananger
// TODO: Serialize scenes
std::vector<spark::GameObject *> g_gameObjects{};

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
void QuitSDL(SDL_Window *window)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// IMGUI
void InitImgui(SDL_Window *window, SDL_Renderer *renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}
void ImguiNewFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Render(SDL_Renderer *renderer)
{

    ImguiNewFrame();

    // IMGUI window rendering goes here
    for (auto &go : g_gameObjects)
    {
        go->RenderImGui();
    }

    ImGui::Render();

    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    // render game or whatever sdl has to handle here
    for (auto &go : g_gameObjects)
    {
        go->Render();
    }

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    InitSDL();
    auto window = CreateWindow();
    auto renderer = CreateRenderer(window);

    auto &lua = spark::LuaInstance::GetInstance();
    lua.Init();

    // lua.GetState().script(R"(
    //     -- Add your Lua script here
    //     local myVector = vec3(1.0, 2.0, 3.0)
    //     print("Initial vector:", tostring(myVector))

    //     local scaleFactor = 5.0
    //     local scaledVector = myVector * scaleFactor
    //     print("Scaled vector: ", scaledVector)
    // )");
    InitImgui(window, renderer);

    // game loop
    bool running = true;
    bool windowOpen = true;

    auto go = std::make_unique<spark::GameObject>();
    go->AddComponent<spark::TestComponent>();
    go->AddComponent<spark::ScriptComponent>("res/test.lua")->Init();
    g_gameObjects.emplace_back(go.get());
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT)
                running = false;
            if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        // input

        // update
        for (auto &go : g_gameObjects)
        {
            go->Update();
        }
        // render
        Render(renderer);
    }

    QuitSDL(window);
    return 0;
}