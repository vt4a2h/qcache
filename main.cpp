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

    k.removeValue(3);

    auto v3 = k.makeValue(5);
    *v3 = 10;

    auto v3_3 = k[5];
    k.clear();

    return 0;
}
