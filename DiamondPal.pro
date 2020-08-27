#-------------------------------------------------
#
# Project DiamondPal
#
#-------------------------------------------------

QT += core gui multimedia serialport opengl
QT += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
requires(qtConfig(combobox))

TARGET = DiamondPal
TEMPLATE = app

HEADERS  += mainwindow.h \
            _glwidget.h \
            _renderer.h \
            _shader.h \
            _sceneentity.h \
            _scene.h \
            _camera.h \
            _assetloader.h \
            _texture.h\
           _tools.h \
           _framebuffer.h

SOURCES += main.cpp\
            mainwindow.cpp \
            _glwidget.cpp \
            _renderer.cpp \
            _shader.cpp \
            _sceneentity.cpp \
            _scene.cpp \
            _camera.cpp \
            _assetloader.cpp \
            _texture.cpp\
           _tools.cpp \
           _framebuffer.cpp

FORMS    += mainwindow.ui

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    models.qrc

INCLUDEPATH += $$PWD/Dependancies/common/glm-0.9.9.3/glm/
DEPENDPATH += $$PWD/Dependancies/common/glm-0.9.9.3/glm/

linux-g++ { #check if platform is linux
message(Linux)
DEFINES += PLATFORM_LINUX=true
INCLUDEPATH += /usr/local/include/dc1394
DEPENDPATH += /usr/local/include/dc1394
LIBS += -L/usr/local/lib -ldc1394
LIBS += -lOpenGL32
}

win32 { #check if platform is windows
DEFINES += PLATFORM_WIN=true
INCLUDEPATH += $$PWD/Dependancies/win/FireGrab/Lib
DEPENDPATH += $$PWD/Dependancies/win/FireGrab/Lib
LIBS +=
}

DISTFILES +=
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependancies/win/FireGrab/Lib/ -lFGCamera
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependancies/win/FireGrab/Lib/ -lFGCamera

