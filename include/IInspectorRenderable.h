#ifndef IINSPECTORRENDERABLE_H
#define IINSPECTORRENDERABLE_H

namespace spark
{
    struct IInspectorRenderable
    {
        virtual ~IInspectorRenderable() = default;
        virtual void RenderInspector() = 0;
    };
} // namespace spark

#endif // IINSPECTORRENDERABLE_H