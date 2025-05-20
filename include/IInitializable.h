#ifndef IINITIALIZABLE_H
#define IINITIALIZABLE_H

namespace spark
{
    struct IInitializable
    {
        virtual ~IInitializable() = default;
        virtual void Init() = 0;
    };

} // namespace spark

#endif // IINITIALIZABLE_H
