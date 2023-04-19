#pragma once
#include "ManagerBase.h"

#include "Core/scene.h"

class SceneManager : public ManagerBase
{
public:
    SceneManager() : ManagerBase("SceneManager") {}

    virtual void Update(float dt) override;

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;
};

extern SceneManager* gSceneManager;

// @TODO: refactor
struct Scene;
Scene& Com_GetScene();
