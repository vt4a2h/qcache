#include <iostream>

#include "Cache.hpp"

int main()
{
    sc::Cache<int, double> k;
    auto v = k.putValue(3, new double(10));
    std::cout << "v: " << *v << std::endl;
    *v = 11;
    std::cout << "v: " << *v << std::endl;

    auto v1 = k.makeValue(4);
    *v1 = 12;
    std::cout << "v1: " << *v1 << std::endl;

    auto v2 = k.takeValue(4);
    std::cout << "v3: " << *v2 << std::endl;


    return 0;
}
