#ifndef SINGLETON_H
#define SINGLETON_H

namespace spark
{
    // anything that inherits from this becomes a singelton
    template <typename T>
    class Singleton
    {
    public:
        static T &GetInstance()
        {
            static T instance{};
            return instance;
        }

        virtual ~Singleton() = default;
        Singleton(const Singleton &other) = delete;
        Singleton(Singleton &&other) = delete;
        Singleton &operator=(const Singleton &other) = delete;
        Singleton &operator=(Singleton &&other) = delete;

    protected:
        Singleton() = default;
    };
}

#endif // SINGLETON_H
