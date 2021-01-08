#pragma once

#include <Type/EnumType.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>
#include <filesystem>

class Module {
public:

    std::filesystem::path path;
    std::string name;
    std::vector<std::shared_ptr<TypeRef>> types;
    std::vector<std::shared_ptr<TypeRef>> importedTypes;


    std::shared_ptr<TypeRef> addNewType(const std::string& typeName){
        auto type = std::make_shared<Type>();
        type->name = typeName;
        type->moduleName = name;

        auto tr = std::make_shared<TypeRef>();
        tr->type = type;

        types.push_back(tr);
        return tr;
    }

    void addImportedType(std::shared_ptr<TypeRef> type) {
        if(findImportedType(type->type) == nullptr){
            importedTypes.push_back(type);
        }
    }

    std::shared_ptr<TypeRef> findImportedType(std::shared_ptr<Type> type) {
        auto it = std::find_if(importedTypes.begin(), importedTypes.end(), [type](std::shared_ptr<TypeRef> t){ return t->type == type; });
        if(it != importedTypes.end()){
            return *it;
        }
        return nullptr;
    }

    std::shared_ptr<TypeRef> findType(const std::string& name) {
        auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<TypeRef> t){ return t->type->name == name; });
        if(it != types.end()){
            return *it;
        }
        return nullptr;
    }
};
