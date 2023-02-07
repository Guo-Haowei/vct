#pragma once
#include "Interface/ISceneManager.hpp"

#include "SceneGraph/Scene.hpp"

Scene& Com_GetScene();

class SceneManager : public ISceneManager {
public:
    virtual bool Initialize() override;
    virtual void Tick() override {}
    virtual void Finalize() override {}

    virtual bool LoadScene( const char* scenePath ) override;
};