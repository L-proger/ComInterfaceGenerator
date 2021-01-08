#pragma once

#include <Generator/Generator.h>
#include <Generator/Cpp/CppCodeFile.h>

class CppGenerator : public Generator {
public:
    CppGenerator(const std::string& outputPath) : Generator(outputPath){
        auto cstdintModule = std::make_shared<Module>();
        TypeCache::addModule(cstdintModule);
    }

    std::shared_ptr<CodeFile> createCodeFile() override {
        return std::make_shared<CppCodeFile>();
    }
};
