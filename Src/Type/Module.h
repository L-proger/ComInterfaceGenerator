#pragma once

#include <Type/EnumType.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>
#include <filesystem>

class Module {
public:
    struct Dependency {
        std::shared_ptr<Module> module;
        std::vector<std::shared_ptr<Type>> types;

        void addType(std::shared_ptr<Type> type) {
            auto it = std::find_if(types.begin(), types.end(), [type](std::shared_ptr<Type> t){ return t == type; });
            if(it == types.end()){
                types.push_back(type);
            }
        }

        bool removeType(std::shared_ptr<Type> type) {
            auto it = std::find_if(types.begin(), types.end(), [type](std::shared_ptr<Type> t){ return t == type; });
            if(it == types.end()){
                return false;
            }
            types.erase(it);
            return true;
        }
    };


    std::filesystem::path path;
    std::string name;
    std::vector<std::shared_ptr<Type>> types;
    std::vector<std::shared_ptr<Dependency>> dependencies;

    void addDependency(std::shared_ptr<Module> module, std::shared_ptr<Type> type) {
        auto dep = findDependency(module->name);
        if(dep == nullptr){
            dep = std::make_shared<Dependency>();
            dep->module = module;
            dependencies.push_back(dep);
        }
        dep->addType(type);
    }

    std::shared_ptr<Dependency> findDependency(const std::string& moduleName) {
        auto it = std::find_if(dependencies.begin(), dependencies.end(), [moduleName](std::shared_ptr<Dependency> t){ return t->module->name == moduleName; });
        if(it != dependencies.end()){
            return *it;
        }
        return nullptr;
    }

    bool removeDependency(const std::string& moduleName) {
        auto it = std::find_if(dependencies.begin(), dependencies.end(), [moduleName](std::shared_ptr<Dependency> t){ return t->module->name == moduleName; });
        if(it != dependencies.end()){
            dependencies.erase(it);
            return true;
        }
        return false;
    }

    bool hasDependency(std::shared_ptr<Module> module) {
        auto it = std::find_if(dependencies.begin(), dependencies.end(), [module](std::shared_ptr<Dependency> t){ return t->module == module; });
        return it != dependencies.end();
    }

    bool hasDependency(const std::string& moduleName) {
        auto it = std::find_if(dependencies.begin(), dependencies.end(), [moduleName](std::shared_ptr<Dependency> t){ return t->module->name == moduleName; });
        return it != dependencies.end();
    }


    std::shared_ptr<Type> findType(const std::string& name) {
        auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<Type> t){ return t->name == name; });
        if(it != types.end()){
            return *it;
        }
        return nullptr;
    }
};
