#pragma once
#include <set>
#include "Interface/IAssetLoader.hpp"

class AssetLoader : public IAssetLoader {
public:
    virtual bool Initialize() override;
    virtual void Tick() override {}
    virtual void Finalize() override;

    virtual bool AddSearchPath( const std::string& path ) override;

    virtual bool RemoveSearchPath( const std::string& path ) override;

    virtual void ClearSearchPath() override;

    virtual std::vector<char> SyncOpenAndReadText( const char* fileName ) override;

private:
    std::set<std::string> m_searchPaths;
};
