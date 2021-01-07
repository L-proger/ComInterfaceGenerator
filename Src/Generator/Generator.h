#pragma once

#include <Type/Module.h>
#include <Generator/CodeFile.h>
#include <map>
#include "GeneratorModule.h"
#include <iostream>

class Generator {
public:
    Generator(const std::string& outputPath) : _outputPath(outputPath){

    }
    void addType(std::shared_ptr<Type> type) {

        auto module = findOrCreateModule(type->moduleName);

        auto interfaceType = std::dynamic_pointer_cast<InterfaceType>(type);
        if(interfaceType != nullptr){
            if(interfaceType->baseInterfaceType != nullptr){
                addType(module, interfaceType->baseInterfaceType);
            }
            for(auto& method : interfaceType->methods){
                addType(module,method.returnType);
                for(auto& arg : method.args){
                    addType(module,arg.type);
                }
            }
        }

        auto structType = std::dynamic_pointer_cast<StructType>(type);
        if(structType != nullptr){
            for(auto& field : structType->fields){
                addType(module,field.type);
            }
        }

        auto enumType = std::dynamic_pointer_cast<EnumType>(type);
        if(enumType != nullptr){
            addType(module, enumType->fieldType);
        }

        addType(module, type);

    }

    void addType(std::shared_ptr<GeneratorModule> currentModule, std::shared_ptr<Type> type) {
        auto module = findOrCreateModule(type->moduleName);
        if(module != currentModule){
            currentModule->addDependency(module);
            addType(type);
        }else{
            currentModule->addType(type);
        }
    }

    std::vector<std::shared_ptr<GeneratorModule>> getModules() {
        std::vector<std::shared_ptr<GeneratorModule>> result;
        for(auto it = _modules.begin(); it != _modules.end(); ++it){
            result.push_back(it->second);
        }
        return result;
    }

    std::shared_ptr<GeneratorModule> findOrCreateModule(const std::string& name) {
        auto it = _modules.find(name);
        if(it == _modules.end()){
            auto result = std::make_shared<GeneratorModule>();
            result->name = name;
            _modules[name] = result;
            return result;
        }
        return it->second;
    }

    void generate() {
        for(auto& module : _modules){
            auto file = createCodeFile();
            file->writeModule(module.second);
            std::cout << "### File: " << module.second->name << std::endl;
            std::cout << file->toString() << std::endl;
        }
    }
protected:
    virtual std::shared_ptr<CodeFile> createCodeFile() = 0;

    std::map<std::string, std::shared_ptr<GeneratorModule>> _modules;
    std::string _outputPath;
};
