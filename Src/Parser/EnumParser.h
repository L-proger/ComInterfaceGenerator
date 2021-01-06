#pragma once

#include "CidlBaseListener.h"
#include <Type/EnumType.h>
#include "TypeCache.h"
#include <Constant/IntegerConstant.h>

class EnumParser : public CidlBaseListener {
public:
    std::shared_ptr<EnumType> result;
    void enterEnumDefinition(CidlParser::EnumDefinitionContext * ctx) override {
        result = TypeCache::makeType<EnumType>(ctx->identifier()->getText());

        auto fieldTypeName = ctx->integer_primitive()->getText();
        auto fieldType = TypeCache::findType(fieldTypeName);

        result->fieldType = fieldType;

        auto currentValue = makeIntegerConstant(fieldType, "0", false);

        for(auto& fieldCtx : ctx->enumField()){
            EnumType::Field field;
            field.name = fieldCtx->identifier()->getText();

            auto valueContext = fieldCtx->enum_field_value();
            if(valueContext != nullptr){
                field.value = makeIntegerConstant(fieldType, valueContext->getText(), valueContext->HEX_LITERAL() != nullptr);
                currentValue = std::dynamic_pointer_cast<NumericConstant>(field.value->copy());
                currentValue->increment();
            }else{
                field.value = std::dynamic_pointer_cast<NumericConstant>(currentValue->copy());
                currentValue->increment();
            }
            result->fields.push_back(field);
        }
    }
};
