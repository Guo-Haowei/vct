#pragma once
#include "ILoader.h"

class MyLoader : public ILoader
{
public:
    virtual Scene* parse(const char* root, const char* file) override;
};

