#pragma once

#include "Type.h"
#include <memory>
#include <vector>

class StructType : public Type {
public:
    struct Field {
        std::string name;
        std::shared_ptr<Type> type;
    };
    std::vector<Field> fields;
};
