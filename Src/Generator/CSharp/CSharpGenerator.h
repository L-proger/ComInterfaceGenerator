#pragma once

#include <Generator/Generator.h>
#include "CSharpCodeFile.h"
#include <TypeCache.h>
#include <map>

class CSharpGenerator : public Generator {
public:
    CSharpGenerator(const std::string& outputPath) : Generator(outputPath){

    }

    std::shared_ptr<CodeFile> createCodeFile() override {
        return std::make_shared<CSharpCodeFile>();
    }
};
