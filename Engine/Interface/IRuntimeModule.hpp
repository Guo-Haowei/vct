#pragma once

class IApplication;

class IRuntimeModule {
public:
    IRuntimeModule() = default;
    virtual ~IRuntimeModule() = default;

    virtual bool Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Tick() = 0;

    void SetAppPointer( IApplication* pApp ) { m_pApp = pApp; }

protected:
    IApplication* m_pApp;
};
