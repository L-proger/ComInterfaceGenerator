#pragma once

#include "CidlBaseListener.h"
#include <Type/AttributeType.h>
#include <Constant/StringConstant.h>
#include <Constant/FloatConstant.h>
#include <Constant/DoubleConstant.h>
#include "TypeCache.h"

class AttributeParser : public CidlBaseListener {
public:
    std::shared_ptr<TypeRef> result;
    void enterAttribute(CidlParser::AttributeContext * ctx) override {
        auto _result = std::make_shared<AttributeType>();
        _result->name = TypeNameParser::parse(ctx->local_type()).name;

        auto argList = ctx->argument_list();

        if(argList != nullptr){
            for(auto& argCtx : ctx->argument_list()->argument()){
                auto expr = argCtx->expression();
                if(expr != nullptr){
                    if(expr->STRING_LITERAL() != nullptr){
                        _result->arguments.push_back(std::make_shared<StringConstant>(TypeCache::findPrimitiveType("string")->type, expr->getText()));
                    }else{
                        auto numeric = expr->numeric_literal();
                        if(numeric->float_literal() != nullptr){
                            if(numeric->float_literal()->FLOAT_SUFFIX() != nullptr){
                                _result->arguments.push_back(std::make_shared<FloatConstant>(TypeCache::findPrimitiveType("float")->type, expr->getText()));
                            }else{
                                _result->arguments.push_back(std::make_shared<DoubleConstant>(TypeCache::findPrimitiveType("double")->type, expr->getText()));
                            }
                        }else if(numeric->HEX_LITERAL() != nullptr){
                            throw std::runtime_error("Not implemented");
                        }
                    }
                }else{
                    throw std::runtime_error("Not implemented");
                }
            }
        }


        result = std::make_shared<TypeRef>();
        result->type = _result;

    }
};
