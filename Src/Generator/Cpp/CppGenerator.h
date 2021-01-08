#pragma once

#include <Generator/Generator.h>
#include <Generator/Cpp/CppCodeFile.h>

class CppGenerator : public Generator {
public:
    CppGenerator(const std::string& outputPath) : Generator(outputPath){

    }

    std::shared_ptr<CodeFile> createCodeFile() override {
        return std::make_shared<CppCodeFile>();
    }
};