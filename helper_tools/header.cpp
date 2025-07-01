#include "header.hpp"

#include <iostream>
using std::cout, std::endl;

int main(void)
{
    constexpr auto found = *FindAuthor("TPK Healy");
    cout << found.name << endl;
}
