#pragma once

#include <Type/AttributeType.h>
#include <Type/TypeRef.h>

class MethodArg {
public:
    std::string name;
    std::vector<std::shared_ptr<TypeRef>> attributes;
    std::shared_ptr<TypeRef> type;
    bool reference = false;
};

class MethodDesc {
public:
    std::string name;
    std::shared_ptr<TypeRef> returnType;
    std::vector<MethodArg> args;
};
