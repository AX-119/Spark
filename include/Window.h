#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <memory>
#include "Singleton.h"
#include <string>
namespace spark
{
    struct SDLWindowDeleter
    {
        void operator()(SDL_Window *window) const
        {
            if (window)
                SDL_DestroyWindow(window);
        }
    };

    class Window final : public Singleton<Window>
    {
    public:
        ~Window() = default;
        Window(const Window &other) = delete;
        Window(Window &&other) = delete;
        Window &operator=(const Window &other) = delete;
        Window &operator=(Window &&other) = delete;

        SDL_Window *GetSDLWindow() const;
        std::string GetTitle() const;
        int GetWidth() const;
        int GetHeight() const;
        std::pair<int, int> GetWindowPosition() const;

        void SetWidth(const int width);
        void SetHeight(const int height);
        void SetTitle(const std::string &title);
        void SetWindowPosition(const int x, const int y);

        void OnResize(const int newWidth, const int newHeight);

    private:
        friend Singleton<Window>;
        Window();

    private:
        std::string m_title{"Spark"};
        int m_width{1920};
        int m_height{1080};
        SDL_WindowFlags m_flags{SDL_WINDOW_RESIZABLE};
        std::unique_ptr<SDL_Window, SDLWindowDeleter> m_SDLWindow{};
    };

} // namespace spark

#endif // WINDOW_H