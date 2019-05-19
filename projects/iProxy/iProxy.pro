TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    iProxy.cpp

HEADERS += \
    iProxy.h \
    svdpi.h \
    gsf_is_pid.h

INCLUDEPATH  += D:/vmshare/usr/include
