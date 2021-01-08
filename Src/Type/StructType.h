#pragma once

#include "Type.h"
#include <memory>
#include <vector>
#include "TypeRef.h"

class StructType : public Type {
public:
    struct Field {
        std::string name;
        std::shared_ptr<TypeRef> type;
    };
    std::vector<Field> fields;
};
