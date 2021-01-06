TEMPLATE = app
CONFIG += console c++17

INCLUDEPATH += $$PWD/generated \
    $$PWD/antlr/runtime

SOURCES += \
    generated/CidlBaseListener.cpp \
    generated/CidlLexer.cpp \
    generated/CidlListener.cpp \
    generated/CidlParser.cpp \
    Src/main.cpp

DEFINES += SRC_DIR="\\\"$$PWD\\\"" \
    ANTLR4CPP_STATIC

LIBS += -L"$$PWD/antlr/lib/vs-2019/x64/Debug Static"  -lantlr4-runtime  -lUser32 -lAdvapi32 -lOle32 -lWinusb -lSetupAPI
