#pragma once

#include <Type/Module.h>
#include <Generator/CodeFile.h>
#include <map>
#include <iostream>
#include <TypeCache.h>
#include <Generator/ExternalModule.h>

class Generator {
public:
    Generator(const std::string& outputPath) : _outputPath(outputPath){
    }
    void generate() {
        auto modules = TypeCache::getModules();
        for(auto& module : modules){

            auto externalModule = std::dynamic_pointer_cast<ExternalModule>(module) != nullptr;
            if((module->name != TypeCache::primitiveModuleName()) && !externalModule && !isGenerated(module)){
                _generatedModules.push_back(module);
                auto file = createCodeFile();
                file->writeModule(module);
                std::cout << "### File: " << module->name << std::endl;
                std::cout << file->toString() << std::endl;
            }
        }
    }
protected:
    bool isGenerated(std::shared_ptr<Module> module){
        return std::find(_generatedModules.begin(), _generatedModules.end(), module) != _generatedModules.end();
    }
    std::vector<std::shared_ptr<Module>> _generatedModules;
    virtual std::shared_ptr<CodeFile> createCodeFile() = 0;
    std::string _outputPath;
};
