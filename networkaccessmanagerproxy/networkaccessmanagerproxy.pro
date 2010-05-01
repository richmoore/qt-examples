FORMS +=     networkrequests.ui


SOURCES += main.cpp \
    networkaccessmanagerproxy.cpp \
    networkaccessviewer.cpp

QT += webkit \
    network
HEADERS += networkaccessmanagerproxy.h \
    networkaccessviewer.h
