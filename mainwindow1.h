#ifndef MAINWINDOW1_H
#define MAINWINDOW1_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QErrorMessage>
#include <iostream>
#include "computer.h"

class MainWindow1 : public QWidget
{
    std::vector<Computer> Notes; //Кэш
    QListWidget *listWidget = new QListWidget; //Отображаемый в главном окне список

    QWidget *addW;    //Указатели на новые окна
    QWidget *editW;
    QWidget *showW;

    QLineEdit *searhlineedit = nullptr; // Поле поиска в главном окне

    QLayout *imageLayout = nullptr;
    QLabel *imageLabel = nullptr; //Картинка
    QImage TempIm;
    QString TempPh;

    int currentID = 0;

    QLineEdit *lineedit1 = nullptr;
    QLineEdit *lineedit2 = nullptr;
    QLineEdit *lineedit3 = nullptr;
    QLineEdit *lineedit4 = nullptr;
    QLineEdit *lineedit5 = nullptr;
    QLineEdit *lineedit6 = nullptr;
    QLineEdit *lineedit7 = nullptr;
    //Извините....
    QLineEdit *lineedit21 = nullptr;
    QLineEdit *lineedit22 = nullptr;
    QLineEdit *lineedit23 = nullptr;
    QLineEdit *lineedit24 = nullptr;
    QLineEdit *lineedit25 = nullptr;
    QLineEdit *lineedit26 = nullptr;
    QLineEdit *lineedit27 = nullptr;
public:
    MainWindow1();
    void zachistka(int a);
    void createAddWindow();
    void createEditWindow();
    void createShowWindow();
    void deleteMethod();
    void searchMethod();
    void updateMethod();
    void createFilesFromVector(const QString& directory);
    void readFilesIntoVector(const QString& directory);
    void deleteFilesNotInVector(const QString& directory);
    //void loadImage(const QString& filePath);
public slots:
    void onClick1();
    void onClick2();
    void onClick3();
    void onClick4();
};

#endif // MAINWINDOW1_H
