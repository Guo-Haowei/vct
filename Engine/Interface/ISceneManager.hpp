#pragma once
#include <memory>
#include "IRuntimeModule.hpp"

class ISceneManager : public IRuntimeModule {
public:
    virtual bool LoadScene( const char* scenePath ) = 0;
};