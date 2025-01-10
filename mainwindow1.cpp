#include "mainwindow1.h"
#include <QThreadPool>
#include <QRunnable>
#include <QXmlStreamReader>
#include <QBuffer>

class XmlReaderTask : public QRunnable {
public:
    XmlReaderTask(const QString& filePath, QVector<Computer>& notes, QListWidget* listWidget, QMutex& mutex, QWaitCondition& condition)
        : filePath(filePath), notes(notes), listWidget(listWidget), mutex(mutex), condition(condition) {}

    void run() override {
        QFile fileToRead(filePath);
        if (fileToRead.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader xml(&fileToRead);

            while (!xml.atEnd() && !xml.hasError()) {
                QXmlStreamReader::TokenType token = xml.readNext();
                if (token == QXmlStreamReader::StartElement && xml.name() == "item") {
                    Computer item;

                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "item")) {
                        xml.readNext();

                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                            if (xml.name() == "TypeField") {
                                item.TypeSet(xml.readElementText().toStdString());
                            } else if (xml.name() == "InNumField") {
                                item.InNumberSet(xml.readElementText().toInt());
                            } else if (xml.name() == "DesignerField") {
                                item.DesignerSet(xml.readElementText().toStdString());
                            } else if (xml.name() == "ModelField") {
                                item.ModelSet(xml.readElementText().toStdString());
                            } else if (xml.name() == "MacAdrField") {
                                item.MacAdressSet(xml.readElementText().toLongLong());
                            } else if (xml.name() == "OSField") {
                                item.OSSet(xml.readElementText().toStdString());
                            } else if (xml.name() == "DatesField") {
                                QString dates = xml.readElementText();
                                Edates d, e;
                                d.day = dates.mid(0, 2).toShort();
                                d.month = dates.mid(3, 2).toShort();
                                d.year = dates.mid(6, 2).toShort();
                                e.day = dates.mid(9, 2).toShort();
                                e.month = dates.mid(12, 2).toShort();
                                e.year = dates.mid(15, 2).toShort();
                                item.DatesSet(d, e);
                            } else if (xml.name() == "Byte64Field") {
                                item.Base64Set(xml.readElementText());
                            }
                        }
                    }
                    {
                        QMutexLocker locker(&mutex);
                        notes.push_back(item);
                        listWidget->addItem(QString::number(item.InNumberGet()));
                    }
                    condition.wakeAll();
                }

            }

            if (xml.hasError()) {
                std::cerr << "Ошибка при чтении XML-файла: " << xml.errorString().toStdString() << std::endl;
            }
            fileToRead.close();
        } else {
            std::cerr << "Ошибка открытия файла для чтения: " << filePath.toStdString() << std::endl;
        }
    }
private:
    QString filePath;
    QVector<Computer>& notes;
    QListWidget* listWidget;
    QMutex& mutex;
    QWaitCondition& condition;
};

void onClick1();
void onClick2();
void onClick3();
QString imageToBase64(const QImage& image);
QImage base64ToImage(const QString& base64String);

MainWindow1::MainWindow1() //конструктор главного окна
{
    Notes.reserve(100); // Предварительное выделение памяти

    QThreadPool* threadPool = QThreadPool::globalInstance();
    QDir dir("Dbase");

    if (!dir.exists()) {
        std::cerr << "Директория не существует: Dbase" << std::endl;
        return;
    }
    QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files);

    int tasksCount = files.size();

    if (tasksCount > 0){
        for (const auto& file : files) {
            QString filePath = QString("Dbase") + QDir::separator() + file;
            XmlReaderTask* task = new XmlReaderTask(filePath, Notes, listWidget, this->mutex, this->condition);
            threadPool->start(task);
        }

        QMutexLocker locker(&this->mutex);
        while(tasksCount != Notes.size())
        {
            this->condition.wait(&this->mutex);
        }
    }


    this->setWindowTitle("Qt5 Extended Example");
    this->resize(500, 300);

    QLabel* label = new QLabel("Программа мониторинга ПК"); //Дизайним

    QPushButton* button1 = new QPushButton("Добавить", this);
    QPushButton* button2 = new QPushButton("Редактировать", this);
    QPushButton* button3 = new QPushButton("Просмотр", this);
    QPushButton* button4 = new QPushButton("Удалить", this);
    QPushButton* button5 = new QPushButton("Обновить", this);
    QPushButton* button6 = new QPushButton("Поиск", this);

    zachistka(3); // Очистка полей

    searhlineedit = new QLineEdit();
    QVBoxLayout* layout = new QVBoxLayout(); //Дизайн
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(button4);

    QVBoxLayout* layout2 = new QVBoxLayout();

    layout2->addWidget(listWidget);

    QHBoxLayout* layout3 = new QHBoxLayout();

    layout3->addLayout(layout);
    layout3->addLayout(layout2);

    QHBoxLayout* layout4 = new QHBoxLayout();

    layout4->addWidget(searhlineedit);
    layout4->addWidget(button6);
    layout4->addWidget(button5);

    QVBoxLayout* layout5 = new QVBoxLayout();

    layout5->addWidget(label);
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    this->setLayout(layout5);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::createAddWindow); //Соединение кнопок (нажатие) с функционалом (методы)
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow1::createEditWindow);
    QObject::connect(button3, &QPushButton::clicked, this, &MainWindow1::createShowWindow);
    QObject::connect(button4, &QPushButton::clicked, this, &MainWindow1::deleteMethod);
    QObject::connect(button5, &QPushButton::clicked, this, &MainWindow1::updateMethod);
    QObject::connect(button6, &QPushButton::clicked, this, &MainWindow1::searchMethod);
}

// Создаем файлы .xml из вектора-кэша
void MainWindow1::createFilesFromVector(const QString& directory) {
    QDir dir(directory);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) { // Создаем директорию, если ее нет
            std::cerr << "Ошибка создания директории: " << directory.toStdString() << std::endl;
            return;
        }
    }

    for (Computer& str : Notes) { //Циклом проходим по вектору и создаём файлы
        QString filename = QString::number(str.InNumberGet()) + ".xml";
        QString filePath = directory + QDir::separator() + filename;

        QFile file(filePath); //создание файла по пути (включая название файла)
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { //Если файл открылся для записи
            QTextStream out(&file);

            //Тут циклом запись в формате XML
            out << str.InNumberGet() << "\n"; // Записываем строку в файл
            file.close();
        }
        else {
            std::cerr << "Ошибка открытия файла для записи: " << filePath.toStdString() << std::endl;
        }
    }
}

// Считываем файлы .xml в вектор-кэш
void MainWindow1::readFilesIntoVector(const QString& directory) {
    QDir dir(directory); // Создаем директорию
    if (!dir.exists()) {
        std::cerr << "Директория не существует: " << directory.toStdString() << std::endl;
        return;
    }

    if (!Notes.empty()) {
        Notes.clear(); // Очищаем вектор перед чтением
    }

    QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files);

    for (const auto& file : files) { // Цикл по всем XML-файлам в директории
        QString filePath = directory + QDir::separator() + file;
        QFile fileToRead(filePath);
        if (fileToRead.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader xml(&fileToRead);

            while (!xml.atEnd() && !xml.hasError()) {
                QXmlStreamReader::TokenType token = xml.readNext();

                if (token == QXmlStreamReader::StartElement && xml.name() == "item") {
                    Computer item;

                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "item")) {
                        xml.readNext();

                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                            if (xml.name() == "TypeField") {
                                item.TypeSet(xml.readElementText().toStdString());
                            }
                            else if (xml.name() == "InNumField") {
                                item.InNumberSet(xml.readElementText().toInt());
                            }
                            else if (xml.name() == "DesignerField") {
                                item.DesignerSet(xml.readElementText().toStdString());
                            }
                            else if (xml.name() == "ModelField") {
                                item.ModelSet(xml.readElementText().toStdString());
                            }
                            else if (xml.name() == "MacAdrField") {
                                item.MacAdressSet(xml.readElementText().toLongLong());
                            }
                            else if (xml.name() == "OSField") {
                                item.OSSet(xml.readElementText().toStdString());
                            }
                            else if (xml.name() == "DatesField") {
                                QString dates = xml.readElementText();
                                Edates d, e;
                                d.day = dates.mid(0, 2).toShort();
                                d.month = dates.mid(3, 2).toShort();
                                d.year = dates.mid(6, 2).toShort();
                                e.day = dates.mid(9, 2).toShort();
                                e.month = dates.mid(12, 2).toShort();
                                e.year = dates.mid(15, 2).toShort();
                                item.DatesSet(d, e);
                            }
                            else if (xml.name() == "Byte64Field") {
                                item.Base64Set(xml.readElementText());
                            }
                        }
                    }

                    Notes.push_back(item); // Добавляем компьютер в вектор
                    if (!Notes.empty()) {
                        listWidget->addItem(QString::number(Notes.back().InNumberGet()));
                    }
                }
            }

            if (xml.hasError()) {
                std::cerr << "Ошибка при чтении XML-файла: " << xml.errorString().toStdString() << std::endl;
            }

            fileToRead.close();
        }
        else {
            std::cerr << "Ошибка открытия файла для чтения: " << filePath.toStdString() << std::endl;
        }
    }
}

void MainWindow1::zachistka(int a) //Метод зачистки полей (3 режима, в каждом режиме определенные поля чистит)
{
    if (a == 1)
    {
        delete lineedit1;
        lineedit1 = nullptr;
        delete lineedit2;
        lineedit2 = nullptr;
        delete lineedit3;
        lineedit3 = nullptr;
        delete lineedit4;
        lineedit4 = nullptr;
        delete lineedit5;
        lineedit5 = nullptr;
        delete lineedit6;
        lineedit6 = nullptr;
        delete lineedit7;
        lineedit7 = nullptr;
    }
    if (a == 2) //Удаляем мусор, чистим
    {
        delete lineedit21;
        lineedit21 = nullptr;
        delete lineedit22;
        lineedit22 = nullptr;
        delete lineedit23;
        lineedit23 = nullptr;
        delete lineedit24;
        lineedit24 = nullptr;
        delete lineedit25;
        lineedit25 = nullptr;
        delete lineedit26;
        lineedit26 = nullptr;
        delete lineedit27;
        lineedit27 = nullptr;
    }
    if (a == 3)
    {
        delete searhlineedit;
        searhlineedit = nullptr;
    }
}

void MainWindow1::createAddWindow() //Метод создания окна "Добавить" (после нажатия на кнопку в "Добавить" в главном окне)
{
    addW = new QWidget;
    addW->setWindowTitle("Окно Добавления");
    addW->resize(400, 300);
    addW->show();

    if (imageLabel != nullptr) //Чистка полей
    {
        delete imageLabel;
        imageLabel = nullptr;
    }
    if (imageLayout != nullptr)
    {
        delete imageLayout;
        imageLayout = nullptr;
    }

    zachistka(1); //Чистка


    imageLayout = new QVBoxLayout(); //Дизайн

    QVBoxLayout* layout1 = new QVBoxLayout();

    QPushButton* button1 = new QPushButton("Добавить", this);
    QPushButton* button2 = new QPushButton("Загрузить изображение", this);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick1);  //Назначение функционала для кнопок (кнопка нажата -> вызываемый метод)
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow1::onClick2);

    lineedit1 = new QLineEdit();
    lineedit2 = new QLineEdit();
    lineedit3 = new QLineEdit();
    lineedit4 = new QLineEdit();
    lineedit5 = new QLineEdit();
    lineedit6 = new QLineEdit();
    lineedit7 = new QLineEdit();
    layout1->addWidget(lineedit1); //Дизайн
    layout1->addWidget(lineedit2);
    layout1->addWidget(lineedit3);
    layout1->addWidget(lineedit4);
    layout1->addWidget(lineedit5);
    layout1->addWidget(lineedit6);
    layout1->addWidget(lineedit7);

    QVBoxLayout* layout2 = new QVBoxLayout();

    QLabel* label1 = new QLabel("Тип компьютера");
    QLabel* label2 = new QLabel("Инвентарный номер");
    QLabel* label3 = new QLabel("Производитель");
    QLabel* label4 = new QLabel("Модель");
    QLabel* label5 = new QLabel("MAC-адресс");
    QLabel* label6 = new QLabel("Операционная система");
    QLabel* label7 = new QLabel("Даты ввода и вывода");
    layout2->addWidget(label1);
    layout2->addWidget(label2);
    layout2->addWidget(label3);
    layout2->addWidget(label4);
    layout2->addWidget(label5);
    layout2->addWidget(label6);
    layout2->addWidget(label7);

    QHBoxLayout* layout3 = new QHBoxLayout();
    layout3->addWidget(button2);
    layout3->addWidget(button1);

    QHBoxLayout* layout4 = new QHBoxLayout();
    layout4->addLayout(imageLayout);
    layout4->addLayout(layout2);
    layout4->addLayout(layout1);


    QVBoxLayout* layout5 = new QVBoxLayout();
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    addW->setLayout(layout5);
}
void MainWindow1::createEditWindow() //Метод создания окна "Редактировать" (после нажатия на кнопку в "Редактировать" в главном окне)
{
    if (listWidget->currentItem()) { //Если выбран ПК в отображаемом списке

        if (imageLabel != nullptr) //Чистка полей
        {
            delete imageLabel;
            imageLabel = nullptr;
        }
        if (imageLayout != nullptr)
        {
            delete imageLayout;
            imageLayout = nullptr;
        }

        zachistka(2); //Чистка полей

        editW = new QWidget;
        editW->setWindowTitle("Окно Редактирования"); //Дизайн
        editW->resize(400, 300);
        editW->show();

        int x;
        for (int i = 0; i < Notes.size(); ++i) //Проходим циклом по кэшу и ищем выбранный в списке ПК в кэше
        {
            if (QString::number(Notes[i].InNumberGet()) == listWidget->currentItem()->text())
            {
                x = i;
            }
        }
        QVBoxLayout* layout1 = new QVBoxLayout(); //Дизайн

        QPushButton* button1 = new QPushButton("Сохранить", this);

        QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick3); // связывание Кнопки "сохранить изменения" с методом функционалом

        lineedit21 = new QLineEdit();
        lineedit22 = new QLineEdit();
        lineedit23 = new QLineEdit();
        lineedit24 = new QLineEdit();
        lineedit25 = new QLineEdit();
        lineedit26 = new QLineEdit();
        lineedit27 = new QLineEdit();


        lineedit21->setText(QString::fromStdString(Notes[x].TypeGet())); //Автозаполнение полей (когда открываем "Редактировать" поля уже заполнены, тем что есть в ПК)
        lineedit22->setText(QString::number(Notes[x].InNumberGet()));
        lineedit23->setText(QString::fromStdString(Notes[x].DesignerGet()));
        lineedit24->setText(QString::fromStdString(Notes[x].ModelGet()));
        lineedit25->setText(QString::number(Notes[x].MacAdressGet()));
        lineedit26->setText(QString::fromStdString(Notes[x].OSGet()));
        lineedit27->setText(QString::fromStdString(Notes[x].DatesGet()));

        currentID = Notes[x].InNumberGet();

        layout1->addWidget(lineedit21);
        layout1->addWidget(lineedit22);
        layout1->addWidget(lineedit23);
        layout1->addWidget(lineedit24);
        layout1->addWidget(lineedit25);
        layout1->addWidget(lineedit26);
        layout1->addWidget(lineedit27);

        QVBoxLayout* layout2 = new QVBoxLayout(); //Дизайн

        QLabel* label1 = new QLabel("Тип компьютера");
        QLabel* label2 = new QLabel("Инвентарный номер");
        QLabel* label3 = new QLabel("Производитель");
        QLabel* label4 = new QLabel("Модель");
        QLabel* label5 = new QLabel("MAC-адресс");
        QLabel* label6 = new QLabel("Операционная система");
        QLabel* label7 = new QLabel("Даты ввода и вывода");
        layout2->addWidget(label1);
        layout2->addWidget(label2);
        layout2->addWidget(label3);
        layout2->addWidget(label4);
        layout2->addWidget(label5);
        layout2->addWidget(label6);
        layout2->addWidget(label7);

        QHBoxLayout* layout3 = new QHBoxLayout();
        layout3->addWidget(button1);

        QHBoxLayout* layout4 = new QHBoxLayout();
        layout4->addLayout(layout2);
        layout4->addLayout(layout1);

        QVBoxLayout* layout5 = new QVBoxLayout();
        layout5->addLayout(layout4);
        layout5->addLayout(layout3);

        editW->setLayout(layout5);
    }
}
void MainWindow1::createShowWindow() //Метод создания окна "Просмотр" (после нажатия на кнопку в "Просмотр" в главном окне)
{
    if (listWidget->currentItem()) { //Если выбран ПК в отображаемом списке

        if (imageLabel != nullptr) //Чистка
        {
            delete imageLabel;
            imageLabel = nullptr;
        }
        if (imageLayout != nullptr)
        {
            delete imageLayout;
            imageLayout = nullptr;
        }

        showW = new QWidget;
        showW->setWindowTitle("Окно Просмотра");
        showW->resize(400, 300);
        showW->show();

        int x;
        for (int i = 0; i < Notes.size(); ++i) //Проходим циклом по кэшу и ищем выбранный в списке ПК в кэше
        {
            if (QString::number(Notes[i].InNumberGet()) == listWidget->currentItem()->text())
            {
                x = i;
            }
        }

        QVBoxLayout* layout1 = new QVBoxLayout(); //Дизайн

        QPushButton* button1 = new QPushButton("Закрыть");

        QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick4);

        QLabel* label12 = new QLabel(QString::fromStdString(Notes[x].TypeGet())); //Автозаполнение полей (когда открываем "Показать" поля уже заполнены, тем что есть в ПК)
        QLabel* label21 = new QLabel(QString::number(Notes[x].InNumberGet()));
        QLabel* label31 = new QLabel(QString::fromStdString(Notes[x].DesignerGet()));
        QLabel* label41 = new QLabel(QString::fromStdString(Notes[x].ModelGet()));
        QLabel* label51 = new QLabel(QString::number(Notes[x].MacAdressGet()));
        QLabel* label61 = new QLabel(QString::fromStdString(Notes[x].OSGet()));
        QLabel* label71 = new QLabel(QString::fromStdString(Notes[x].DatesGet()));
        layout1->addWidget(label12);
        layout1->addWidget(label21);
        layout1->addWidget(label31);
        layout1->addWidget(label41);
        layout1->addWidget(label51);
        layout1->addWidget(label61);
        layout1->addWidget(label71);

        QVBoxLayout* layout2 = new QVBoxLayout();                          //Дизайн

        QLabel* label1 = new QLabel("Тип компьютера");
        QLabel* label2 = new QLabel("Инвентарный номер");
        QLabel* label3 = new QLabel("Производитель");
        QLabel* label4 = new QLabel("Модель");
        QLabel* label5 = new QLabel("MAC-адресс");
        QLabel* label6 = new QLabel("Операционная система");
        QLabel* label7 = new QLabel("Даты ввода и вывода");
        layout2->addWidget(label1);
        layout2->addWidget(label2);
        layout2->addWidget(label3);
        layout2->addWidget(label4);
        layout2->addWidget(label5);
        layout2->addWidget(label6);
        layout2->addWidget(label7);

        QHBoxLayout* layout3 = new QHBoxLayout();
        layout3->addWidget(button1);

        QHBoxLayout* layout4 = new QHBoxLayout();
        if (this->Notes[x].Base64Get() != "") //Если существует картинка для ПК в записи Базы данных, то её тоже отображаем
        {
            imageLabel = new QLabel(showW); //Устанавливаем элементы дизайна
            imageLayout = new QVBoxLayout();

            QImage image = base64ToImage(this->Notes[x].Base64Get());


            QSize PicSize(150, 150); //Меняем размер на фиксированный
            QPixmap pic2(QPixmap::fromImage(image));
            pic2 = pic2.scaled(PicSize, Qt::KeepAspectRatio); //Подгод размера
            imageLabel->setPixmap(pic2); //Устанавливаем картинку

            imageLayout->addWidget(imageLabel);
            layout4->addLayout(imageLayout);
        }
        layout4->addLayout(layout2);
        layout4->addLayout(layout1);

        QVBoxLayout* layout5 = new QVBoxLayout();
        layout5->addLayout(layout4);
        layout5->addLayout(layout3);

        showW->setLayout(layout5);
    }
}

void MainWindow1::deleteMethod() //Метод удаления, после нажатия на кнопку "Удалить" в главном окне
{
    if (listWidget->currentItem()) { //Если выбран объект в списке ПК
        for (int i = 0; i < Notes.size(); i++) //Проходим по кэшу в поисках нашей записи ПК
        {
            if (listWidget->currentItem()->text() == QString::number(Notes[i].InNumberGet()))
            {
                listWidget->takeItem(listWidget->currentRow()); //Убираем ПК из списка

                QString directory = "Dbase";
                QString filename = QString::number(Notes[i].InNumberGet()) + ".xml";
                QString filePath = directory + QDir::separator() + filename;

                QFile file(filePath);
                file.remove();                                      //Убираем файл ПК

                if (Notes[i].ImagePathGet() != "")
                {
                    QFile file(QString::fromStdString(Notes[i].ImagePathGet())); //Убираем картинку если есть
                    file.remove();
                }

                Notes.erase(Notes.begin() + i); //Убираем ПК из Кэша
            }
        }
    }
}

void MainWindow1::searchMethod() //Метод поиска, после нажатия на кнопку "Поиск" в главном окне
{
    if (searhlineedit->text() != "") {
        for (int i = 0; i < Notes.size(); i++) //Тупо проходим циклом и убираем те записи из списка, где не нашлось точного соответсвия элементов ПК с поисковой строкой
        {
            if (!((searhlineedit->text() == QString::fromStdString(Notes[i].DatesGet())) || (searhlineedit->text() == QString::fromStdString(Notes[i].DesignerGet())) || (searhlineedit->text() == QString::number(Notes[i].InNumberGet()))
                || (searhlineedit->text() == QString::number(Notes[i].MacAdressGet())) || (searhlineedit->text() == QString::fromStdString(Notes[i].ModelGet()))
                || (searhlineedit->text() == QString::fromStdString(Notes[i].TypeGet())) || (searhlineedit->text() == QString::fromStdString(Notes[i].OSGet()))))
            {
                listWidget->takeItem(i);
            }
        }
    }
}
void MainWindow1::updateMethod() //Метод обновления, после нажатия на кнопку "обновить" в главном окне
{
    listWidget->clear();
    Notes.clear();       //Чистим всё
    readFilesIntoVector("Dbase"); //Потом просто вызываем тот же метод что и в Конструкторе, чтообы вновь заполнить
}

void MainWindow1::onClick1() //<---------------------------------------Нажатие на кнопку Добавить в окне Добавить------------------------------------------------>
{
    int flag = 0;
    for (int i = 0; i < Notes.size(); i++)
    {
        if (lineedit2->text().toInt() == Notes[i].InNumberGet())
        {
            flag = 1;
        }
    }

    if (lineedit1->text() == "" || lineedit2->text() == "" || lineedit3->text() == "" || lineedit4->text() == ""
        || lineedit5->text() == "" || lineedit6->text() == "" || lineedit7->text() == "")
    {
        flag = 2;
    }
    else if (lineedit2->text().toInt() == 0 || lineedit5->text().toLongLong() == 0 || lineedit7->text().mid(0, 2).toShort() == 0 || lineedit7->text().mid(3, 2).toShort() == 0
        || lineedit7->text().mid(6, 2).toShort() == 0 || lineedit7->text().mid(9, 2).toShort() == 0 || lineedit7->text().mid(12, 2).toShort() == 0 || lineedit7->text().mid(15, 2).toShort() == 0)
    {
        flag = 3;
    }


    if (flag != 0)
    {
        QErrorMessage err; //<==========================ТУТ ПРОПАДАЕТ ОКНО С ОШИБКОЙ=================================>
        if (flag == 1)
        {
            err.show();
            err.showMessage("ID уже существует");
        }
        if (flag == 2)
        {
            err.show(); err.showMessage("Пустые строки");
        }
        if (flag == 3)
        {
            err.show(); err.showMessage("Неверный ввод");
        }
    }
    else {
        Computer tmp;
        Edates tmp2;
        Edates tmp3;

        tmp.TypeSet(lineedit1->text().toStdString()); //Тип комп
        tmp.InNumberSet(lineedit2->text().toInt()); //Инвентар номер
        tmp.DesignerSet(lineedit3->text().toStdString()); //Производитель
        tmp.ModelSet(lineedit4->text().toStdString()); //Модель
        tmp.MacAdressSet(lineedit5->text().toLongLong()); //Мак адресс
        tmp.OSSet(lineedit6->text().toStdString()); //ОС


        tmp2.day = lineedit7->text().mid(0, 2).toShort(); //Даты ввода и вывода (тут структура Dates будет нужна)
        tmp2.month = lineedit7->text().mid(3, 2).toShort();
        tmp2.year = lineedit7->text().mid(6, 2).toShort();

        tmp3.day = lineedit7->text().mid(9, 2).toShort();
        tmp3.month = lineedit7->text().mid(12, 2).toShort();
        tmp3.year = lineedit7->text().mid(15, 2).toShort();

        tmp.DatesSet(tmp2, tmp2); //Даты

        //Проверка на заполненость ВСЕХ полей!
        //Try catch для каждого из них

        QString directory = "Dbase";

        QDir dir(directory);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) { // Создаем директорию, если ее нет
                std::cerr << "Ошибка создания директории: " << directory.toStdString() << std::endl;
                return;
            }
        }

        QString filename = QString::number(tmp.InNumberGet()) + ".xml";
        QString filePath = directory + QDir::separator() + filename;

        if (imageLabel) //Сохранение выбранной картинки в директорию Dbase
        {
            tmp.ImagePathSet(TempPh.toStdString());
            //TempIm.save(directory + QDir::separator() + QString::number(tmp.InNumberGet()) + ".png");
            tmp.Base64Set(imageToBase64(TempIm)); //Тут сохранение картинки в виде byte64 в структуре Computer по нажатии на кнопку "Добавить"
        }

        QFile file(filePath); //создание файла по пути (включая название файла)
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { //Если файл открылся для записи
            //Запись в файл в формате XML


            QXmlStreamWriter writer(&file);

            writer.setAutoFormatting(true); // Для красивого форматирования XML
            //writer.setCodec("iso-8859-1");

            writer.writeStartDocument();
            writer.writeStartElement("data");
            writer.writeStartElement("item");

            writer.writeTextElement("TypeField", QString::fromStdString(tmp.TypeGet()));
            writer.writeTextElement("InNumField", QString::number(tmp.InNumberGet()));
            writer.writeTextElement("DesignerField", QString::fromStdString(tmp.DesignerGet()));
            writer.writeTextElement("ModelField", QString::fromStdString(tmp.ModelGet()));
            writer.writeTextElement("MacAdrField", QString::number(tmp.MacAdressGet()));
            writer.writeTextElement("OSField", QString::fromStdString(tmp.OSGet()));
            writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

            writer.writeTextElement("Byte64Field", tmp.Base64Get()); // Тут запись байт64 в файл (доработать)

            writer.writeEndElement(); // </item>
            writer.writeEndElement(); // </data>
            writer.writeEndDocument();
            file.close();

        }
        else {
            std::cerr << "Ошибка открытия файла для записи: " << filePath.toStdString() << std::endl;
        }


        Notes.push_back(tmp);

        listWidget->addItem(QString::number(Notes.back().InNumberGet()));
        addW->close();
    }
}

void MainWindow1::onClick2() //Добавление картинки в окне Добавить
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Выберите изображение"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    if (filePath.isEmpty()) {
        return; // Пользователь отменил выбор файла
    }

    QImage image(filePath);
    if (image.isNull()) {
        std::cerr << "Ошибка загрузки изображения: " << filePath.toStdString() << std::endl;
        return;
    }

    // Создаем QLabel, если его еще нет
    if (!imageLabel) {
        imageLabel = new QLabel(this);

        if (imageLayout) {
            imageLayout->addWidget(imageLabel);
        }
        else {
            std::cerr << "Ошибка: Не найден layout в MainWindow" << std::endl;
        }
    }

    // Устанавливаем изображение
    imageLayout->addWidget(imageLabel);

    QSize PicSize(150, 150); //Меняем размер на фиксированный
    QPixmap pic2(QPixmap::fromImage(image));
    pic2 = pic2.scaled(PicSize, Qt::KeepAspectRatio);
    imageLabel->setPixmap(pic2);
    TempIm = image;
    TempPh = filePath;
}

void MainWindow1::onClick3() //кнопка сохранить изменения в окне "Редактировать"
{
    int flag = 0;


    if (lineedit21->text() == "" || lineedit22->text() == "" || lineedit23->text() == "" || lineedit24->text() == ""
        || lineedit25->text() == "" || lineedit26->text() == "" || lineedit27->text() == "")
    {
        flag = 2;
    }
    else if (lineedit22->text().toInt() == 0 || lineedit25->text().toLongLong() == 0 || lineedit27->text().mid(0, 2).toShort() == 0 || lineedit27->text().mid(3, 2).toShort() == 0
        || lineedit27->text().mid(6, 2).toShort() == 0 || lineedit27->text().mid(9, 2).toShort() == 0 || lineedit27->text().mid(12, 2).toShort() == 0 || lineedit27->text().mid(15, 2).toShort() == 0)
    {
        flag = 3;
    }


    if (flag != 0)
    {
        QErrorMessage err;
        if (flag == 2)
        {
            err.showMessage("Пустые строки");
        }
        if (flag == 3)
        {
            err.showMessage("Неверный ввод");
        }
    }
    else {
        Computer tmp;
        Edates tmp2;
        Edates tmp3;

        tmp.TypeSet(lineedit21->text().toStdString()); //Тип комп
        tmp.InNumberSet(lineedit22->text().toInt()); //Инвентар номер
        tmp.DesignerSet(lineedit23->text().toStdString()); //Производитель
        tmp.ModelSet(lineedit24->text().toStdString()); //Модель
        tmp.MacAdressSet(lineedit25->text().toLongLong()); //Мак адресс
        tmp.OSSet(lineedit26->text().toStdString()); //ОС

        tmp2.day = lineedit27->text().mid(0, 2).toShort(); //Даты ввода и вывода (тут структура Dates будет нужна)
        tmp2.month = lineedit27->text().mid(3, 2).toShort();
        tmp2.year = lineedit27->text().mid(6, 2).toShort();

        tmp3.day = lineedit27->text().mid(9, 2).toShort();
        tmp3.month = lineedit27->text().mid(12, 2).toShort();
        tmp3.year = lineedit27->text().mid(15, 2).toShort();

        tmp.DatesSet(tmp2, tmp2); //Даты

        int flag = 0;
        int x = 0;

        for (int i = 0; i < Notes.size(); i++) //Если ID такой же то поднимется флаг (=1)
        {
            if (lineedit22->text().toInt() == Notes[i].InNumberGet())
            {
                Notes[i] = tmp;

                QString directory = "Dbase";
                QString filename = QString::number(Notes[i].InNumberGet()) + ".xml";
                QString filePath = directory + QDir::separator() + filename;

                QFile file(filePath);
                file.remove();

                QFile file1(filePath);

                //создание файла по пути (включая название файла)
                if (file1.open(QIODevice::WriteOnly | QIODevice::Text)) { //Если файл открылся для записи
                    //Запись в файл в формате XML


                    QXmlStreamWriter writer(&file1);

                    writer.setAutoFormatting(true); // Для красивого форматирования XML

                    writer.writeStartDocument();
                    writer.writeStartElement("data");
                    writer.writeStartElement("item");

                    writer.writeTextElement("TypeField", QString::fromStdString(tmp.TypeGet()));
                    writer.writeTextElement("InNumField", QString::number(tmp.InNumberGet()));
                    writer.writeTextElement("DesignerField", QString::fromStdString(tmp.DesignerGet()));
                    writer.writeTextElement("ModelField", QString::fromStdString(tmp.ModelGet()));
                    writer.writeTextElement("MacAdrField", QString::number(tmp.MacAdressGet()));
                    writer.writeTextElement("OSField", QString::fromStdString(tmp.OSGet()));
                    writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

                    writer.writeEndElement(); // </item>
                    writer.writeEndElement(); // </data>
                    writer.writeEndDocument();
                    file1.close();

                }
                else {
                    std::cerr << "Ошибка открытия файла для записи (after editing): " << filePath.toStdString() << std::endl;
                }

                flag = 1;
            }
            if (currentID == Notes[i].InNumberGet())
            {
                x = i;
            }
        }

        if (!flag)
        {
            Notes.push_back(tmp);
            listWidget->takeItem(listWidget->currentRow());
            listWidget->addItem(QString::number(tmp.InNumberGet()));
            Notes.erase(Notes.begin() + x);

            QString directory = "Dbase";

            QString filename = QString::number(tmp.InNumberGet()) + ".xml";
            QString filePath = directory + QDir::separator() + filename;

            QFile file(filePath); //создание файла по пути (включая название файла)
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { //Если файл открылся для записи
                //Запись в файл в формате XML


                QXmlStreamWriter writer(&file);

                writer.setAutoFormatting(true); // Для красивого форматирования XML
                //writer.setCodec("iso-8859-1");

                writer.writeStartDocument();
                writer.writeStartElement("data");
                writer.writeStartElement("item");

                writer.writeTextElement("TypeField", QString::fromStdString(tmp.TypeGet()));
                writer.writeTextElement("InNumField", QString::number(tmp.InNumberGet()));
                writer.writeTextElement("DesignerField", QString::fromStdString(tmp.DesignerGet()));
                writer.writeTextElement("ModelField", QString::fromStdString(tmp.ModelGet()));
                writer.writeTextElement("MacAdrField", QString::number(tmp.MacAdressGet()));
                writer.writeTextElement("OSField", QString::fromStdString(tmp.OSGet()));
                writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

                writer.writeEndElement(); // </item>
                writer.writeEndElement(); // </data>
                writer.writeEndDocument();
                file.close();

            }
            else {
                std::cerr << "Ошибка открытия файла для записи (after editing): " << filePath.toStdString() << std::endl;
            }

        }

        editW->close();
    }
}

void MainWindow1::onClick4()
{
    showW->close();
}

/*#include <QString>
#include <QByteArray>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QUrl>*/

// Метод для преобразования изображения в строку Base64
QString imageToBase64(const QImage& image) {

    // Создаем QByteArray для хранения данных изображения
    QByteArray byteArray;

    QBuffer buffer(&byteArray);


    // Записываем данные изображения в QByteArray в формате PNG

    if (!image.save(&buffer, "PNG")) {
        std::cerr << "Ошибка при сохранении изображения в QByteArray";
        return ""; // Возвращаем пустую строку в случае ошибки
    }

    // Преобразуем QByteArray в строку Base64
    QString base64String = QString::fromLatin1(buffer.data().toBase64());
    return base64String;
}

// Метод для преобразования строки Base64 в изображение
QImage base64ToImage(const QString& base64String) {
    // Преобразуем строку Base64 в QByteArray
    QByteArray byteArray = QByteArray::fromBase64(base64String.toLatin1());

    // Создаем QImage из QByteArray
    QImage image;
    if (!image.loadFromData(byteArray)) {
        std::cerr << "Ошибка при загрузке изображения из QByteArray";
        return QImage(); // Возвращаем пустой QImage в случае ошибки
    }
    return image;
}
