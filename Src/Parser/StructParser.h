#pragma once

#include "CidlBaseListener.h"
#include <Type/StructType.h>
#include <TypeCache.h>

class StructParser : public CidlBaseListener {
public:
    std::shared_ptr<StructType> result;

    void enterStructDefinition(CidlParser::StructDefinitionContext* ctx) override {
        result =TypeCache::makeType<StructType>(ctx->identifier()->getText());

        for(auto& fieldCtx : ctx->structField()){
            StructType::Field field;
            field.type = TypeCache::findType(fieldCtx->structFieldType()->getText());
            field.name = fieldCtx->identifier()->getText();
            result->fields.push_back(field);
        }
    }
};
