// Copyright 2017 Peter Hristov
#include <iostream>
#include <vector>
#include <iomanip>

#include "./inc/disjoint_set.hpp"
#include "./inc/data.hpp"

using namespace std;


int main()
{
    vector<vector<int>>data = Data::read();

    Data::print(data);

    return 0;
}
