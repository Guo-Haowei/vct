#pragma once
#include "Scene.h"

class ILoader
{
public:
    virtual Scene* parse(const char* root, const char* file) = 0;
};