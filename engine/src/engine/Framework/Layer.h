#pragma once
#include <string>

class Runtime;

class Layer
{
public:
    Layer(const std::string& name = "Layer") : mName(name) {}

    virtual void Attach() = 0;
    virtual void Render() = 0;
    virtual void Update(float dt) = 0;

    const std::string& GetName() const { return mName; }

protected:
    std::string mName;
};
