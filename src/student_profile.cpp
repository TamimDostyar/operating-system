#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using str = std::string;

int main()
{
    str name;
    char middleName;
    int age;
    double gpa;
    int graduationYear;
    int credit_completion;
    bool is_On_deans;

    // helper
    str standing;

    // collect input
    cout << "What is your name? ";
    cin >> name;

    cout << "What is your middle initial? ";
    cin >> middleName;

    cout << "What is your age? ";
    cin >> age;

    cout << "What is your GPA? ";
    cin >> gpa;

    cout << "What is your graduation year? ";
    cin >> graduationYear;

    cout << "Credits completed? ";
    cin >> credit_completion;

    cout << "On Dean's List? 1 for yes, 0 for no: ";
    cin >> is_On_deans;

    // class standing logic
    if (credit_completion <= 29)
    {
        standing = "Freshman";
    }
    else if (credit_completion <= 59)
    {
        standing = "Sophomore";
    }
    else if (credit_completion <= 89)
    {
        standing = "Junior";
    }
    else
    {
        standing = "Senior";
    }

    // print profile card
    cout << "\n============================" << "\n";
    cout << "     STUDENT PROFILE" << "\n";
    cout << "============================" << "\n";
    cout << "Name: " << name << " " << middleName << "." << "\n";
    cout << "Age: " << age << "\n";
    cout << "GPA: " << gpa << "\n";
    cout << "Graduation Year: " << graduationYear << "\n";
    cout << "Credits: " << credit_completion << "\n";

    cout << "Dean's List: ";
    if (is_On_deans)
    {
        cout << "Yes" << "\n";
    }
    else
    {
        cout << "No" << "\n";
    }

    cout << "Class Standing: " << standing << "\n";
    cout << "Honors Eligible: ";
    if (gpa > 3.5 && is_On_deans)
    {
        cout << "Yes" << "\n";
    }
    else
    {
        cout << "No" << "\n";
    }

    return 0;
}