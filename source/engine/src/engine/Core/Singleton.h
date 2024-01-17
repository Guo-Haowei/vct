#pragma once
#include "Types.h"

template<typename T>
class Singleton
{
    DISABLE_COPY(Singleton);

public:
    Singleton()
    {
        assert(msSingleton == nullptr);
        msSingleton = static_cast<T*>(this);
    }

    ~Singleton()
    {
        assert(msSingleton);
        msSingleton = nullptr;
    }

    static T& GetSingleton()
    {
        assert(msSingleton);
        return *msSingleton;
    }

    static T* GetSingletonPtr()
    {
        return msSingleton;
    }

protected:
    static T* msSingleton;
};
