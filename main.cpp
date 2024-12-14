#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include "computer.h"
#include "mainwindow1.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow1 window;

    window.show();

    return a.exec();
}
