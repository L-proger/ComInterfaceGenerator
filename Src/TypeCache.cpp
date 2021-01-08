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

#include <filesystem>
#include <Parser/StringResolve.h>

const char* TypeCache::primitiveModuleName(){
    return "$primitive";
}


std::vector<std::shared_ptr<Type>> TypeCache::types;
std::vector<std::shared_ptr<Module>> TypeCache::modules;
std::stack<std::shared_ptr<Module>> TypeCache::moduleParseStack;
std::vector<std::filesystem::path> searchPaths;

std::shared_ptr<Module> TypeCache::findModule(std::string name) {
    auto it = std::find_if(modules.begin(), modules.end(), [name](std::shared_ptr<Module> t){  return t->name == name; });
    if(it == modules.end()){
        return nullptr;
    }
    return *it;
}

std::filesystem::path TypeCache::findModulePath(const std::string& name){
    std::string ext = ".cidl";
    std::string fileName = name + ext;

    for(auto& searchPath : searchPaths){
        auto modulePath = searchPath / fileName;
        if(std::filesystem::exists(modulePath)){
            return modulePath;
        }
    }
    throw std::exception("Failed to find module path");
}

void TypeCache::addSearchPath(std::filesystem::path path){
    searchPaths.push_back(path);
}

void TypeCache::init() {
    auto primitiveModule = std::make_shared<Module>();
    primitiveModule->name = primitiveModuleName();
    primitiveModule->path = "";
    modules.push_back(primitiveModule);

    TypeCache::makePrimitiveType<Type>("int8");
    TypeCache::makePrimitiveType<Type>("int16");
    TypeCache::makePrimitiveType<Type>("int32");
    TypeCache::makePrimitiveType<Type>("int64");
    TypeCache::makePrimitiveType<Type>("uint8");
    TypeCache::makePrimitiveType<Type>("uint16");
    TypeCache::makePrimitiveType<Type>("uint32");
    TypeCache::makePrimitiveType<Type>("uint64");
    TypeCache::makePrimitiveType<Type>("float");
    TypeCache::makePrimitiveType<Type>("double");
    TypeCache::makePrimitiveType<InterfaceType>("IUnknown");
    TypeCache::makePrimitiveType<Type>("string");
    TypeCache::makePrimitiveType<Type>("void");
}

std::shared_ptr<Module> TypeCache::resolveImport(const std::string& path){
    auto realPath = StringResolve::resolve(path);
    auto currentModule = moduleParseStack.top();
    auto currentModuleDir = std::filesystem::path(currentModule->path).parent_path();

    auto importPath = currentModuleDir / realPath;

    return parseModule(importPath.string());
}

std::shared_ptr<Module> TypeCache::parseModule(const std::string& name) {

    auto path = findModulePath(name);
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
            modules.push_back(currentModule);
            moduleParseStack.push(currentModule);

            CidlParser parser(&tokens);
            auto module = parser.module();

            ModuleParser moduleParser(&TypeCache::resolveImport, currentModule);
            module->enterRule(&moduleParser);

            std::cout << "Module " << moduleName << " parse complete" << std::endl;
            moduleParseStack.pop();
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

const std::vector<std::shared_ptr<Type>>& TypeCache::getTypes(){
    return types;
}
const std::vector<std::shared_ptr<Module>>& TypeCache::getModules(){
    return modules;
}

std::shared_ptr<Type> TypeCache::findOrDefineReferencedType(TypeName name) {
    if(name.isPrimitive){
        name.module = primitiveModuleName();
    }

    //


    if(!name.isPrimitive && name.isLocalType){
        name.module = moduleParseStack.top()->name;
    }

    if(!name.isLocalType && !name.isPrimitive){
        if(findModule(name.module) == nullptr){
            std::cout << "Parse dependent module: " << name.module << std::endl;
            parseModule(name.module);
        }
    }

    auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<Type> t){  return t->name == name.name && t->moduleName == name.module; });
    if(it != types.end()){
        if(!name.isLocalType){
            auto remoteModule = findModule(name.module);
            moduleParseStack.top()->addDependency(remoteModule, *it);
        }

        return *it;
    }else{
        throw std::runtime_error("Type not found: " + name.module + "::" + name.name);
    }
}

std::shared_ptr<Type> TypeCache::findPrimitiveType(std::string name) {
    TypeName tn;
    tn.module = primitiveModuleName();
    tn.isLocalType = false;
    tn.isPrimitive = true;
    tn.name = name;
    return findOrDefineReferencedType(tn);
}


void TypeCache::replaceType(std::shared_ptr<Type> srcType, std::shared_ptr<Type> dstType, const std::string moduleName){
    auto module = findModule(srcType->moduleName);
    if(module == nullptr){
        throw new std::runtime_error("Can not replace type. Module not found:");
    }
}



