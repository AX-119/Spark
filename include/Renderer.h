#ifndef RENDERER_H
#define RENDERER_H
#include <SDL3/SDL.h>
#include "Singleton.h"
#include <memory>

namespace spark
{
    struct SDLRendererDeleter
    {
        void operator()(SDL_Renderer *renderer)
        {
            if (renderer)
                SDL_DestroyRenderer(renderer);
        }
    };

    class Renderer final : public Singleton<Renderer>
    {

    public:
        ~Renderer() = default;
        SDL_Renderer *GetSDLRenderer() const;

        bool SetRenderScale(float scaleX, float scaleY);
        bool GetRenderScale(float *scaleX, float *scaleY);

        bool SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        bool SetDrawColorFloat(float r, float g, float b, float a);
        bool GetDrawColor(Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
        bool GetDrawColorFloat(float *r, float *g, float *b, float *a);
        bool Clear();
        bool Present();
        bool RenderPoint(float x, float y);
        bool RenderPoints(const SDL_FPoint *points, int count);
        bool RenderLine(float x1, float y1, float x2, float y2);
        bool RenderLines(const SDL_FPoint *points, int count);
        bool RenderRect(const SDL_FRect *rect);
        bool RenderRects(const SDL_FRect *rects, int count);
        bool RenderFillRect(const SDL_FRect *rect);
        bool RenderFillRects(const SDL_FRect *rects, int count);
        bool RenderTexture(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect);
        bool RenderTextureRotated(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect, double angle, const SDL_FPoint *center, SDL_FlipMode flip);
        bool RenderTextureAffine(SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FPoint *origin, const SDL_FPoint *right, const SDL_FPoint *down);
        bool RenderTextureTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float scale, const SDL_FRect *dstrect);
        bool RenderTexture9Grid(SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect);
        bool RenderTexture9GridTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect, float tileScale);
        bool RenderGeometry(SDL_Texture *texture, const SDL_Vertex *vertices, int num_vertices, const int *indices, int num_indices);
        bool RenderGeometryRaw(SDL_Texture *texture, const float *xy, int xy_stride, const SDL_FColor *color, int color_stride, const float *uv, int uv_stride, int num_vertices, const void *indices, int num_indices, int size_indices);

    private:
        friend Singleton<Renderer>;
        Renderer();

    private:
        std::unique_ptr<SDL_Renderer, SDLRendererDeleter> m_SDLRenderer;
    };

} // namespace spark

#endif // RENDERER_H