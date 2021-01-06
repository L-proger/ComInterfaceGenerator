#pragma once

#include <Type/AttributeType.h>

class MethodArg {
public:
    std::string name;
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<Type> type;
    bool reference = false;
};

class MethodDesc {
public:
    std::string name;
    std::shared_ptr<Type> returnType;
    std::vector<MethodArg> args;
};
