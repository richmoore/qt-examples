TEMPLATE = app
TARGET = weblist
include (../examples.pri)

CONFIG -= app_bundle
QT += webkit

HEADERS += webview.h
SOURCES += main.cpp \
           webview.cpp
