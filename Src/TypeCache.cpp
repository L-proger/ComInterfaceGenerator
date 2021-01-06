#include "TypeCache.h"
#include <vector>
#include <stdexcept>



std::vector<std::shared_ptr<Type>> TypeCache::types;

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


std::shared_ptr<Type> TypeCache::findType(std::string name) {
    auto it = std::find_if(types.begin(), types.end(), [name](std::shared_ptr<Type> t){  return t->name == name; });
    if(it == types.end()){
        throw std::runtime_error("Type not found: " + name);
    }
    return *it;
}


