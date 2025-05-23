QT += core gui widgets

CONFIG += c++17

HEADERS += \
    chat_client.hpp \
    functions.hpp

SOURCES += \
    chat_client.cpp \
    functions.cpp \
    main.cpp

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic

# Installation (optional)
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

RESOURCES += resources.qrc
