#pragma once

#include "CidlBaseListener.h"
#include <Type/EnumType.h>
#include "TypeCache.h"
#include <Constant/IntegerConstant.h>

class EnumParser : public CidlBaseListener {
public:
    std::shared_ptr<TypeRef> result;
    void enterEnumDefinition(CidlParser::EnumDefinitionContext * ctx) override {
        auto enumName = TypeNameParser::parse(ctx->local_type());
        result = TypeCache::makeLocalType<EnumType>(enumName.name);

        auto fieldTypeName = ctx->integer_primitive()->getText();
        auto fieldType = TypeCache::findPrimitiveType(fieldTypeName);

        auto enumType = std::dynamic_pointer_cast<EnumType>(result->type);
        enumType->fieldType = fieldType;

        auto currentValue = makeIntegerConstant(fieldType, "0", false);

        for(auto& fieldCtx : ctx->enumField()){
            EnumType::Field field;
            field.name = TypeNameParser::parse(fieldCtx->local_type()).name;

            auto valueContext = fieldCtx->enum_field_value();
            if(valueContext != nullptr){
                field.value = makeIntegerConstant(fieldType, valueContext->getText(), valueContext->HEX_LITERAL() != nullptr);
                currentValue = std::dynamic_pointer_cast<NumericConstant>(field.value->copy());
                currentValue->increment();
            }else{
                field.value = std::dynamic_pointer_cast<NumericConstant>(currentValue->copy());
                currentValue->increment();
            }
            enumType->fields.push_back(field);
        }
    }
};
