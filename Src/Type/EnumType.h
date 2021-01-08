#pragma once

#include "TypeRef.h"
#include <Constant/NumericConstant.h>
#include <vector>

class EnumType : public Type {
public:
    std::shared_ptr<TypeRef> fieldType;
    struct Field {
        std::string name;
        std::shared_ptr<NumericConstant> value;
    };
    std::vector<Field> fields;
};
