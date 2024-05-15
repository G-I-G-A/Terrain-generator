// Singleton.h
#ifndef SINGLETON_H // include guard
#define SINGLETON_H

namespace Turtle
{
// Scott Meyers singleton implementation
template<typename SingletonType>
class Singleton
{
public:
    static SingletonType& Instance()
    {
        static SingletonType instance;
        return instance;
    }

    // disable copy/move

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};
}

#endif /* SINGLETON_H */