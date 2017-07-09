// Copyright 2017 Peter Hristov
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <set>
#include <stdlib.h> /* srand, rand */

#include "./inc/contour_tree.hpp"
#include "./inc/data.hpp"
#include "./inc/disjoint_set.hpp"

using namespace std;

vector<pair<int, int>> sortedVertices;

vector<int> vertexMap;

int getIndex(vector<vector<int>> vertices, vector<vector<int>>, int);

pair<int, int> findCurrent(int current, const vector<vector<int>> &);
vector<vector<int>> sortVertices(vector<vector<int>>);

int main(int argc, char *argv[])
{
    // generateRandomData(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    bool debug = false;

    vector<vector<int>> data = Data::read();
    vector<vector<int>> vertices = sortVertices(data);

    if (debug)
    {
        cout << "\nData : ";
        Data::print(data);
        cout << "\nVertices : ";
        Data::print(vertices);
    }

    auto joinTree = ContourTree::getJoinTree(vertices, sortedVertices);
    auto splitTree = ContourTree::getSplitTree(vertices, sortedVertices);

    if (debug)
    {
        cout << "\nJoin Tree :";
        Data::printTree(joinTree);
        cout << "\nSplit Tree :";
        Data::printTree(splitTree);
    }

    // Get Contour Tree
    vector<vector<int>> contourTree = ContourTree::getContourTree(joinTree, splitTree, debug);

    // Remove augmentation
    ContourTree::removeAugmentation(contourTree);

    // Conver to edge list format
    auto edges = ContourTree::convertToEdges(contourTree);

    for (const auto e : edges)
    {
        //long low = getIndex(vertices, contourTree, e.first);
        //long high = getIndex(vertices, contourTree, e.second);

        long low = vertexMap[e.first];
        long high = vertexMap[e.second];

        printf("%12ld %12ld\n", min(low, high), max(low, high));
    }

    return 0;
}

/**
 * Maps 1D indices to the 2D grid
 */
pair<int, int> findCurrent(int current, const vector<vector<int>> &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        for (int j = 0; j < data[i].size(); j++)
        {
            if (data[i][j] == current)
            {
                return make_pair(i, j);
            }
        }
    }
    return make_pair(-1, -1);
}

/**
 * @TODO Remove the back from here
 */
vector<vector<int>> sortVertices(vector<vector<int>> data)
{
    std::vector<std::vector<int>> vertices(data.size(), std::vector<int>(data[0].size()));

    // Linearise array in order to sort it
    vector<pair<int, int *>> elements;
    for (int x = 0; x < data.size(); x++)
    {
        for (int y = 0; y < data[x].size(); y++)
        {
            int index = x * data[0].size() + y;
            int *value = &data[x][y];

            elements.push_back(make_pair(index, value));
        }
    }

    // for (int x = 0; x < elements.size(); x++)
    //{
    // cout << x << " , " << elements[x].first << " - " << *elements[x].second << endl;
    //}

    sort(elements.begin(), elements.end(), [](pair<int, int *> a, pair<int, int *> b) { return (Data::compare(a.second, b.second) < 0); });

    // cout << endl;

    // for (int x = 0; x < elements.size(); x++)
    //{
    // cout << x << " , " << elements[x].first << " - " << *elements[x].second << endl;
    //}

    for (int x = 0; x < elements.size(); x++)
    {
        int i = elements[x].first / data[0].size();
        int j = elements[x].first % data[0].size();

        sortedVertices.push_back(make_pair(i, j));

        vertexMap.push_back(elements[x].first);

        vertices[i][j] = x;
    }

    return vertices;

    // Dead Weight

    for (int x = 0; x < data.size(); x++)
    {
        for (int y = 0; y < data[x].size(); y++)
        {
            pair<int, int> max(x, y);

            for (int i = 0; i < data.size(); i++)
            {
                for (int j = 0; j < data[i].size(); j++)
                {
                    // if (data[i][j] < data[max.first][max.second])
                    if (Data::compare(&data[i][j], &data[max.first][max.second]) < 0)
                    {
                        max = make_pair(i, j);
                    }
                }
            }

            // @TODO this is the hack
            vertices[max.first][max.second] = x * data[0].size() + y;
            data[max.first][max.second] = std::numeric_limits<int>::max();
        }
    }

    return vertices;
}

int getIndex(vector<vector<int>> vertices, vector<vector<int>> contourTree, int i)
{
    if (-1 == i)
    {
        return -1;
    }

    pair<int, int> p = findCurrent(i, vertices);

    return (p.first * vertices[0].size() + p.second);
}
