#pragma once

#include <string>
#include <stdexcept>
#include <Type/Method.h>
#include <Generator/Attribute/AttributeUtils.h>

class CppMarshaler {
public:
    static std::string getVariableName(const std::string& argName){
        return argName + "Marshaler";
    }

    static bool shouldMarshal(const MethodArg& arg) {
        return (arg.array || (arg.type->type->name == "std::string"));
    }

    static std::string getMarshalerType(const MethodArg& arg){
        if(shouldMarshal(arg)){
            auto attrs = AttributeList::parse(arg.attributes);
            auto outAttr = attrs.getAttribute<OutAttribute>();

            if(outAttr != nullptr){
                return "LFramework::ArrayOutMarshaler";
            }else{
                return "LFramework::ArrayInMarshaler";
            }
        }

        throw std::runtime_error("Can not faind marhshaler for type");
    }
};
