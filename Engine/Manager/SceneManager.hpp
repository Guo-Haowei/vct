#pragma once
#include <atomic>

#include "Interface/ISceneManager.hpp"
#include "SceneGraph/Scene.hpp"

class SceneManager : public ISceneManager {
public:
    virtual bool Initialize() override;
    virtual void Tick() override;
    virtual void Finalize() override;

    virtual bool LoadScene( const char* scene_name ) override;

    virtual uint64_t GetSceneRevision() const override { return m_nSceneRevision; }

    virtual Scene* GetScene() const override;

protected:
    bool LoadAssimp( const char* scene_name );

    std::shared_ptr<Scene> m_pScene;
    std::atomic<uint64_t> m_nSceneRevision{ 0 };
};