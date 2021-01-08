#pragma once

#include <Type/Type.h>
#include <memory>

class TypeRef {
public:
    std::shared_ptr<Type> type;
};

