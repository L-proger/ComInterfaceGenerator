#pragma once

#include <Type/EnumType.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>

class Module {
public:
    std::string path;
    std::string name;
    std::vector<std::shared_ptr<Module>> imports;
    std::vector<std::shared_ptr<EnumType>> enums;
    std::vector<std::shared_ptr<StructType>> structs;
    std::vector<std::shared_ptr<InterfaceType>> interfaces;
};
