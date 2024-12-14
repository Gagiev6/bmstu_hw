#include "mainwindow1.h"

void onClick1();
void onClick2();
void onClick3();

MainWindow1::MainWindow1()
{
    readFilesIntoVector("Dbase");

    this->setWindowTitle("Qt5 Extended Example");
    this->resize(500, 300);

    QLabel *label = new QLabel("Программа мониторинга ПК");

    QPushButton *button1 = new QPushButton("Добавить",this);
    QPushButton *button2 = new QPushButton("Редактировать",this);
    QPushButton *button3 = new QPushButton("Просмотр",this);
    QPushButton *button4 = new QPushButton("Удалить",this);
    QPushButton *button5 = new QPushButton("Обновить",this);
    QPushButton *button6 = new QPushButton("Поиск",this);

    zachistka(3);

    searhlineedit = new QLineEdit();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(button4);

    QVBoxLayout *layout2 = new QVBoxLayout();

    layout2->addWidget(listWidget);

    QHBoxLayout *layout3 = new QHBoxLayout();

    layout3->addLayout(layout);
    layout3->addLayout(layout2);

    QHBoxLayout *layout4 = new QHBoxLayout();

    layout4->addWidget(searhlineedit);
    layout4->addWidget(button6);
    layout4->addWidget(button5);

    QVBoxLayout *layout5 = new QVBoxLayout();

    layout5->addWidget(label);
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    this->setLayout(layout5);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::createAddWindow);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow1::createEditWindow);
    QObject::connect(button3, &QPushButton::clicked, this, &MainWindow1::createShowWindow);
    QObject::connect(button4, &QPushButton::clicked, this, &MainWindow1::deleteMethod);
    QObject::connect(button5, &QPushButton::clicked, this, &MainWindow1::updateMethod);
    QObject::connect(button6, &QPushButton::clicked, this, &MainWindow1::searchMethod);
}

// Создаем файлы .txt
void MainWindow1::createFilesFromVector(const QString& directory) {
    QDir dir(directory);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) { // Создаем директорию, если ее нет
            std::cerr << "Ошибка создания директории: " << directory.toStdString() << std::endl;
            return;
        }
    }

    for (Computer& str : Notes) {

        QString filename = QString::number(str.InNumberGet()) + ".xml";
        QString filePath = directory + QDir::separator() + filename;

        QFile file(filePath); //создание файла по пути (включая название файла)
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { //Если файл открылся для записи
            QTextStream out(&file);

            //Тут циклом запись в формате XML
            out << str.InNumberGet() << "\n"; // Записываем строку в файл
            file.close();
        } else {
            std::cerr << "Ошибка открытия файла для записи: " << filePath.toStdString() << std::endl;
        }
    }
}

// Считываем файлы .txt
void MainWindow1::readFilesIntoVector(const QString& directory) {
    QDir dir(directory);
    if (!dir.exists()) {
        std::cerr << "Директория не существует: " << directory.toStdString() << std::endl;
        return;
    }

    if (!Notes.empty()){
    Notes.clear(); // Очищаем вектор перед чтением
    }

    QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files);

    for (const auto& file : files) {
        QString filePath = directory + QDir::separator() + file;
        QFile fileToRead(filePath);
        if (fileToRead.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QString line;
            Computer item;

            QTextStream in(&fileToRead);

            quint64 c = 0;
            line = in.readLine();
            line = in.readLine();
            line = in.readLine();
            line = in.readLine();
            int a=0;
            int b=0;

            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.TypeSet(line.mid(a, b-a).toStdString());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.InNumberSet(line.mid(a,b-a).toInt());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.DesignerSet(line.mid(a,b-a).toStdString());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.ModelSet(line.mid(a,b-a).toStdString());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.MacAdressSet(line.mid(a,b-a).toLongLong());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}
            item.OSSet(line.mid(a,b-a).toStdString());

            line = in.readLine();
            a=0;
            while (line[a] != ">")
            {a++;}
            a+=1;
            b = a;
            while (line[b] != "<")
            {b++;}

            Edates d;
            Edates e;
            d.day = line.mid(a,2).toShort();
            d.month = line.mid(a+3,2).toShort();
            d.year = line.mid(a+6,2).toShort();

            e.day = line.mid(a+9,2).toShort();
            e.month = line.mid(a+12,2).toShort();
            e.year = line.mid(a+15,2).toShort();

            item.DatesSet(d,e);

            Notes.push_back(item);

            if (!Notes.empty()){
                listWidget->addItem(QString::number(Notes.back().InNumberGet()));}
            else {
                std::cerr << "No writing" <<std::endl;
            }
            fileToRead.close();
        } else {
            std::cerr << "Ошибка открытия файла для чтения: " << filePath.toStdString() << std::endl;
        }
    }
}

// Удаление файлов, которых нет в векторе
void MainWindow1::deleteFilesNotInVector(const QString& directory) {
    QDir dir(directory);
    if (!dir.exists()) {
        std::cerr << "Директория не существует: " << directory.toStdString() << std::endl;
        return;
    }

    QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files);
    for (const auto& file : files) {
        QString filename = file.chopped(4);// Удаляет последние 4 символа (".txt")

        // Ищем Computer с именем, совпадающим с именем файла
        auto it = std::find_if(Notes.begin(), Notes.end(), [&](const Computer& comp){
            return std::to_string(comp.InNumberGet()) == filename.toStdString();
        });

        if (it == Notes.end()) { // Если компьютер с таким именем не найден
            QString filePath = directory + QDir::separator() + file;
            if (!dir.remove(filePath)) {
                std::cerr << "Ошибка удаления файла: " << filePath.toStdString() << std::endl;
            }
        }
    }
}

void MainWindow1::zachistka(int a)
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
    if (a == 2)
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

void MainWindow1::createAddWindow()
{
    addW = new QWidget;
    addW->setWindowTitle("Окно Добавления");
    addW->resize(400,300);
    addW->show();

    if (imageLabel != nullptr)
    { delete imageLabel;
        imageLabel = nullptr;}
    if (imageLayout != nullptr)
    { delete imageLayout;
        imageLayout = nullptr;}

    zachistka(1);


    imageLayout = new QVBoxLayout();

    QVBoxLayout *layout1 = new QVBoxLayout();

    QPushButton *button1 = new QPushButton("Добавить", this);
    QPushButton *button2 = new QPushButton("Загрузить изображение", this);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick1);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow1::onClick2);

    lineedit1 = new QLineEdit();
    lineedit2 = new QLineEdit();
    lineedit3 = new QLineEdit();
    lineedit4 = new QLineEdit();
    lineedit5 = new QLineEdit();
    lineedit6 = new QLineEdit();
    lineedit7 = new QLineEdit();
    layout1->addWidget(lineedit1);
    layout1->addWidget(lineedit2);
    layout1->addWidget(lineedit3);
    layout1->addWidget(lineedit4);
    layout1->addWidget(lineedit5);
    layout1->addWidget(lineedit6);
    layout1->addWidget(lineedit7);

    QVBoxLayout *layout2 = new QVBoxLayout();

    QLabel *label1 = new QLabel("Тип компьютера");
    QLabel *label2 = new QLabel("Инвентарный номер");
    QLabel *label3 = new QLabel("Производитель");
    QLabel *label4 = new QLabel("Модель");
    QLabel *label5 = new QLabel("MAC-адресс");
    QLabel *label6 = new QLabel("Операционная система");
    QLabel *label7 = new QLabel("Даты ввода и вывода");
    layout2->addWidget(label1);
    layout2->addWidget(label2);
    layout2->addWidget(label3);
    layout2->addWidget(label4);
    layout2->addWidget(label5);
    layout2->addWidget(label6);
    layout2->addWidget(label7);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(button2);
    layout3->addWidget(button1);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addLayout(imageLayout);
    layout4->addLayout(layout2);
    layout4->addLayout(layout1);


    QVBoxLayout *layout5 = new QVBoxLayout();
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    addW->setLayout(layout5);
}
void MainWindow1::createEditWindow()
{
    if (listWidget->currentItem()){

    if (imageLabel != nullptr)
    { delete imageLabel;
    imageLabel = nullptr;}
    if (imageLayout != nullptr)
    { delete imageLayout;
    imageLayout = nullptr;}

    zachistka(2);

    editW = new QWidget;
    editW->setWindowTitle("Окно Редактирования");
    editW->resize(400,300);
    editW->show();

    int x;
    for (int i=0; i < Notes.size(); ++i) //Посмтреть ещё как можно реализовать
    {
        if (QString::number(Notes[i].InNumberGet()) == listWidget->currentItem()->text())
        {
            x=i;
        }
    }
    QVBoxLayout *layout1 = new QVBoxLayout();

    QPushButton *button1 = new QPushButton("Сохранить",this);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick3);

    lineedit21 = new QLineEdit();
    lineedit22 = new QLineEdit();
    lineedit23 = new QLineEdit();
    lineedit24 = new QLineEdit();
    lineedit25 = new QLineEdit();
    lineedit26 = new QLineEdit();
    lineedit27 = new QLineEdit();


    lineedit21->setText(QString::fromStdString(Notes[x].TypeGet()));
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

    QVBoxLayout *layout2 = new QVBoxLayout();

    QLabel *label1 = new QLabel("Тип компьютера");
    QLabel *label2 = new QLabel("Инвентарный номер");
    QLabel *label3 = new QLabel("Производитель");
    QLabel *label4 = new QLabel("Модель");
    QLabel *label5 = new QLabel("MAC-адресс");
    QLabel *label6 = new QLabel("Операционная система");
    QLabel *label7 = new QLabel("Даты ввода и вывода");
    layout2->addWidget(label1);
    layout2->addWidget(label2);
    layout2->addWidget(label3);
    layout2->addWidget(label4);
    layout2->addWidget(label5);
    layout2->addWidget(label6);
    layout2->addWidget(label7);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(button1);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addLayout(layout2);
    layout4->addLayout(layout1);

    QVBoxLayout *layout5 = new QVBoxLayout();
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    editW->setLayout(layout5);
    }
}
void MainWindow1::createShowWindow()
{
    if (listWidget->currentItem()){

        if (imageLabel != nullptr)
        { delete imageLabel;
            imageLabel = nullptr;}
        if (imageLayout != nullptr)
        { delete imageLayout;
            imageLayout = nullptr;}

    showW = new QWidget;
    showW->setWindowTitle("Окно Просмотра");
    showW->resize(400,300);
    showW->show();


    int x;
    for (int i=0; i < Notes.size(); ++i) //Посмтреть ещё как можно реализовать
    {
        if (QString::number(Notes[i].InNumberGet()) == listWidget->currentItem()->text())
        {
            x=i;
        }
    }


    QVBoxLayout *layout1 = new QVBoxLayout();

    QPushButton *button1 = new QPushButton("Закрыть");

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow1::onClick4);

    QLabel *label12 = new QLabel(QString::fromStdString(Notes[x].TypeGet()));
    QLabel *label21 = new QLabel(QString::number(Notes[x].InNumberGet()));
    QLabel *label31 = new QLabel(QString::fromStdString(Notes[x].DesignerGet()));
    QLabel *label41 = new QLabel(QString::fromStdString(Notes[x].ModelGet()));
    QLabel *label51 = new QLabel(QString::number(Notes[x].MacAdressGet()));
    QLabel *label61 = new QLabel(QString::fromStdString(Notes[x].OSGet()));
    QLabel *label71 = new QLabel(QString::fromStdString(Notes[x].DatesGet()));
    layout1->addWidget(label12);
    layout1->addWidget(label21);
    layout1->addWidget(label31);
    layout1->addWidget(label41);
    layout1->addWidget(label51);
    layout1->addWidget(label61);
    layout1->addWidget(label71);

    QVBoxLayout *layout2 = new QVBoxLayout();

    QLabel *label1 = new QLabel("Тип компьютера");
    QLabel *label2 = new QLabel("Инвентарный номер");
    QLabel *label3 = new QLabel("Производитель");
    QLabel *label4 = new QLabel("Модель");
    QLabel *label5 = new QLabel("MAC-адресс");
    QLabel *label6 = new QLabel("Операционная система");
    QLabel *label7 = new QLabel("Даты ввода и вывода");
    layout2->addWidget(label1);
    layout2->addWidget(label2);
    layout2->addWidget(label3);
    layout2->addWidget(label4);
    layout2->addWidget(label5);
    layout2->addWidget(label6);
    layout2->addWidget(label7);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(button1);

    QHBoxLayout *layout4 = new QHBoxLayout();
    if (Notes[x].ImagePathGet() != "")
    {
        imageLabel = new QLabel(showW);
        imageLayout = new QVBoxLayout();

        QImage image(QString::fromStdString(Notes[x].ImagePathGet()));

        QSize PicSize(150, 150); //Меняем размер на фиксированный
        QPixmap pic2(QPixmap::fromImage(image));
        pic2 = pic2.scaled(PicSize,Qt::KeepAspectRatio);
        imageLabel->setPixmap(pic2);

        imageLayout->addWidget(imageLabel);
        layout4->addLayout(imageLayout);
    }
    layout4->addLayout(layout2);
    layout4->addLayout(layout1);

    QVBoxLayout *layout5 = new QVBoxLayout();
    layout5->addLayout(layout4);
    layout5->addLayout(layout3);

    showW->setLayout(layout5);
    }
}
void MainWindow1::deleteMethod()
{
    if (listWidget->currentItem()){
        for (int i =0; i < Notes.size(); i++)
        {
            if (listWidget->currentItem()->text() == QString::number(Notes[i].InNumberGet()))
            {
                listWidget->takeItem(listWidget->currentRow());

                QString directory = "Dbase";
                QString filename = QString::number(Notes[i].InNumberGet()) + ".xml";
                QString filePath = directory + QDir::separator() + filename;

                QFile file(filePath);
                file.remove();

                if (Notes[i].ImagePathGet() != "")
                {
                    QFile file(QString::fromStdString(Notes[i].ImagePathGet()));
                    file.remove();
                }

                Notes.erase(Notes.begin()+i);
            }
        }
    }
}

void MainWindow1::searchMethod() //Перепроверить работу поиска
{
    for (int i = 0; i <= Notes.size(); i++)
    {
        if(!((searhlineedit->text() == Notes[i].DatesGet())||(searhlineedit->text() == Notes[i].DesignerGet())||(searhlineedit->text() == QString::number(Notes[i].InNumberGet()))
            ||(searhlineedit->text() == QString::number(Notes[i].MacAdressGet()))||(searhlineedit->text() == Notes[i].ModelGet())
              ||(searhlineedit->text() == Notes[i].TypeGet())||(searhlineedit->text() == Notes[i].OSGet())))
        {
            listWidget->takeItem(i);
        }
    }
}
void MainWindow1::updateMethod()
{
    listWidget->clear();
    Notes.clear();
    readFilesIntoVector("Dbase");
}

void MainWindow1::onClick1() //<---------------------------------------Нажатие на кнопку Добавить в окне Добавить------------------------------------------------>
{
    int flag= 0;
    for (int i = 0; i < Notes.size(); i++)
    {
        if (lineedit2->text().toInt() == Notes[i].InNumberGet())
        {flag = 1;}
    }

    if (lineedit1->text() == "" ||lineedit2->text() == "" || lineedit3->text() == ""||lineedit4->text() == ""
        ||lineedit5->text() == ""||lineedit6->text() == ""||lineedit7->text() == "")
    {flag = 2;}
    else if (lineedit2->text().toInt() == 0 ||lineedit5->text().toLongLong() == 0 || lineedit7->text().mid(0,2).toShort() == 0 || lineedit7->text().mid(3,2).toShort() == 0
               || lineedit7->text().mid(6,2).toShort() == 0 || lineedit7->text().mid(9,2).toShort() == 0 || lineedit7->text().mid(12,2).toShort() == 0 || lineedit7->text().mid(15,2).toShort() == 0)
    {flag = 3;}


    if (flag != 0)
    {
        QErrorMessage err;
        if (flag == 1)
        {err.show();
        err.showMessage("ID уже существует");
        }
        if (flag == 2)
        {err.show();err.showMessage("Пустые строки");}
        if (flag == 3)
        {err.show();err.showMessage("Неверный ввод");}
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


    tmp2.day = lineedit7->text().mid(0,2).toShort(); //Даты ввода и вывода (тут структура Dates будет нужна)
    tmp2.month = lineedit7->text().mid(3,2).toShort();
    tmp2.year = lineedit7->text().mid(6,2).toShort();

    tmp3.day = lineedit7->text().mid(9,2).toShort();
    tmp3.month = lineedit7->text().mid(12,2).toShort();
    tmp3.year = lineedit7->text().mid(15,2).toShort();

    tmp.DatesSet(tmp2,tmp2); //Даты

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
        writer.writeTextElement("OSField",QString::fromStdString(tmp.OSGet()));
        writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

        writer.writeEndElement(); // </item>
        writer.writeEndElement(); // </data>
        writer.writeEndDocument();
        file.close();

    } else {
        std::cerr << "Ошибка открытия файла для записи: " << filePath.toStdString() << std::endl;
    }

    if (imageLabel) //Сохранение выбранной картинки в директорию Dbase
    {
        tmp.ImagePathSet(TempPh.toStdString());
        TempIm.save(directory + QDir::separator()+ QString::number(tmp.InNumberGet())+".png");
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
        } else {
            std::cerr << "Ошибка: Не найден layout в MainWindow" << std::endl;
        }
    }

    // Устанавливаем изображение
    imageLayout->addWidget(imageLabel);

    QSize PicSize(150, 150); //Меняем размер на фиксированный
    QPixmap pic2(QPixmap::fromImage(image));
    pic2 = pic2.scaled(PicSize,Qt::KeepAspectRatio);
    imageLabel->setPixmap(pic2);
    TempIm = image;
    TempPh = filePath;
}

void MainWindow1::onClick3()
{
    int flag= 0;

    if (lineedit1->text() == "" ||lineedit2->text() == "" || lineedit3->text() == ""||lineedit4->text() == ""
        ||lineedit5->text() == ""||lineedit6->text() == ""||lineedit7->text() == "")
    {flag = 2;}
    else if (lineedit2->text().toInt() == 0 ||lineedit5->text().toLongLong() == 0 || lineedit7->text().mid(0,2).toShort() == 0 || lineedit7->text().mid(3,2).toShort() == 0
             || lineedit7->text().mid(6,2).toShort() == 0 || lineedit7->text().mid(9,2).toShort() == 0 || lineedit7->text().mid(12,2).toShort() == 0 || lineedit7->text().mid(15,2).toShort() == 0)
    {flag = 3;}


    if (flag != 0)
    {
        QErrorMessage err;
        if (flag == 2)
        {err.showMessage("Пустые строки");}
        if (flag == 3)
        {err.showMessage("Неверный ввод");}
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

    //Проверка циклом строки в lineedit27 верный ли формат?
    tmp2.day = lineedit27->text().mid(0,2).toShort(); //Даты ввода и вывода (тут структура Dates будет нужна)
    tmp2.month = lineedit27->text().mid(3,2).toShort();
    tmp2.year = lineedit27->text().mid(6,2).toShort();

    tmp3.day = lineedit27->text().mid(9,2).toShort();
    tmp3.month = lineedit27->text().mid(12,2).toShort();
    tmp3.year = lineedit27->text().mid(15,2).toShort();

    tmp.DatesSet(tmp2,tmp2); //Даты

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
                //writer.setCodec("iso-8859-1");

                writer.writeStartDocument();
                writer.writeStartElement("data");
                writer.writeStartElement("item");

                writer.writeTextElement("TypeField", QString::fromStdString(tmp.TypeGet()));
                writer.writeTextElement("InNumField", QString::number(tmp.InNumberGet()));
                writer.writeTextElement("DesignerField", QString::fromStdString(tmp.DesignerGet()));
                writer.writeTextElement("ModelField", QString::fromStdString(tmp.ModelGet()));
                writer.writeTextElement("MacAdrField", QString::number(tmp.MacAdressGet()));
                writer.writeTextElement("OSField",QString::fromStdString(tmp.OSGet()));
                writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

                writer.writeEndElement(); // </item>
                writer.writeEndElement(); // </data>
                writer.writeEndDocument();
                file1.close();

            } else {
                std::cerr << "Ошибка открытия файла для записи (after editing): " << filePath.toStdString() << std::endl;
            }

            flag = 1;
        }
        if (currentID == Notes[i].InNumberGet())
        {
            x=i;
        }
    }

    if (!flag)
    {
        Notes.push_back(tmp);
        listWidget->takeItem(listWidget->currentRow());
        listWidget->addItem(QString::number(tmp.InNumberGet()));
        Notes.erase(Notes.begin()+x);

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
            writer.writeTextElement("OSField",QString::fromStdString(tmp.OSGet()));
            writer.writeTextElement("DatesField", QString::fromStdString(tmp.DatesGet()));

            writer.writeEndElement(); // </item>
            writer.writeEndElement(); // </data>
            writer.writeEndDocument();
            file.close();

        } else {
            std::cerr << "Ошибка открытия файла для записи (after editing): " << filePath.toStdString() << std::endl;
        }

    }

    //Реализовать механизм перезаписи в случае если уже есть такой ПК с таким инвентарным номером!



    //Создать фотку с названием (инвентарный номер) в папке Dbase, которую выбрал пользователь (после нажатия на кнопку)

    //createFilesFromVector("Dbase");

    editW->close();
    }
}

void MainWindow1::onClick4()
{
    showW->close();
}
