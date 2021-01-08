#pragma once

#include "AttributeType.h"
#include "Method.h"
#include "TypeRef.h"

class InterfaceType : public Type {
public:
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<TypeRef> baseInterfaceType;
    std::vector<MethodDesc> methods;
};
