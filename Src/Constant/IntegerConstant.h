#pragma once

#include "NumericConstant.h"
#include <sstream>
#include <Type/TypeRef.h>

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
class IntegerConstant : public NumericConstant {
public:
    T value;
    IntegerConstant(std::shared_ptr<Type> type, T value): NumericConstant(type), value(value){}
    IntegerConstant(std::shared_ptr<Type> type, const std::string& valueString, bool hex) : NumericConstant(type){
        if constexpr(std::is_signed_v<T>){
            value = (T)std::stoll(valueString, nullptr, hex ? 16 : 10);
        }else{
            value = (T)std::stoull(valueString, nullptr, hex ? 16 : 10);
        }
    }
    void increment() override {
        value++;
    }
    void decrement() override {
        value--;
    }
    void negate() override {
        value = -value;
    }
    std::shared_ptr<Constant> copy() override {
        return std::static_pointer_cast<Constant>(std::make_shared<IntegerConstant>(type, value));
    }

    std::string toString() override {
        std::stringstream ss;
        ss << +value;
        return ss.str();
    }
};


inline std::shared_ptr<NumericConstant> makeIntegerConstant(std::shared_ptr<TypeRef> type, const std::string& valueString, bool hex){
    if(type->type->name == "int8"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int8_t>>(type->type, valueString, hex));
    }else if(type->type->name == "int16"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int16_t>>(type->type, valueString, hex));
    }else if(type->type->name == "int32"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int32_t>>(type->type, valueString, hex));
    }else if(type->type->name == "int64"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int64_t>>(type->type, valueString, hex));
    }else if(type->type->name == "uint8"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint8_t>>(type->type, valueString, hex));
    }else if(type->type->name == "uint16"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint16_t>>(type->type, valueString, hex));
    }else if(type->type->name == "uint32"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint32_t>>(type->type, valueString, hex));
    }else if(type->type->name == "uint64"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint64_t>>(type->type, valueString, hex));
    }
    return nullptr;
}
