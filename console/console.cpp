#include <filesystem>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std::filesystem;
class Human {
public:
    virtual void GetHelath() = 0;
};

class Stas : public Human { 
   
};



int main()
{
    Stas ss;
}

void Stas::GetHelath()
{
}
