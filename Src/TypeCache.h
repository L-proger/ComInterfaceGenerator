#pragma once

#include <Type/Type.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <Type/Module.h>
#include <Parser/TypeNameParser.h>
#include <stack>
#include <filesystem>

class TypeCache {
public:
    static const char* primitiveModuleName();
    static void init();

    static std::shared_ptr<Module> parseModule(const std::string& name);
    static std::shared_ptr<Type> findPrimitiveType(std::string name);
    static std::shared_ptr<Type> findOrDefineReferencedType(TypeName name);

    static void addSearchPath(std::filesystem::path path);
    static std::filesystem::path findModulePath(const std::string& name);

    template<typename T, typename ... Args>
    static std::shared_ptr<T> makeLocalType(const std::string& name, Args&&... args){
        TypeName tn;
        tn.isLocalType = true;
        tn.isPrimitive = false;
        tn.name = name;
        tn.module = moduleParseStack.top()->name;

        if( moduleParseStack.top()->findType(name) != nullptr){
            throw std::runtime_error("Type already exists");
        }else{
            auto newType = std::make_shared<T, Args...>(std::move<Args>(args)...);
            newType->name = tn.name;
            newType->moduleName = tn.module;
            types.push_back(newType);
            moduleParseStack.top()->types.push_back(newType);
            return newType;
        }
    }

    static std::shared_ptr<Module> findModule(std::string name);
    static const std::vector<std::shared_ptr<Type>>& getTypes();
    static const std::vector<std::shared_ptr<Module>>& getModules();


    static void replaceType(std::shared_ptr<Type> srcType, std::shared_ptr<Type> dstType, const std::string moduleName);
private:
    template<typename T, typename ... Args>
    static std::shared_ptr<T> makePrimitiveType(const std::string& name, Args&&... args){
        auto newType = std::make_shared<T, Args...>(std::move<Args>(args)...);
        newType->name = name;
        newType->moduleName = primitiveModuleName();
        types.push_back(newType);
        findModule(primitiveModuleName())->types.push_back(newType);
        return newType;
    }

    static std::vector<std::shared_ptr<Type>> types;
    static std::vector<std::shared_ptr<Module>> modules;
    static std::stack<std::shared_ptr<Module>> moduleParseStack;

    static std::shared_ptr<Module> resolveImport(const std::string& path);
};
