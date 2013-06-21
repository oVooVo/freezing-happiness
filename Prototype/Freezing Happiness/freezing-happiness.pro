#-------------------------------------------------
#
# Project created by QtCreator 2013-05-14T18:37:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = de

#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    BackEnd/project.cpp \
    BackEnd/Objects/object.cpp \
    BackEnd/Objects/empty.cpp \
    BackEnd/Objects/root.cpp \
    BackEnd/Properties/property.cpp \
    BackEnd/Properties/propertystring.cpp \
    BackEnd/mathutility.cpp \
    FrontEnd/objecttree.cpp \
    FrontEnd/mainwindow.cpp \
    BackEnd/Tags/tag.cpp \
    FrontEnd/viewport.cpp \
    FrontEnd/psrgrabber.cpp \
    FrontEnd/navigationtoolbar.cpp \
    BackEnd/projectcontainer.cpp \
    BackEnd/Objects/spline.cpp \
    FrontEnd/Manager/propertymanager.cpp \
    FrontEnd/Manager/manager.cpp \
    FrontEnd/Manager/objectmanager.cpp \
    FrontEnd/Manager/viewportmanager.cpp \
    FrontEnd/objectedit.cpp \
    FrontEnd/combobox.cpp \
    BackEnd/Objects/instance.cpp \
    BackEnd/Properties/referenceproperty.cpp \
    BackEnd/Tags/constrainttag.cpp \
    BackEnd/Objects/point.cpp \
    BackEnd/Tags/pointtag.cpp \
    BackEnd/renderoptions.cpp \
    BackEnd/Properties/boolproperty.cpp \
    FrontEnd/rendermanager.cpp \
    BackEnd/Objects/camera.cpp \
    BackEnd/Properties/selectproperty.cpp \
    FrontEnd/gradientedit.cpp \
    FrontEnd/gradienteditor.cpp \
    BackEnd/Objects/ellipse.cpp \
    BackEnd/Properties/realproperty.cpp \
    BackEnd/Tags/styletag.cpp \
    BackEnd/Objects/cloner.cpp \
    BackEnd/Properties/compositeproperty.cpp \
    BackEnd/Properties/transformproperty.cpp \
    BackEnd/Properties/integerproperty.cpp \
    BackEnd/Properties/splineproperty.cpp \
    FrontEnd/splineedit.cpp \
    BackEnd/Tags/alignsplinetag.cpp \
    BackEnd/Objects/pathobject.cpp


HEADERS += \
    BackEnd/project.h \
    BackEnd/Objects/object.h \
    BackEnd/Objects/empty.h \
    BackEnd/Objects/root.h \
    BackEnd/Properties/property.h \
    BackEnd/Properties/propertystring.h \
    BackEnd/mathutility.h \
    FrontEnd/objecttree.h \
    FrontEnd/mainwindow.h \
    FrontEnd/viewport.h \
    FrontEnd/psrgrabber.h \
    FrontEnd/navigationtoolbar.h \
    BackEnd/projectcontainer.h \
    BackEnd/Objects/spline.h \
    FrontEnd/Manager/propertymanager.h \
    FrontEnd/Manager/manager.h \
    FrontEnd/Manager/objectmanager.h \
    FrontEnd/Manager/viewportmanager.h \
    FrontEnd/objectedit.h \
    FrontEnd/combobox.h \
    BackEnd/Objects/instance.h \
    BackEnd/Properties/referenceproperty.h \
    BackEnd/Tags/constrainttag.h \
    BackEnd/Objects/point.h \
    BackEnd/Tags/pointtag.h \
    BackEnd/renderoptions.h \
    BackEnd/Properties/boolproperty.h \
    FrontEnd/rendermanager.h \
    BackEnd/Objects/camera.h \
    BackEnd/Properties/selectproperty.h \
    BackEnd/Tags/tag.h \
    FrontEnd/gradientedit.h \
    FrontEnd/gradienteditor.h \
    BackEnd/Objects/ellipse.h \
    BackEnd/Properties/realproperty.h \
    BackEnd/Tags/styletag.h \
    BackEnd/Objects/cloner.h \
    BackEnd/Properties/compositeproperty.h \
    BackEnd/Properties/transformproperty.h \
    BackEnd/Properties/integerproperty.h \
    BackEnd/Properties/splineproperty.h \
    FrontEnd/splineedit.h \
    BackEnd/Tags/alignsplinetag.h \
    BackEnd/Objects/pathobject.h

FORMS += \
    FrontEnd/mainwindow.ui \
    FrontEnd/navigationtoolbar.ui \
    FrontEnd/rendermanager.ui \
    FrontEnd/styleeditor.ui
