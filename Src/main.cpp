#include "TypeCache.h"
#include <filesystem>
#include <Generator/CSharp/CSharpGenerator.h>
#include <Generator/Cpp/CppGenerator.h>
#include <iostream>

int main() {

    TypeCache::init();
    std::filesystem::path srcPath(SRC_DIR);
    auto apiPath = srcPath / "IDL";
    TypeCache::addSearchPath(apiPath);

    auto module = TypeCache::parseModule("Example.Network2");

    auto allTypes = TypeCache::getTypes();
    auto allModules = TypeCache::getModules();

    CppGenerator gen("ololo");
    gen.generate();

    return 0;
}
