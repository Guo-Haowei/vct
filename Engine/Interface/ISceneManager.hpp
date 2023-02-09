#pragma once
#include <memory>
#include "IRuntimeModule.hpp"

class Scene;

class ISceneManager : public IRuntimeModule {
public:
    virtual bool LoadScene( const char* scenePath ) = 0;

    virtual uint64_t GetSceneRevision() const = 0;

    virtual Scene* GetScene() const = 0;
};