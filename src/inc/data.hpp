#ifndef DATA
#define DATA

#include <iostream>
#include <iomanip>
#include <vector>

namespace Data
{
    /**
     * Read data grid from STDIN
     */
    std::vector<std::vector<int>> read()
    {
        int width, height;

        std::cin >> width >> height;

        std::vector<std::vector<int>> data(width, std::vector<int>(height));

        for(int i = 0 ; i < width ; i++)
        {
            for(int j = 0 ; j < height ; j++)
            {
                std::cin >> data[i][j];
            }
        }

        return data;
    }

    /**
     * Write data grid to STDIN
     */
    void print(const std::vector<std::vector<int>> &data)
    {
        std::cout << data.size() << " " << (*data.begin()).size();

        for(const auto row : data)
        {
            std::cout << std::endl;
            for(const auto element : row)
            {
                std::cout << std::setw(4) << element;
            }
        }
    }
}

#endif // DATA
