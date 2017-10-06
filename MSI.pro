TEMPLATE = app
CONFIG += console xll
CONFIG -= app_bundle

INCLUDEPATH +=  /usr/include\
                /usr/include/x86_64-linux-gnu\
                /usr/local/include\

LIBS += -L/lib -L/usr/lib -L/usr/local/lib \
        -lm -lSDL2 \
        -l826_64\
        -lX11\
        -lavcodec -lavformat -lavutil -lswresample -lswscale\
        -lopencv_core -lopencv_videoio -lopencv_highgui


SOURCES += main.cpp \
    parameters.cpp \
    graphic.cpp \
    evenements.cpp \
    coils.cpp \
    pid.cpp \
    microcam.cpp

HEADERS += \
    parameters.h \
    graphic.h \
    evenements.h \
    coils.h \
    pid.h \
    microcam.h
