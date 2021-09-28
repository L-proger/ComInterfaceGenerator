#pragma once

#pragma once

#include <Generator/CodeFile.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>
#include <Type/EnumType.h>
#include <TypeCache.h>
#include <unordered_map>
#include <Generator/Attribute/AttributeUtils.h>
#include <sstream>
#include <Generator/Cpp/CppMarshaler.h>

class CppCodeFile : public CodeFile {
public:
    CppCodeFile() {

    }


    std::string getFileName(std::shared_ptr<Module> module) override {
        return module->name + ".h";
    }

    std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }

    CppCodeFile& beginNamespaceScope(std::string name) {
        name = replaceAll(name, ".", "::");
        write("namespace " + name);
        beginScope(name);
        return *this;
    }

    CppCodeFile& beginScope(std::string name) {
        _currentScopeName = name;
        writeLine("{").ident();
        return *this;
    }
    CppCodeFile& endScope(std::string textAfter) {
        std::string text = "}" + textAfter;
        if(_currentScopeName.size() != 0){
            text += " //" + _currentScopeName;
            _currentScopeName.clear();
        }
        unident().writeLine(text);
        return *this;
    }

    CppCodeFile& endScope() {
        return endScope("");
    }


    CppCodeFile& comment(const std::string& text) {
        write("//" + text);
        return *this;
    }

    std::unordered_map<std::string, std::shared_ptr<Module>> getImportedModules(std::shared_ptr<Module> module) {
        std::unordered_map<std::string, std::shared_ptr<Module>> references;
        for(auto& dep : module->importedTypes){
            auto depModule = TypeCache::findModule(dep->type->moduleName);
            references[depModule->name] = depModule;
        }
        return references;
    }

    void writeModule(std::shared_ptr<Module> module) override {
        _module = module;

        writeLine("#pragma once");
        auto importedModules = getImportedModules(module);

        for(auto& dep : module->importedTypes){
            comment("imported type: " + dep->type->name + " from module: " + dep->type->moduleName).writeLine();
        }

        for(auto& importedModule : importedModules){
            auto m = importedModule.second;
            auto external = std::dynamic_pointer_cast<ExternalModule>(m);

            if(external != nullptr && external->implicitReference){
                continue;
            }

            write("#include <").write(m->name);
            if(external == nullptr){
                write(".h");
            }
            writeLine(">");
        }

        for(auto& type : module->types){
            writeType(type->type);
        }

    }
private:
    std::shared_ptr<Module> _module;
    void writeType(std::shared_ptr<Type> type) {
        auto interfaceType = std::dynamic_pointer_cast<InterfaceType>(type);
        if(interfaceType != nullptr){
            writeInterface(interfaceType);
            return;
        }

        auto structType = std::dynamic_pointer_cast<StructType>(type);
        if(structType != nullptr){
            writeStruct(structType);
            return;
        }

        auto enumType = std::dynamic_pointer_cast<EnumType>(type);
        if(enumType != nullptr){
            writeEnum(enumType);
            return;
        }
    }

    std::string fullName(std::shared_ptr<Type> type){
        auto module = TypeCache::findModule(type->moduleName);
        if(std::dynamic_pointer_cast<ExternalModule>(module) != nullptr){
            return type->name;
        }else{
            auto result = type->moduleName + "." + type->name;
            return replaceAll(result, ".", "::");
        }
    }

    std::string fullName(std::shared_ptr<TypeRef> type){
        return fullName(type->type);
    }


    std::string guidToCode(ComInterfaceGenerator::Guid guid) {
        std::stringstream ss;

        ss << "{ ";
        ss << std::setfill('0') << std::hex;
        ss << "0x" << std::setw(8) << guid.data1;
        ss << ", ";

        ss << "0x" << std::setw(4) << guid.data2;
        ss << ", ";

        ss << "0x" << std::setw(4) << guid.data3;
        ss << ", ";

        ss << "{ ";

        for(int i = 0; i < 8; ++i){
             ss  << "0x" << std::setw(2) << +guid.data4[i];
             if(i != 7){
                ss << ", ";
             }
        }
        ss << " } }";
        return ss.str();
    }

    void writeInterface(std::shared_ptr<InterfaceType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("class ").write(type->name).writeLine(";");
        endScope();


        //Lframework types specialization
        beginNamespaceScope("LFramework");
        writeInterfaceAbi(type);
        writeInterfaceRemap(type);
        writeInterfaceWrapper(type);
        endScope(); //LFramework
        //struct InterfaceAbi<MicroNetwork::Host::INetwork> : public InterfaceAbi<IUnknown> {
    }

    void writeInterfaceAbi(std::shared_ptr<InterfaceType> type) {
        //Write ABI
        comment("Interface ABI").writeLine();
        writeLine("template<>");

        write("struct ").write("InterfaceAbi<").write(fullName(type)).write(">");
        auto baseInterfaceType = std::dynamic_pointer_cast<InterfaceType>(type)->baseInterfaceType;
        if(type->baseInterfaceType != nullptr){
            write(" : public InterfaceAbi<").write(fullName(baseInterfaceType->type)).write(">");
        }
        beginScope(type->name);//ABI
        if(type->baseInterfaceType != nullptr){
            write("using Base = InterfaceAbi<").write(fullName(baseInterfaceType->type)).writeLine(">;");\
        }

        //Write ID()
        auto attrs = AttributeList::parse(type->attributes);
        auto guids = attrs.getAttributes<GuidAttribute>();
        comment("{" + guids[0]->guid.toString() + "}").writeLine();
        write("static constexpr InterfaceID ID() { return ").write(guidToCode(guids[0]->guid)).writeLine("; }");

        //Write abi methods
        for(auto& method : type->methods){
            writeAbiMethod(method);
        }

        //Delete destructor
        unident().write("private:").ident().writeLine().writeLine("~InterfaceAbi() = delete;");

        endScope(";"); //ABI
    }

    void writeInterfaceRemap(std::shared_ptr<InterfaceType> type) {
        comment("Interface Remap").writeLine();
        writeLine("template<class TImplementer>");
        auto baseInterfaceType = std::dynamic_pointer_cast<InterfaceType>(type)->baseInterfaceType;
        write("struct InterfaceRemap<")
                .write(fullName(type))
                .write(", TImplementer> : public InterfaceRemap<")
                .write(fullName(baseInterfaceType->type))
                .write(", TImplementer>");

        beginScope("Remap");

        //unident().write("public:").ident().writeLine();




        //Write remap methods
        for(auto& method : type->methods){
            writeAbiMethodDeclaration(method);
            beginScope("");

            write("try");
            beginScope("");


            if(method.returnsValue()){
                write("result = this->implementer()->");
            }else{
                write("this->implementer()->");
            }




            write(method.name);
            write("(");


            auto args = getAbiMethodArgs(method, false);
            for(std::size_t i = 0; i < args.size(); ++i){
               
                write(args[i].name);
               

                if(i < args.size() - 1){
                    write(", ");
                }
            }
            writeLine(");");


            endScope(); //end try
            write("catch(...)");
            beginScope("");
                writeLine("return LFramework::Result::UnknownFailure;");
            endScope(); //end catch

            writeLine("return LFramework::Result::Ok;");
            endScope();
            //writeLine(" }");
        }

        endScope(";");


    }

    static bool isInterface(MethodArg arg) {
        return std::dynamic_pointer_cast<InterfaceType>(arg.type->type) != nullptr;
    }


    void writeInterfaceWrapperArgMarshaler(const MethodArg& arg, bool methodResult) {
        std::string varName = methodResult ? "result" : arg.name;
        write(CppMarshaler::getMarshalerType(arg)).space().write(CppMarshaler::getVariableName(varName)).write("(").write(varName).writeLine(");");
    }

    void writeInterfaceWrapper(std::shared_ptr<InterfaceType> type) {
        comment("Interface Wrapper").writeLine();
        writeLine("template<>");
        write("class LFramework::InterfaceWrapper<").write(fullName(type)).write(">");
        beginScope("Interface Wrapper");
        unident().write("public:").ident().writeLine();

        for(auto& method : type->methods){
            //return type
            //
            writeWrapperArg(method.returnType, true);
            write(" ").write(method.name).write("(");
            //write(fullName(method.returnType.type)).write(" ").write(method.name).write("(");

            //method args
            for(std::size_t i = 0; i < method.args.size(); ++i){
                writeWrapperArg(method.args[i]);
                if(i < method.args.size() - 1){
                    write(", ");
                }
            }
            write(")");
            beginScope(method.name); //Method body

            //Call

            //declare result variable
            if(method.returnsValue()){
                if (isInterface(method.returnType)) {
                    write("LFramework::ComPtr<").write(fullName(method.returnType.type)).write(">").writeLine(" result;");
                }else if(method.returnType.array){
                    write("std::vector<").write(fullName(method.returnType.type)).write(">").writeLine(" result;");
                }else{
                    write(fullName(method.returnType.type)).writeLine(" result;");
                }

                //marshal result
               /* if (CppMarshaler::shouldMarshal(method.returnType)) {
                    writeInterfaceWrapperArgMarshaler(method.returnType, true);
                }*/
            }

            //marshal args
            for (std::size_t i = 0; i < method.args.size(); ++i) {
                if(CppMarshaler::shouldMarshal(method.args[i])){
                    writeInterfaceWrapperArgMarshaler(method.args[i], false);
                }
            }

            if (method.name == "getNodes") {
                std::cout << "FOUND" << std::endl;
            }

            write("auto comCallResult = _abi->").write(method.name).write("(");
            for(std::size_t i = 0; i < method.args.size(); ++i){
                if (CppMarshaler::shouldMarshal(method.args[i])) {
                    write(CppMarshaler::getVariableName(method.args[i].name));
                }else {
                    write(method.args[i].name);
                }

                if (isInterface(method.args[i]) && !method.args[i].reference) {
                    write(".detach()");
                }

                if (i < method.args.size() - 1) {
                    write(", ");
                }
                
                    
              
            }

            if(method.returnsValue()){
                if (!method.args.empty()) {
                    write(", ");
                }
               

                if(CppMarshaler::shouldMarshal(method.returnType)){
                    if (isInterface(method.returnType) && !method.returnType.reference) {
                        write("result.detach()");
                    }
                    else {
                        write("result");
                    }
                  
                }else{
                    write("result");
                }

            }

            writeLine(");"); //Call end

            write("if(comCallResult != Result::Ok)");

            beginScope("");
            writeLine("throw ComException(comCallResult);");
            endScope();


            if(method.returnsValue()){
                writeLine("return result;");
            }

            endScope(); //End method body
        }

        unident().write("private:").ident().writeLine();
        write("InterfaceAbi<").write(fullName(type)).writeLine(">* _abi;");

        endScope(";");
    }

    void writeStruct(std::shared_ptr<StructType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("struct ").write(type->name);
        beginScope(type->name);
        for(auto& field : type->fields){
            writeStructField(field);

        }
        endScope(";");
        endScope();
    }

    void writeStructField(StructType::Field field){
        write(fullName(field.type->type)).write(" ").write(field.name).writeLine(";");
    }

    void writeEnum(std::shared_ptr<EnumType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("enum ").write(type->name).write(" : ").write(fullName(type->fieldType->type));
        beginScope(type->name);
        for(auto& field : type->fields){
            writeEnumField(field);
        }
        endScope(";");
        endScope();
    }

    void writeEnumField(EnumType::Field field){
        write(field.name).write(" = ").write(field.value->toString()).writeLine(",");
    }

    std::vector<MethodArg> getAbiMethodArgs(MethodDesc& method, bool includeResultType = true){
        std::vector<MethodArg> result;
        result.insert(result.begin(), method.args.begin(), method.args.end());
        if(includeResultType && method.returnsValue()){
            result.push_back(method.returnType);
        }
        return result;
    }

    void writeAbiMethodDeclaration(MethodDesc desc){
        write("virtual Result LFRAMEWORK_COM_CALL ").write(desc.name).write("(");
        auto args = getAbiMethodArgs(desc);

        for(std::size_t i = 0; i < args.size(); ++i){
            writeAbiArg(args[i]);
            if(i < args.size() - 1){
                write(", ");
            }
        }
        write(")");
    }
    void writeAbiMethod(MethodDesc desc) {
        writeAbiMethodDeclaration(desc);
        writeLine(" = 0;");
    }

 
    void writeWrapperArg(MethodArg arg, bool returnType = false) {
        auto attrs = AttributeList::parse(arg.attributes);
        auto constAttr = attrs.getAttribute<ConstAttribute>();
        auto outAttr = attrs.getAttribute<OutAttribute>();

        auto isInterfaceArg = isInterface(arg);

        if (isInterfaceArg) {
            write("LFramework::ComPtr<").write(fullName(arg.type)).write(">");
        }
        else {

            if (arg.array) {
                if (outAttr != nullptr) {
                    write("std::vector<").write(fullName(arg.type)).write(">");
                }
                else {
                    write("const std::vector<").write(fullName(arg.type)).write(">");
                }
                if (!returnType) {
                    write("&");
                }
            }
            else {
                if (constAttr != nullptr) {
                    write("const ");
                }
                write(fullName(arg.type->type));
                if (arg.reference) {
                    if (arg.type->type->name == "void") {
                        write("*");
                    }
                    else {
                        write("&");
                    }
                }
            }
        }


       
        if (!returnType) {
            write(" ");
            write(arg.name);
        }
    }


    void writeAbiArg(MethodArg arg) {
        auto attrs = AttributeList::parse(arg.attributes);
        auto constAttr = attrs.getAttribute<ConstAttribute>();

        bool isInterfaceArg = isInterface(arg);
        if (isInterfaceArg) {
            if (arg.reference) {
                write("LFramework::ComPtr<");
                write(fullName(arg.type->type));
                write(">&");
            }
            else {
                write("LFramework::InterfaceAbi<");
                write(fullName(arg.type->type));
                write(">*");
            }
           // 
        }
        else {
            if (CppMarshaler::shouldMarshal(arg)) {
                write(CppMarshaler::getMarshalerType(arg));
            }
            else {
                if (constAttr != nullptr) {
                    write("const ");
                }
                write(fullName(arg.type->type));
                if (arg.reference) {
                    if (arg.type->type->name == "void") {
                        write("*");
                    }
                    else {
                        write("&");
                    }
                }
            }
        }
      

        write(" ");
        write(arg.name);
    }

    std::string _currentScopeName;
};
