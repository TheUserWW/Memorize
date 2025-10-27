#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <Windows.h>
#include <ShellAPI.h>



int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
