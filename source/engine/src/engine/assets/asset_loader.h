#pragma once

namespace vct {

enum AssetType {
    ASSET_TYPE_TEXT,
    ASSET_TYPE_SHADER,
};

struct LoadTask {
    std::string asset_path;
};

void loader_main();

}  // namespace vct
