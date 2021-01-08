#include "TypeCache.h"
#include <filesystem>
#include <Generator/CSharp/CSharpGenerator.h>
#include <Generator/Cpp/CppGenerator.h>
#include <iostream>
#include <Generator/Guid.h>
#include <Generator/Attribute/AttributeUtils.h>

void run() {
    TypeCache::init();
    std::filesystem::path srcPath(SRC_DIR);
    auto apiPath = srcPath / "IDL";
    TypeCache::addSearchPath(apiPath);

    auto module = TypeCache::parseModule("Example.Network2");

    auto allModules = TypeCache::getModules();

    auto netModule = TypeCache::findModule("Example.Network");
    auto inetworkRef = netModule->findType("INetwork");
    auto inetwork = std::dynamic_pointer_cast<InterfaceType>(inetworkRef->type);

    auto attributes = AttributeList::parse(inetwork->attributes);

    CppGenerator gen("ololo");
    gen.generate();
}

int main() {
    try{
        run();
        return 0;
    }catch(const std::exception& ex){
        std::cerr << "Error occured: " << ex.what() << std::endl;
        return -1;
    }catch(...){
        std::cerr << "Unexpected error occured" << std::endl;
        return -1;
    }
}
