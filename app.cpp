#include <iostream>
#include <string>

int main()
{
    double gpa;
    std::string name;

    std::cout << "what is your name? ";
    std::cin >> name;

    std::cout << "what is your gpa? ";
    std::cin >> gpa;

    // now get those and print them
    std::cout << "Name: " << name << " GPA: " << gpa << "\n";
    return 0;
}
