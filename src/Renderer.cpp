#include "Renderer.h"
#include "Window.h"
#include <iostream>
namespace spark
{
    Renderer::Renderer() : m_SDLRenderer{std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(SDL_CreateRenderer(Window::GetInstance().GetSDLWindow(), nullptr))}
    {
    }

    SDL_Renderer *Renderer::GetSDLRenderer() const
    {
        return m_SDLRenderer.get();
    }

    void Renderer::SetVSync(bool enabled)
    {
        if (m_SDLRenderer)
        {
            SDL_SetRenderVSync(m_SDLRenderer.get(), enabled ? 1 : 0);
        }
    }

    bool Renderer::IsVSyncEnabled() const
    {
        int vsync;
        if (SDL_GetRenderVSync(m_SDLRenderer.get(), &vsync))
        {
            return vsync == 1;
        }
        else
        {
            std::cerr << "[Renderer]: SDL_GetRenderVSync failed!\n";
            return false;
        }
    }

    bool Renderer::SetRenderScale(float scaleX, float scaleY)
    {
        return SDL_SetRenderScale(m_SDLRenderer.get(), scaleX, scaleY);
    }

    bool Renderer::GetRenderScale(float *scaleX, float *scaleY)
    {
        return SDL_GetRenderScale(m_SDLRenderer.get(), scaleX, scaleY);
    }

    bool Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return SDL_SetRenderDrawColor(m_SDLRenderer.get(), r, g, b, a);
    }

    bool Renderer::SetDrawColorFloat(float r, float g, float b, float a)
    {
        return SDL_SetRenderDrawColorFloat(m_SDLRenderer.get(), r, g, b, a);
    }

    bool Renderer::GetDrawColor(Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
    {
        return SDL_GetRenderDrawColor(m_SDLRenderer.get(), r, g, b, a);
    }

    bool Renderer::GetDrawColorFloat(float *r, float *g, float *b, float *a)
    {
        return SDL_GetRenderDrawColorFloat(m_SDLRenderer.get(), r, g, b, a);
    }

    bool Renderer::Clear()
    {
        return SDL_RenderClear(m_SDLRenderer.get());
    }

    bool Renderer::Present()
    {
        return SDL_RenderPresent(m_SDLRenderer.get());
    }

    bool Renderer::RenderPoint(float x, float y)
    {
        return SDL_RenderPoint(m_SDLRenderer.get(), x, y);
    }

    bool Renderer::RenderPoints(const SDL_FPoint *points, int count)
    {
        return SDL_RenderPoints(m_SDLRenderer.get(), points, count);
    }

    bool Renderer::RenderLine(float x1, float y1, float x2, float y2)
    {
        return SDL_RenderLine(m_SDLRenderer.get(), x1, y1, x2, y2);
    }

    bool Renderer::RenderLines(const SDL_FPoint *points, int count)
    {
        return SDL_RenderLines(m_SDLRenderer.get(), points, count);
    }

    bool Renderer::RenderRect(const SDL_FRect *rect)
    {
        return SDL_RenderRect(m_SDLRenderer.get(), rect);
    }

    bool Renderer::RenderRects(const SDL_FRect *rects, int count)
    {
        return SDL_RenderRects(m_SDLRenderer.get(), rects, count);
    }

    bool Renderer::RenderFillRect(const SDL_FRect *rect)
    {
        return SDL_RenderFillRect(m_SDLRenderer.get(), rect);
    }

    bool Renderer::RenderFillRects(const SDL_FRect *rects, int count)
    {
        return SDL_RenderFillRects(m_SDLRenderer.get(), rects, count);
    }

    // bool Renderer::RenderTexture(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect)
    // {
    //     return false;
    // }
    // bool Renderer::RenderTextureRotated(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect, double angle, const SDL_FPoint *center, SDL_FlipMode flip)
    // {
    //     return false;
    // }
    // bool Renderer::RenderTextureAffine(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FPoint *origin, const SDL_FPoint *right, const SDL_FPoint *down)
    // {
    //     return false;
    // }
    // bool Renderer::RenderTextureTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float scale, const SDL_FRect *dstrect)
    // {
    //     return false;
    // }
    // bool Renderer::RenderTexture9Grid(SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect)
    // {
    //     return false;
    // }
    // bool Renderer::RenderTexture9GridTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect, float tileScale)
    // {
    //     return false;
    // }
    // bool Renderer::RenderGeometry(SDL_Texture *texture, const SDL_Vertex *vertices, int num_vertices, const int *indices, int num_indices)
    // {
    //     return false;
    // }
    // bool Renderer::RenderGeometryRaw(SDL_Texture *texture, const float *xy, int xy_stride, const SDL_FColor *color, int color_stride, const float *uv, int uv_stride, int num_vertices, const void *indices, int num_indices, int size_indices)
    // {
    //     return false;
    // }
}