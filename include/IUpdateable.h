#ifndef IUPDATEABLE_H
#define IUPDATEABLE_H

namespace spark
{
    struct IUpdateable
    {
        virtual ~IUpdateable() = default;
        virtual void Update() = 0;
    };

} // namespace spark

#endif // IUPDATEABLE_H
