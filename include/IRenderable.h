#ifndef IRENDERABLE_H
#define IRENDERABLE_H

namespace spark
{
    struct IRenderable
    {
        virtual ~IRenderable() = default;
        virtual void Render() = 0;
    };

} // namespace spark

#endif // IRENDERABLE_H
