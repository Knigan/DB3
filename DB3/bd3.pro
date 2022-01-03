QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddMemberDialog.cpp \
    StudentDialog.cpp \
    StudentSignIn.cpp \
    TeacherDialog.cpp \
    connection.cpp \
    createcollectivedialog.cpp \
    deletememberdialog.cpp \
    editprofiledialog.cpp \
    entercollectivedialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AddMemberDialog.h \
    StudentDialog.h \
    StudentSignIn.h \
    TeacherDialog.h \
    connection.h \
    createcollectivedialog.h \
    deletememberdialog.h \
    editprofiledialog.h \
    entercollectivedialog.h \
    mainwindow.h

FORMS += \
    AddMemberDialog.ui \
    StudentDialog.ui \
    StudentSignIn.ui \
    TeacherDialog.ui \
    connection.ui \
    createcollectivedialog.ui \
    deletememberdialog.ui \
    editprofiledialog.ui \
    entercollectivedialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
