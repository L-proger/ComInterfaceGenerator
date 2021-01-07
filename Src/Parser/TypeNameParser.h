#pragma once

#include "CidlBaseListener.h"

struct TypeName {
    std::string module;
    std::string name;
    bool isLocalType = true;
    bool isPrimitive = false;
};

namespace TypeNameParser {

inline TypeName parse(CidlParser::Local_typeContext* ctx){
    TypeName result;
    result.name = ctx->getText();
    result.isLocalType = true;
    result.isPrimitive = ctx->primitive() != nullptr;
    return result;
}

inline TypeName parse(CidlParser::Imported_typeContext* ctx){
    TypeName result;
    auto n =ctx->getText();
    auto n2 =  ctx->imported_type_name()->getText();
     auto n3 =  ctx->imported_type_module()->getText();
    result.name = ctx->imported_type_name()->ID()->getText();
    result.module = ctx->imported_type_module()->getText();
    result.module  = result.module.substr(0, result.module.size() - 1);
    result.isLocalType = false;
    return result;
}

inline TypeName parse(CidlParser::Local_or_imported_typeContext* ctx){
    if(ctx->local_type() != nullptr){
        return parse(ctx->local_type());
    }else{
        return parse(ctx->imported_type());
    }
}

}
