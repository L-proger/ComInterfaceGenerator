#pragma once

#include <Type/Type.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <Type/Module.h>

class TypeCache {
public:
    static void init();

    static std::shared_ptr<Module> parseModule(const std::string& path);

    static std::shared_ptr<Type> findType(std::string name);

    template<typename T, typename ... Args>
    static std::shared_ptr<T> makeType(const std::string& name, Args&&... args){
        try{
            findType(name);
            throw std::runtime_error("Type already exists");
        }catch(...){
            auto newType = std::make_shared<T, Args...>(std::move<Args>(args)...);
            newType->name = name;
            types.push_back(newType);
            return newType;
        }
    }

    static std::shared_ptr<Module> findModule(std::string name);
private:
    static std::vector<std::shared_ptr<Type>> types;
    static std::vector<std::shared_ptr<Module>> modules;

    static std::shared_ptr<Module> resolveImport(const std::string& path);
};
