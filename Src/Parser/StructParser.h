#pragma once

#include "CidlBaseListener.h"
#include <Type/StructType.h>
#include <TypeCache.h>
#include "TypeNameParser.h"

class StructParser : public CidlBaseListener {
public:
    std::shared_ptr<TypeRef> result;

    void enterStructDefinition(CidlParser::StructDefinitionContext* ctx) override {
        auto structName = TypeNameParser::parse(ctx->local_type());
        result = TypeCache::makeLocalType<StructType>(structName.name);

        auto structType = std::dynamic_pointer_cast<StructType>(result->type);
        for(auto& fieldCtx : ctx->structField()){
            StructType::Field field;
            auto fieldType = TypeNameParser::parse(fieldCtx->local_or_imported_type());
            auto fieldName = TypeNameParser::parse(fieldCtx->local_type());
            field.type = TypeCache::findOrDefineReferencedType(fieldType);
            field.name = fieldName.name;
            structType->fields.push_back(field);
        }
    }
};
