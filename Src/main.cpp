#include "TypeCache.h"
#include <filesystem>

int main() {
    TypeCache::init();
    std::filesystem::path srcPath(SRC_DIR);
    auto apiPath = srcPath / "IDL";
    auto testFilePath = apiPath / "Network2.cidl";
    auto module = TypeCache::parseModule(testFilePath.string());
    return 0;
}
