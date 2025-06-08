#include "Window.h"
namespace spark
{
    Window::Window() : m_SDLWindow{std::unique_ptr<SDL_Window, SDLWindowDeleter>(SDL_CreateWindow(m_title.c_str(), m_width, m_height, m_flags))}
    {
    }
    SDL_Window *Window::GetSDLWindow() const
    {
        return m_SDLWindow.get();
    }
    std::string Window::GetTitle() const
    {
        return m_title;
    }

    int Window::GetWidth() const
    {
        return m_width;
    }

    int Window::GetHeight() const
    {
        return m_height;
    }

    std::pair<int, int> Window::GetWindowPosition() const
    {
        int x, y;
        SDL_GetWindowPosition(m_SDLWindow.get(), &x, &y);

        return std::make_pair(x, y);
    }

    void Window::SetWidth(const int width)
    {
        if (width < 0)
            return;
        m_width = width;
    }

    void Window::SetHeight(const int height)
    {
        if (height < 0)
            return;
        m_height = height;
    }

    void Window::SetTitle(const std::string &title)
    {
        m_title = title;
    }

    void Window::SetWindowPosition(const int x, const int y)
    {
        SDL_SetWindowPosition(m_SDLWindow.get(), x, y);
    }

}