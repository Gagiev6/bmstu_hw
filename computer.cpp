#include "computer.h"


void Computer::InNumberSet(int a)
{
    InNumber = a;
}
void Computer::TypeSet(std::string a)
{
    Type = a;
}
void Computer::DesignerSet(std::string a)
{
    Designer = a;
}
void Computer::ModelSet(std::string a)
{
    Model = a;
}
void Computer::OSSet(std::string a)
{
    OS = a;
}

void Computer::ImagePathSet(std::string a)
{
    ImagePath = a;
}

void Computer::Base64Set(QString a)
{
    Base64 = a;
}

void Computer::DatesSet(Edates a, Edates b)
{
    Dates.first.day = a.day;
    Dates.first.month = a.month;
    Dates.first.year = a.year;
    Dates.second.day = b.day;
    Dates.second.month = b.month;
    Dates.second.year = b.year;
}
void Computer::MacAdressSet(long long a)
{
    MacAdress = a;
}

int Computer::InNumberGet() const
{
    return InNumber;
}

std::string Computer::TypeGet() const
{
    return Type;
}

std::string Computer::DesignerGet() const
{
    return Designer;
}

std::string Computer::ModelGet() const
{
    return Model;
}

std::string Computer::OSGet() const
{
    return OS;
}

std::string Computer::ImagePathGet() const
{
    return ImagePath;
}

QString Computer::Base64Get() const
{
    return Base64;
}

std::string Computer::DatesGet() const
{
    std::string a;
    a = std::to_string(Dates.first.day) + "." + std::to_string(Dates.first.month);
    a = a + "." + std::to_string(Dates.first.year) + " " + std::to_string(Dates.second.day) + ".";
    a = a + std::to_string(Dates.second.month) + "." + std::to_string(Dates.second.year);

    return a;
}

long long Computer::MacAdressGet() const
{
    return MacAdress;
}

Computer::Computer()
{}

Computer::~Computer()
{}
