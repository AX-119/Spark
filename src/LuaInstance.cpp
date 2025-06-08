#include "LuaInstance.h"
#include <Component.h>
#include <glm/glm.hpp>
#include <Components/TransformComponent.h>
#include <Components/ScriptComponent.h>
#include <Window.h>
#include <Renderer.h>

namespace spark
{
    void LuaInstance::Init()
    {
        m_Lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);
        SetupBindings();
    }
    void LuaInstance::SetupBindings()
    {

        // Set the global lua space variables
        // (this tells the lua space e.g. what a "GameObject" is and what functions it can call)
        // format: "NameInLua", functionptr
        m_Lua.new_usertype<spark::Component>("Component", sol::no_constructor, "GetParent", &spark::Component::GetParent);

        m_Lua.new_usertype<glm::vec3>(
            "vec3",
            sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b)
            { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b)
            { return a - b; },
            sol::meta_function::multiplication, sol::overload([](const glm::vec3 &v, float s)
                                                              { return v * s; }, [](float s, const glm::vec3 &v)
                                                              { return s * v; }),
            sol::meta_function::division, [](const glm::vec3 &v, float s)
            { return v / s; },
            "length", [](const glm::vec3 &v)
            { return glm::length(v); },
            "normalize", [](const glm::vec3 &v)
            { return glm::normalize(v); },
            sol::meta_function::to_string, [](const glm::vec3 &v)
            { return "{ x= " + std::to_string(v.x) + ", y= " + std::to_string(v.y) + ", z= " + std::to_string(v.z) + " }"; });

        m_Lua.new_usertype<glm::quat>(
            "quat", sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
            "x", &glm::quat::x,
            "y", &glm::quat::y,
            "z", &glm::quat::z,
            "w", &glm::quat::w,
            sol::meta_function::to_string, [](const glm::quat &q)
            { return "{ w=" + std::to_string(q.w) + "x= " + std::to_string(q.x) + ", y= " + std::to_string(q.y) + ", z= " + std::to_string(q.z) + " }"; },
            sol::meta_function::multiplication, [](const glm::quat &a, const glm::quat &b)
            { return a * b; });

        m_Lua.new_usertype<spark::Window>("Window",
                                          sol::no_constructor,

                                          // Window properties (getters)
                                          "get_title", &spark::Window::GetTitle,
                                          "get_width", &spark::Window::GetWidth,
                                          "get_height", &spark::Window::GetHeight,
                                          "get_window_position", &spark::Window::GetWindowPosition,
                                          //"get_sdl_window", &spark::Window::GetSDLWindow,

                                          // Window properties (setters)
                                          "set_width", &spark::Window::SetWidth,
                                          "set_height", &spark::Window::SetHeight,
                                          "set_title", &spark::Window::SetTitle,
                                          "set_window_position", &spark::Window::SetWindowPosition,

                                          // Property-style access (more Lua-like)
                                          "title", sol::property(&spark::Window::GetTitle, &spark::Window::SetTitle),
                                          "width", sol::property(&spark::Window::GetWidth, &spark::Window::SetWidth),
                                          "height", sol::property(&spark::Window::GetHeight, &spark::Window::SetHeight));

        m_Lua.set_function("get_window", []() -> spark::Window &
                           { return spark::Window::GetInstance(); });

        m_Lua.new_usertype<SDL_FRect>("FRect",
                                      "x", &SDL_FRect::x,
                                      "y", &SDL_FRect::y,
                                      "w", &SDL_FRect::w,
                                      "h", &SDL_FRect::h);
        m_Lua.set_function("FRect", [](float x, float y, float w, float h) -> SDL_FRect
                           {
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect; });

        m_Lua.set_function("create_frect", [](float x, float y, float w, float h) -> SDL_FRect
                           { return SDL_FRect{x, y, w, h}; });

        m_Lua.new_usertype<SDL_FPoint>("FPoint",
                                       sol::constructors<SDL_FPoint(), SDL_FPoint(float, float)>(),
                                       "x", &SDL_FPoint::x,
                                       "y", &SDL_FPoint::y);

        m_Lua.new_usertype<SDL_FColor>("FColor",
                                       sol::constructors<SDL_FColor(), SDL_FColor(float, float, float, float)>(),
                                       "r", &SDL_FColor::r,
                                       "g", &SDL_FColor::g,
                                       "b", &SDL_FColor::b,
                                       "a", &SDL_FColor::a);

        // For SDL_Vertex if you're using geometry rendering
        m_Lua.new_usertype<SDL_Vertex>("Vertex",
                                       sol::constructors<SDL_Vertex()>(),
                                       "position", &SDL_Vertex::position,
                                       "color", &SDL_Vertex::color,
                                       "tex_coord", &SDL_Vertex::tex_coord);

        m_Lua.new_usertype<spark::Renderer>("Renderer",
                                            sol::no_constructor, // Prevent direct construction since it's a singleton

                                            // Core renderer functions
                                            //"get_sdl_renderer", &spark::Renderer::GetSDLRenderer,
                                            "clear", &spark::Renderer::Clear,
                                            "present", &spark::Renderer::Present,

                                            // Render scale functions
                                            "set_render_scale", &spark::Renderer::SetRenderScale,
                                            "get_render_scale", &spark::Renderer::GetRenderScale,

                                            // Draw color functions
                                            "set_draw_color", &spark::Renderer::SetDrawColor,
                                            "set_draw_color_float", &spark::Renderer::SetDrawColorFloat,
                                            "get_draw_color", &spark::Renderer::GetDrawColor,
                                            "get_draw_color_float", &spark::Renderer::GetDrawColorFloat,

                                            // Basic rendering functions
                                            "render_point", &spark::Renderer::RenderPoint,
                                            "render_points", &spark::Renderer::RenderPoints,
                                            "render_line", &spark::Renderer::RenderLine,
                                            "render_lines", &spark::Renderer::RenderLines,

                                            // Rectangle rendering
                                            "render_rect", &spark::Renderer::RenderRect,
                                            "render_rects", &spark::Renderer::RenderRects,
                                            "render_fill_rect", &spark::Renderer::RenderFillRect,
                                            "render_fill_rects", &spark::Renderer::RenderFillRects

                                            // Texture rendering
                                            // "render_texture", &spark::Renderer::RenderTexture,
                                            // "render_texture_rotated", &spark::Renderer::RenderTextureRotated,
                                            // "render_texture_affine", &spark::Renderer::RenderTextureAffine,
                                            // "render_texture_tiled", &spark::Renderer::RenderTextureTiled,
                                            // "render_texture_9grid", &spark::Renderer::RenderTexture9Grid,
                                            // "render_texture_9grid_tiled", &spark::Renderer::RenderTexture9GridTiled,

                                            // Geometry rendering
                                            //"render_geometry", &spark::Renderer::RenderGeometry,
                                            //"render_geometry_raw", &spark::Renderer::RenderGeometryRaw
        );

        m_Lua.set_function("get_renderer", []() -> spark::Renderer &
                           { return spark::Renderer::GetInstance(); });

        m_Lua.new_usertype<spark::TransformComponent>("TransformComponent", sol::no_constructor, sol::base_classes, sol::bases<spark::Component>(),
                                                      "SetLocalPosition", &spark::TransformComponent::SetLocalPosition,
                                                      "GetLocalPosition", &spark::TransformComponent::GetLocalPosition,
                                                      "SetLocalRotation", &spark::TransformComponent::SetLocalRotation,
                                                      "GetLocalRotation", &spark::TransformComponent::GetLocalRotation,
                                                      "SetLocalScale", &spark::TransformComponent::SetLocalScale,
                                                      "GetLocalScale", &spark::TransformComponent::GetLocalScale,
                                                      "GetWorldPosition", &spark::TransformComponent::GetWorldPosition,
                                                      "GetWorldRotation", &spark::TransformComponent::GetWorldRotation,
                                                      "GetWorldScale", &spark::TransformComponent::GetWorldScale,
                                                      "GetWorldMatrix", &spark::TransformComponent::GetWorldMatrix);

        m_Lua.new_usertype<spark::ScriptComponent>("ScriptComponent", sol::no_constructor, sol::base_classes, sol::bases<spark::Component>(),
                                                   "ReloadScript", &spark::ScriptComponent::ReloadScript);
        m_Lua.new_usertype<spark::GameObject>("GameObject", sol::no_constructor, "GetName", &spark::GameObject::GetName, "GetParent", &spark::GameObject::GetParent,
                                              // sol2 typically handles default arguments well for member functions.
                                              "SetParent", &spark::GameObject::SetParent,

                                              // Children access:
                                              // Lua typically uses 1-based indexing. These bindings expose 0-based (C++) access.
                                              // Lua scripts would call e.g. GetChild(0) for the first child.
                                              // You could adapt this in the lambda to be 1-based if preferred for Lua.
                                              "GetChildCount", [](spark::GameObject &go)
                                              {
                // Assumes GetChildren() returns a standard container like std::vector<GameObject*>
                return go.GetChildren().size(); }, "GetChild", [](spark::GameObject &go, size_t index_cpp) -> spark::GameObject *
                                              {
                                                  // const auto& children = go.GetChildren(); // GetChildren() is const std::vector<GameObject *> &GetChildren() const;
                                                  // if (index_cpp < children.size()) {
                                                  //     return children[index_cpp];
                                                  // }
                                                  // The above was commented out because GameObject.h actually has:
                                                  // std::vector<GameObject *> m_childrenRawPtrs;
                                                  // const std::vector<GameObject *> &GetChildren() const; -> this is correct
                                                  const std::vector<spark::GameObject *> &children = go.GetChildren();
                                                  if (index_cpp < children.size())
                                                  {
                                                      return children[index_cpp];
                                                  }
                                                  // Optionally, you could raise a Lua error for out-of-bounds access:
                                                  // luaL_error(go.m_scriptEnv.lua_state(), "GetChild: index out of bounds");
                                                  return nullptr; // Return nil to Lua if index is out of bounds
                                              },

                                              "Delete", &spark::GameObject::Delete, "GetIsToBeDeleted", &spark::GameObject::GetIsToBeDeleted,

                                              // GetComponent<T>() bindings:
                                              // These use lambdas to call the templated GetComponent<T>() method.
                                              "GetTransformComponent", [](spark::GameObject &go)
                                              { return go.GetComponent<spark::TransformComponent>(); }, "GetScriptComponent", [](spark::GameObject &go)
                                              { return go.GetComponent<spark::ScriptComponent>(); },
                                              // Example for another component type (if you have, e.g., RenderComponent):
                                              // "GetRenderComponent", [](spark::GameObject& go) {
                                              //     return go.GetComponent<spark::RenderComponent>();
                                              // },

                                              // AddComponent<T>(Args...) bindings:
                                              // These use lambdas to call the templated AddComponent<T>(Args...) method.
                                              "AddTransformComponent", [](spark::GameObject &go)
                                              {
                // Assumes TransformComponent can be added without extra arguments to AddComponent,
                // or AddComponent<TransformComponent>() handles default construction.
                return go.AddComponent<spark::TransformComponent>(); }, "AddScriptComponent", [](spark::GameObject &go, const std::string &scriptPath)
                                              {
                // Calls AddComponent<ScriptComponent>(this_gameobject_ptr, scriptPath)
                // The GameObject::AddComponent template takes (Args&&... args)
                // The ScriptComponent constructor is (GameObject* parent, const std::string& scriptPath)
                // The 'this' (parent GameObject*) is implicitly handled by AddComponent.
                return go.AddComponent<spark::ScriptComponent>(scriptPath); }
                                              // Example for another component type with arguments:
                                              // "AddLightComponent", [](spark::GameObject& go, float intensity, const glm::vec3& color) {
                                              //    return go.AddComponent<spark::LightComponent>(intensity, color);
                                              // }

        );

        m_Lua.set_function("get_mouse_position", []() -> std::pair<float, float>
                           {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return std::make_pair(x, y); });

        m_Lua.set_function("get_mouse_buttons", []() -> Uint32
                           {
    float x, y; // We don't need these but SDL_GetMouseState requires them
    return SDL_GetMouseState(&x, &y); });

        // Option 3: Individual button check functions
        m_Lua.set_function("is_mouse_button_down", [](int button) -> bool
                           {
    float x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    
    switch(button) {
        case 1: return (buttons & SDL_BUTTON_LMASK) != 0;  // Left
        case 2: return (buttons & SDL_BUTTON_MMASK) != 0;  // Middle
        case 3: return (buttons & SDL_BUTTON_RMASK) != 0;  // Right
        case 4: return (buttons & SDL_BUTTON_X1MASK) != 0; // X1
        case 5: return (buttons & SDL_BUTTON_X2MASK) != 0; // X2
        default: return false;
    } });

        // Option 4: Check specific named buttons
        m_Lua.set_function("is_left_mouse_down", []() -> bool
                           {
    float x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    return (buttons & SDL_BUTTON_LMASK) != 0; });

        m_Lua.set_function("is_right_mouse_down", []() -> bool
                           {
    float x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    return (buttons & SDL_BUTTON_RMASK) != 0; });

        m_Lua.set_function("is_middle_mouse_down", []() -> bool
                           {
    float x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    return (buttons & SDL_BUTTON_MMASK) != 0; });
    }
} // namespace spark
