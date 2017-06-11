#ifndef DISJOINT_SET
#define DISJOINT_SET

#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

// Vertex data structure used by RTF
class DisjointSet
{
    std::vector<std::pair<int, int>> nodes;

  public:
    void add(int value)
    {
        this->nodes.push_back(std::make_pair(value, this->nodes.size()));
    }

    int find(int i)
    {
        int p = nodes[i].second;

        if (p == i)
        {
            return p;
        }

        int root = find(p);

        // Path compression
        nodes[i].second = root;

        return root;
    }

    void merge(int i, int j)
    {
        int rootI = find(i);
        int rootJ = find(j);

        nodes[rootI].second = rootJ;
    }

    void print()
    {
        std::cout << "\n\nIndices";

        int w = 4;

        for (auto t = 0; t < this->nodes.size(); t++)
        {
            std::cout << std::setw(w) << t;
        }

        std::cout << std::endl;

        std::cout << "Values ";
        for (auto t : this->nodes)
        {
            std::cout << std::setw(w) << t.first;
        }

        std::cout << std::endl;

        std::cout << "Parents";
        for (auto t : this->nodes)
        {
            std::cout << std::setw(w) << t.second;
        }

        std::cout << std::endl;
    }
};

// Alternative find.
// int find(int i)
//{
// int p = this->nodes[i].second;

// while (p != nodes[p].second)
//{
// p = nodes[p].second;
//}

// return p;
//}

#endif // DISJOINT_SET
