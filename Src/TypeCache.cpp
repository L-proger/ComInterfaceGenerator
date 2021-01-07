#include "TypeCache.h"
#include <vector>
#include <stdexcept>
#include "antlr4-runtime.h"
#include <tree/IterativeParseTreeWalker.h>
#include "CidlLexer.h"
#include "CidlParser.h"
#include "CidlBaseListener.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "tree/ParseTreeListener.h"
#include "tree/ParseTree.h"
#include "tree/ErrorNode.h"
#include <Parser/ModuleParser.h>
#include <stack>
#include <filesystem>
#include <Parser/StringResolve.h>

std::vector<std::shared_ptr<Type>> TypeCache::types;
std::vector<std::shared_ptr<Module>> TypeCache::modules;

std::stack<std::shared_ptr<Module>> moduleParseStack;

std::shared_ptr<Module> TypeCache::findModule(std::string name) {
    auto it = std::find_if(modules.begin(), modules.end(), [name](std::shared_ptr<Module> t){  return t->name == name; });
    if(it == modules.end()){
        return nullptr;
    }
    return *it;
}


void addIUnknownInterfceType() {
    TypeCache::makeType<Type>("IUnknown");
}

void addStringType() {
    TypeCache::makeType<Type>("string");
}

void addVoidType() {
    TypeCache::makeType<Type>("void");
}


void TypeCache::init() {
    TypeCache::makeType<Type>("int8");
    TypeCache::makeType<Type>("int16");
    TypeCache::makeType<Type>("int32");
    TypeCache::makeType<Type>("int64");
    TypeCache::makeType<Type>("uint8");
    TypeCache::makeType<Type>("uint16");
    TypeCache::makeType<Type>("uint32");
    TypeCache::makeType<Type>("uint64");
    TypeCache::makeType<Type>("float");
    TypeCache::makeType<Type>("double");

    addIUnknownInterfceType();
    addStringType();
    addVoidType();
}

std::shared_ptr<Module> TypeCache::resolveImport(const std::string& path){
    auto realPath = StringResolve::resolve(path);
    auto currentModule = moduleParseStack.top();
    auto currentModuleDir = std::filesystem::path(currentModule->path).parent_path();

    auto importPath = currentModuleDir / realPath;

    return parseModule(importPath.string());
}

std::shared_ptr<Module> TypeCache::parseModule(const std::string& path) {

    std::ifstream codeFile (path);
    if (codeFile.is_open()) {
        std::filesystem::path p(path);
        auto moduleName = p.filename().replace_extension().string();

        if(findModule(moduleName) == nullptr){
            antlr4::ANTLRInputStream input(codeFile);

            CidlLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);
            tokens.fill();

            auto currentModule = std::make_shared<Module>();
            currentModule->name = moduleName;
            currentModule->path = path;
            moduleParseStack.push(currentModule);

            CidlParser parser(&tokens);
            auto module = parser.module();

            ModuleParser moduleParser(&TypeCache::resolveImport);
            module->enterRule(&moduleParser);

            currentModule->enums = moduleParser.enums;
            currentModule->structs = moduleParser.structs;
            currentModule->interfaces = moduleParser.interfaces;

            std::cout << "Module " << moduleName << " parse complete" << std::endl;
            return currentModule;
        }else{
            std::cout << "Module " << moduleName << " already parsed" << std::endl;
            return findModule(moduleName);
        }
    }else{
        std::cout << "Failed to read code file!" << std::endl;
        throw std::runtime_error("Module parsing failed");
    }
}



std::shared_ptr<Type> TypeCache::findType(std::string name) {
    auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<Type> t){  return t->name == name; });
    if(it == types.end()){
        throw std::runtime_error("Type not found: " + name);
    }
    return *it;
}


