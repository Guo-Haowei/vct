#pragma once
#include "Core/Singleton.h"

class Application;

class ManagerBase {
    DISABLE_COPY(ManagerBase);

public:
    ManagerBase(const char* name) : mName(name) {}
    virtual ~ManagerBase() = default;

    virtual bool DependsOn(const ManagerBase&) { return false; }

    bool Initialize();
    void Finalize();

    virtual void Update(float);

    const char* GetName() const { return mName; }
    bool Initialized() const { return mInitialized; }

protected:
    virtual bool InitializeInternal() = 0;
    virtual void FinalizeInternal() = 0;

    const char* mName = nullptr;
    bool mInitialized = false;
    Application* mApplication = nullptr;

    friend class Application;
};
