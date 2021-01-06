#pragma once

#include "AttributeType.h"
#include "Method.h"

class InterfaceType : public Type {
public:
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<InterfaceType> baseInterfaceType;
    std::vector<MethodDesc> methods;
};
