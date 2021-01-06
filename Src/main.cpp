#include "antlr4-runtime.h"
#include <tree/IterativeParseTreeWalker.h>
#include "CidlLexer.h"
#include "CidlParser.h"
#include "CidlBaseListener.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include "tree/ParseTreeListener.h"
#include "tree/ParseTree.h"
#include "tree/ErrorNode.h"
#include <variant>
#include <optional>
#include <type_traits>
#include <stdexcept>

class Type {
public:
    virtual ~Type() = default;
    std::string name;

private:

};

std::vector<std::shared_ptr<Type>> types;

std::shared_ptr<Type> findType(std::string name) {
    auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<Type> t){  return t->name == name; });
    if(it == types.end()){
        throw std::runtime_error("Type not found: " + name);
    }
    return *it;
}

class Constant {
public:
    virtual ~Constant() = default;
    std::shared_ptr<Type> type;
    Constant(std::shared_ptr<Type> type):type(type){

    }
    virtual std::shared_ptr<Constant> copy() = 0;
};

class StringConstant : public Constant {
public:
    std::string value;
    StringConstant(std::shared_ptr<Type> type, const std::string& value) : Constant(type), value(value){}
    std::shared_ptr<Constant> copy() override {
        return std::make_shared<StringConstant>(type, value);
    }
};

class NumericConstant : public Constant {
public:
    NumericConstant(std::shared_ptr<Type> type) : Constant(type){}
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void negate() = 0;
};

class FloatConstant : public NumericConstant {
public:
    float value;
    FloatConstant(std::shared_ptr<Type> type, float value ) : NumericConstant(type), value(value){

    }
    FloatConstant(std::shared_ptr<Type> type, const std::string& valueStr ) : NumericConstant(type){
        value = std::stof(valueStr);
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
        return std::make_shared<FloatConstant>(type, value);
    }
};

class DoubleConstant : public NumericConstant {
public:
    double value;
    DoubleConstant(std::shared_ptr<Type> type, double value ) : NumericConstant(type), value(value){

    }
    DoubleConstant(std::shared_ptr<Type> type, const std::string& valueStr ) : NumericConstant(type){
        value = std::stod(valueStr);
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
        return std::make_shared<DoubleConstant>(type, value);
    }
};



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
};

std::shared_ptr<NumericConstant> makeIntegerConstant(std::shared_ptr<Type> type, const std::string& valueString, bool hex){
    if(type->name == "int8"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int8_t>>(type, valueString, hex));
    }else if(type->name == "int16"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int16_t>>(type, valueString, hex));
    }else if(type->name == "int32"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int32_t>>(type, valueString, hex));
    }else if(type->name == "int64"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::int64_t>>(type, valueString, hex));
    }else if(type->name == "uint8"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint8_t>>(type, valueString, hex));
    }else if(type->name == "uint16"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint16_t>>(type, valueString, hex));
    }else if(type->name == "uint32"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint32_t>>(type, valueString, hex));
    }else if(type->name == "uint64"){
        return std::static_pointer_cast<NumericConstant>(std::make_shared<IntegerConstant<std::uint64_t>>(type, valueString, hex));
    }
    return nullptr;
}


class EnumType : public Type {
public:
    std::shared_ptr<Type> fieldType;
    struct Field {
        std::string name;
        std::shared_ptr<NumericConstant> value;
    };
    std::vector<Field> fields;
};

class StructType : public Type {
public:
    struct Field {
        std::string name;
        std::shared_ptr<Type> type;
    };
    std::vector<Field> fields;
};

class AttributeType : public Type {
public:
    std::vector<std::shared_ptr<Constant>> arguments;
};

class MethodArg {
public:
    std::string name;
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<Type> type;
    bool reference = false;
};

class MethodDesc {
public:
    std::string name;
    std::shared_ptr<Type> returnType;
    std::vector<MethodArg> args;
};

class InterfaceType : public Type {
public:
    std::vector<std::shared_ptr<AttributeType>> attributes;
    std::shared_ptr<InterfaceType> baseInterfaceType;
    std::vector<MethodDesc> methods;
};


class EnumParser : public CidlBaseListener {
public:
    std::shared_ptr<EnumType> result;
    void enterEnumDefinition(CidlParser::EnumDefinitionContext * ctx) override {
        result = std::make_shared<EnumType>();
        result->name = ctx->identifier()->getText();

        auto fieldTypeName = ctx->integer_primitive()->getText();
        auto fieldType = findType(fieldTypeName);

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

class StructParser : public CidlBaseListener {
public:
    std::shared_ptr<StructType> result;

    void enterStructDefinition(CidlParser::StructDefinitionContext* ctx) override {
        result = std::make_shared<StructType>();
        result->name = ctx->identifier()->getText();

        for(auto& fieldCtx : ctx->structField()){
            StructType::Field field;
            field.type = findType(fieldCtx->structFieldType()->getText());
            field.name = fieldCtx->identifier()->getText();
            result->fields.push_back(field);
        }
    }
};

class MethodParser : public CidlBaseListener {
public:
    MethodDesc result;

    void enterMethod(CidlParser::MethodContext* ctx) override {
        result.name = ctx->method_name()->getText();
        result.returnType = findType(ctx->type_name()->getText());


        auto paramList = ctx->method_parameter_list();
        if(paramList != nullptr){
            for(auto& paramCtx : paramList->method_parameter()){
                MethodArg arg;
                arg.name = paramCtx->identifier()->getText();
                if(paramCtx->type_name() != nullptr){
                    arg.type = findType(paramCtx->type_name()->getText());
                    arg.reference = false;
                }else{
                    arg.type = findType(paramCtx->reference_type_name()->type_name()->getText());
                    arg.reference = true;
                }

                //TODO: parse attributes

                result.args.push_back(arg);
            }
        }
    }
};


class AttributeParser : public CidlBaseListener {
public:
    std::shared_ptr<AttributeType> result;
    void enterAttribute(CidlParser::AttributeContext * ctx) override {
        result = std::make_shared<AttributeType>();
        result->name = ctx->identifier()->getText();

        auto argList = ctx->argument_list();

        if(argList != nullptr){
            for(auto& argCtx : ctx->argument_list()->argument()){
                auto expr = argCtx->expression();
                if(expr != nullptr){
                    if(expr->STRING_LITERAL() != nullptr){
                        result->arguments.push_back(std::make_shared<StringConstant>(findType("string"), expr->getText()));
                    }else{
                        auto numeric = expr->numeric_literal();
                        if(numeric->float_literal() != nullptr){
                            if(numeric->float_literal()->FLOAT_SUFFIX() != nullptr){
                                result->arguments.push_back(std::make_shared<FloatConstant>(findType("float"), expr->getText()));
                            }else{
                                result->arguments.push_back(std::make_shared<DoubleConstant>(findType("double"), expr->getText()));
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

    }
};

class InterfaceParser : public CidlBaseListener {
public:
    std::shared_ptr<InterfaceType> result;

    void enterInterfaceDefinition(CidlParser::InterfaceDefinitionContext* ctx) override {
        result = std::make_shared<InterfaceType>();
        result->name = ctx->identifier()->getText();

        result->baseInterfaceType = std::dynamic_pointer_cast<InterfaceType>(findType(ctx->interfaceInheritanceList()->identifier()->getText()));


        auto attributesList = ctx->attribute_list();
        for(auto& attributeCtx : attributesList->attribute()){
            AttributeParser parser;
            attributeCtx->enterRule(&parser);
            result->attributes.push_back(parser.result);
        }

        for(auto& methodCtx : ctx->method()){
            MethodParser parser;
            methodCtx->enterRule(&parser);
            result->methods.push_back(parser.result);
        }
    }
};

class ModuleParser : public CidlBaseListener {
public:
    std::vector<std::string> imports;
    std::vector<std::shared_ptr<EnumType>> enums;
    std::vector<std::shared_ptr<StructType>> structs;
    std::vector<std::shared_ptr<InterfaceType>> interfaces;

    virtual void enterModule(CidlParser::ModuleContext * ctx) override {
        //Get module imports
        auto importList = ctx->importExprList();
        if(importList != nullptr){
            auto expr = importList->importExpr();
            for(auto & import : expr){
                imports.push_back(import->import_file_path()->getText());
            }
        }

        //parse enums
        for(auto& enumType : ctx->enumDefinition()){
            EnumParser parser;
            enumType->enterRule(&parser);
            enums.push_back(parser.result);
        }

        //parse structs
        for(auto& structType : ctx->structDefinition()){
            StructParser parser;
            structType->enterRule(&parser);
            structs.push_back(parser.result);
        }

        //parse interfaces
        for(auto& interfaceType : ctx->interfaceDefinition()){
            InterfaceParser parser;
            interfaceType->enterRule(&parser);
            interfaces.push_back(parser.result);
        }
    }
};




void addIntegralType(const std::string& name) {
    auto t = std::make_shared<Type>();
    t->name = name;
    types.push_back(t);
}


void addFloatType(const std::string& name) {
    auto t = std::make_shared<Type>();
    t->name = name;
    types.push_back(t);
}

void addIUnknownInterfceType() {
    auto t = std::make_shared<InterfaceType>();
    t->name = "IUnknown";
    types.push_back(t);
}

void addStringType() {
    auto t = std::make_shared<Type>();
    t->name = "string";
    types.push_back(t);
}

void addVoidType() {
    auto t = std::make_shared<Type>();
    t->name = "void";
    types.push_back(t);
}

int main() {
    addIntegralType("int8");
    addIntegralType("int16");
    addIntegralType("int32");
    addIntegralType("int64");
    addIntegralType("uint8");
    addIntegralType("uint16");
    addIntegralType("uint32");
    addIntegralType("uint64");

    addFloatType("float");
    addFloatType("double");

    addIUnknownInterfceType();
    addStringType();
    addVoidType();

    std::filesystem::path srcPath(SRC_DIR);

    auto apiPath = srcPath / ".." / "IDL";
    auto testFilePath = apiPath / "Core.cidl";

    auto path = testFilePath.string();
    std::ifstream codeFile (path);
    if (codeFile.is_open()) {
        std::cout << "Code file is open!" << std::endl;
        antlr4::ANTLRInputStream input(codeFile);

        CidlLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);

        tokens.fill();
        /*for (auto token : tokens.getTokens()) {
            std::cout << token->toString() << std::endl;
        }*/

        CidlParser parser(&tokens);
        auto module = parser.module();

        ModuleParser listener;
        module->enterRule(&listener);

        std::cout << "Done" << std::endl;

    }else{
        std::cout << "Failed to read code file!" << std::endl;
    }

    return 0;
}
