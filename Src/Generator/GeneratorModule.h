#pragma once

#include <Type/Type.h>
#include <memory>
#include <vector>

class GeneratorModule {
public:
    std::string name;
    std::shared_ptr<Type> findType(const std::string& name) {
        auto it = std::find_if(_types.begin(), _types.end(), [name](std::shared_ptr<Type> t){ return t->name == name; });
        if(it != _types.end()){
            return *it;
        }
        return nullptr;
    }

    std::shared_ptr<GeneratorModule> findDependency(const std::string& name) {
        auto it = std::find_if(_dependencies.begin(), _dependencies.end(), [name](std::shared_ptr<GeneratorModule> t){ return t->name == name; });
        if(it != _dependencies.end()){
            return *it;
        }
        return nullptr;
    }

    void addDependency(std::shared_ptr<GeneratorModule> dep) {
        auto localDep = findDependency(dep->name);
        if(localDep == nullptr){
            _dependencies.push_back(dep);
        }
    }

    void addType(std::shared_ptr<Type> type) {
        if(findType(type->name) == nullptr){
            _types.push_back(type);
        }
    }
    const std::vector<std::shared_ptr<GeneratorModule>>& getDependencies() const {
        return _dependencies;
    }
    const std::vector<std::shared_ptr<Type>>& getTypes() const {
        return _types;
    }
protected:
    std::vector<std::shared_ptr<GeneratorModule>> _dependencies;
    std::vector<std::shared_ptr<Type>> _types;
};
