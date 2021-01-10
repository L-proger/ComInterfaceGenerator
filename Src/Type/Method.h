#pragma once

#include <Type/AttributeType.h>
#include <Type/TypeRef.h>

class MethodArg {
public:
    std::string name;
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<TypeRef> type;
    bool reference = false;
    bool array = false;
};

class MethodDesc {
public:
    std::string name;
    MethodArg returnType;
    std::vector<MethodArg> args;

    bool returnsValue() const {
        return returnType.type->type->name != "void";
    }
};
