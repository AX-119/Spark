#ifndef IIMGUIRENDERABLE_H
#define IIMGUIRENDERABLE_H

namespace spark
{
    struct IImGuiRenderable
    {
        virtual ~IImGuiRenderable() = default;
        virtual void RenderImGui() = 0;
    };

} // namespace spark

#endif // IIMGUIRENDERABLE_H
