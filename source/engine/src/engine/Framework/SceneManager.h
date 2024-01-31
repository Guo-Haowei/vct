#pragma once
#include "ManagerBase.h"
#include "Scene/Scene.h"

class SceneManager : public ManagerBase {
public:
    SceneManager() : ManagerBase("SceneManager") {}

    virtual void Update(float dt) override;

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;
};

extern SceneManager* gSceneManager;

// @TODO: refactor
class Scene;
Scene& Com_GetScene();
