TEMPLATE = app
CONFIG += console c++17

include(Dependencies/Dependencies.pri)

SOURCES += \
    Library.cpp \
    main.cpp

HEADERS += \
    LFrameworkConfig.h \
    Library.h

LIBS += -lUser32 -lAdvapi32 -lOle32 -lWinusb -lSetupAPI
