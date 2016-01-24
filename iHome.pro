TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ihomeserver.cpp \
    ihomesession.cpp \
    sconfig.cpp \
    slog.cpp \
    ssocket.cpp \
    digest.cpp \
    md5.cpp \
    jdatabuf.cpp \
    video.cpp \
    yuv2rgb.cpp

HEADERS += \
    ihome.h \
    ihomeserver.h \
    ihomesession.h \
    sconfig.h \
    slog.h \
    ssocket.h \
    md5.h \
    protocol.h \
    ierror.h \
    jdatabuf.h \
    video.h \
    yuv2rgb.h

LIBS = -lpthread -ltinyxml -ljpeg


