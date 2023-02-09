#pragma once

class BaseApplication;

class IRuntimeModule {
public:
    IRuntimeModule() = default;
    virtual ~IRuntimeModule() = default;

    virtual bool Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Tick() = 0;

    void SetAppPointer( BaseApplication* pApp ) { m_pApp = pApp; }
    BaseApplication* GetAppPointer() { return m_pApp; }

protected:
    BaseApplication* m_pApp;
};
