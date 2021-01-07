#include "TypeCache.h"
#include <filesystem>
#include <Generator/CSharp/CSharpGenerator.h>
#include <Generator/Cpp/CppGenerator.h>
#include <iostream>

int main() {

    TypeCache::init();
    std::filesystem::path srcPath(SRC_DIR);
    auto apiPath = srcPath / "IDL";
    auto testFilePath = apiPath / "Example.Network2.cidl";
    auto module = TypeCache::parseModule(testFilePath.string());

    auto allTypes = TypeCache::getTypes();
    auto allModules = TypeCache::getModules();


    CppGenerator gen("ololo");

    auto m = TypeCache::findModule("Example.Network2");
    auto t = m->types[0];
    gen.addType(t);

    /*for(auto& m : allModules){
        gen.addType(m);
    }*/

    auto files = gen.getModules();
    gen.generate();

    for(auto& file : files){
      //  auto str = file->toString();
        std::cout << "#Module file: " << file->name << std::endl;
        //std::cout << str << std::endl;
    }

    return 0;
}
