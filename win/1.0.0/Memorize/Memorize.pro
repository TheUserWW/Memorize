QT += core gui texttospeech charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = Memorize
TEMPLATE = app

SOURCES += main.cpp \
           aboutdialog.cpp \
           addworddialog.cpp \
           flashcard.cpp \
           mainwindow.cpp \
           statistics.cpp \
           testwidget.cpp \
           tutorial.cpp

HEADERS += mainwindow.h \
    aboutdialog.h \
    addworddialog.h \
    flashcard.h \
    statistics.h \
    testwidget.h \
    tutorial.h

FORMS += mainwindow.ui \
    aboutdialog.ui \
    addworddialog.ui \
    flashcarddisplay.ui \
    flashcardsetup.ui \
    statistics.ui \
    testwidget.ui \
    tutorial.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += \
    Memorize_ar_SA.ts \
    Memorize_es_ES.ts \
    Memorize_fr_FR.ts \
    Memorize_kk_CN.ts \
    Memorize_ru_RU.ts \
    Memorize_syr_SY.ts \
    Memorize_ug_CN.ts

RESOURCES += \
    res.qrc

RC_FILE = logo.rc
RC_ICON = app.ico

DISTFILES += \
    logo.rc

DISTFILES +=
