#include <string>
#include <QLabel>

#ifndef COMPUTER_H
#define COMPUTER_H

struct Edates //Структура для дат
{
    unsigned short day;
    unsigned short month;
    unsigned short year;
};

class Computer
{
    int InNumber; //Инвентарный номер (идентификатор!)

    std::string Type; //Тип ПК
    std::string Designer; //Разработчик
    std::string Model; //Модель
    std::string OS; // Установленная ОС
    std::string ImagePath;
    QString Base64;

    std::pair<Edates, Edates> Dates; //Даты ввода и вывода
    long long MacAdress; //Тут нужно реализовать 16-тиричную систему(?)

public: //Ещё будут дополняться

    void InNumberSet(int a);
    void TypeSet(std::string a);
    void DesignerSet(std::string a);
    void ModelSet(std::string a);
    void OSSet(std::string a);
    void ImagePathSet(std::string a);
    void Base64Set(QString a);

    void DatesSet(Edates a, Edates b);
    void MacAdressSet(long long a);

    int InNumberGet() const;
    std::string TypeGet() const;
    std::string DesignerGet() const;
    std::string ModelGet() const;
    std::string OSGet() const;
    std::string ImagePathGet() const;
    QString Base64Get() const;

    std::string DatesGet() const;
    long long MacAdressGet() const;

    bool operator==(const Computer& other) const {
        return InNumber == other.InNumber;
    }

    Computer();
    ~Computer();
};

#endif // COMPUTER_H
