#pragma once
#include <cstdint>

class IApplication {
public:
    virtual ~IApplication() = default;
    virtual bool Initialize( int argc, const char** argv ) = 0;

    virtual bool IsQuit() const = 0;
    virtual void RequestQuit() = 0;

    virtual void CreateMainWindow() = 0;
    virtual void* GetMainWindowHandler() = 0;

    virtual void GetFramebufferSize( uint32_t& width, uint32_t& height ) = 0;
};
