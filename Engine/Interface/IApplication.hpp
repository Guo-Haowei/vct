#pragma once

class IApplication {
public:
    virtual ~IApplication() = default;

    virtual bool Initialize() = 0;
    virtual bool ProcessCommandLine( int argc, const char** argv ) = 0;

    virtual bool ShouldQuit() = 0;
    virtual void RequestQuit() = 0;

    virtual bool CreateMainWindow() = 0;
    virtual void* GetMainWindowHandler() = 0;

    virtual void GetFramebufferSize( int& width, int& height ) = 0;
};
