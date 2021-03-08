#include <memory>
#include <array>
#include <iostream>
int main()
{

    std::array<std::shared_ptr<int>, 100> a;
    if(a[0] == nullptr)
        std::cout << "yes" << std::endl; 
    a[0] = std::shared_ptr<int>(new int);
    //a[0].reset();
    if(a[0] == nullptr)
        std::cout << "yes" << std::endl; 
    return 0;
}