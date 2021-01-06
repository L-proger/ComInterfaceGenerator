#pragma once

#include "Constant/Constant.h"
#include "Type.h"
#include <memory>
#include <vector>

class AttributeType : public Type {
public:
    std::vector<std::shared_ptr<Constant>> arguments;
};
