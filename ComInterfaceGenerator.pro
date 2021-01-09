TEMPLATE = app
CONFIG += console c++17

INCLUDEPATH += $$PWD/Src \
    $$PWD/generated \
    $$PWD/antlr/runtime

SOURCES += \
    Src/TypeCache.cpp \
    generated/CidlBaseListener.cpp \
    generated/CidlLexer.cpp \
    generated/CidlListener.cpp \
    generated/CidlParser.cpp \
    Src/main.cpp

DEFINES += SRC_DIR="\\\"$$PWD\\\"" \
    ANTLR4CPP_STATIC

LIBS += -L"$$PWD/antlr/lib/vs-2019/x64/Debug Static"  -lantlr4-runtime  -lUser32 -lAdvapi32 -lOle32 -lWinusb -lSetupAPI

HEADERS += \
    Src/Constant/Constant.h \
    Src/Constant/DoubleConstant.h \
    Src/Constant/FloatConstant.h \
    Src/Constant/IntegerConstant.h \
    Src/Constant/NumericConstant.h \
    Src/Constant/StringConstant.h \
    Src/Generator/Attribute/Attribute.h \
    Src/Generator/Attribute/AttributeUtils.h \
    Src/Generator/Attribute/ConstAttribute.h \
    Src/Generator/Attribute/GuidAttribute.h \
    Src/Generator/Attribute/InAttribute.h \
    Src/Generator/Attribute/OutAttribute.h \
    Src/Generator/CSharp/CSharpCodeFile.h \
    Src/Generator/CSharp/CSharpGenerator.h \
    Src/Generator/CodeFile.h \
    Src/Generator/Cpp/CppCodeFile.h \
    Src/Generator/Cpp/CppGenerator.h \
    Src/Generator/ExternalModule.h \
    Src/Generator/Generator.h \
    Src/Generator/Guid.h \
    Src/Parser/AttributeListParser.h \
    Src/Parser/AttributeParser.h \
    Src/Parser/EnumParser.h \
    Src/Parser/InterfaceParser.h \
    Src/Parser/MethodParser.h \
    Src/Parser/ModuleParser.h \
    Src/Parser/StringResolve.h \
    Src/Parser/StructParser.h \
    Src/Parser/TypeNameParser.h \
    Src/Type/AttributeType.h \
    Src/Type/EnumType.h \
    Src/Type/InterfaceType.h \
    Src/Type/Method.h \
    Src/Type/Module.h \
    Src/Type/StructType.h \
    Src/Type/Type.h \
    Src/Type/TypeRef.h \
    Src/TypeCache.h
