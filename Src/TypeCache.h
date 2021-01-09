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
    static std::shared_ptr<TypeRef> findPrimitiveType(std::string name, bool addToModuleReferences = true);
    static std::shared_ptr<TypeRef> findOrDefineReferencedType(TypeName name, bool addToModuleReferences = true);
    static void addSearchPath(std::filesystem::path path);
    static std::filesystem::path findModulePath(const std::string& name);
    static std::shared_ptr<Module> findModule(std::string name);
    static const std::vector<std::shared_ptr<Module>>& getModules();
    static void replaceType(std::shared_ptr<TypeRef> srcType, std::shared_ptr<TypeRef> dstType);

    template<typename TModule>
    static std::shared_ptr<TModule> addModule(const std::string& name){
        if(findModule(name) != nullptr){
            throw std::runtime_error("Module already exists");
        }
        auto module = std::make_shared<TModule>();
        module->name = name;
        modules.push_back(module);
        return module;
    }

    template<typename T, typename ... Args>
    static std::shared_ptr<TypeRef> makeLocalType(const std::string& name, Args&&... args){
        TypeName tn;
        tn.isLocalType = true;
        tn.isPrimitive = false;
        tn.name = name;
        tn.module = moduleParseStack.top()->name;

        if( moduleParseStack.top()->findType(name) != nullptr){
            throw std::runtime_error("Multiple types with the same name \"" + name + "\" defined in module \"" + tn.module + "\"");
        }else{
            auto newType = std::make_shared<T>(std::move<Args>(args)...);
            newType->name = tn.name;
            newType->moduleName = tn.module;

            auto tr = std::make_shared<TypeRef>();
            tr->type = newType;
            moduleParseStack.top()->types.push_back(tr);
            return tr;
        }
    }


private:
    template<typename T, typename ... Args>
    static std::shared_ptr<TypeRef> makePrimitiveType(const std::string& name, Args&&... args){
        auto newType = std::make_shared<T, Args...>(std::move<Args>(args)...);
        newType->name = name;
        newType->moduleName = primitiveModuleName();
        //types.push_back(newType);

        auto tr = std::make_shared<TypeRef>();
        tr->type = newType;
        findModule(primitiveModuleName())->types.push_back(tr);
        return tr;
    }

    static std::vector<std::shared_ptr<Module>> modules;
    static std::stack<std::shared_ptr<Module>> moduleParseStack;
};
