// Copyright 2017 Peter Hristov
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <set>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <vector>

#include "./inc/data.hpp"
#include "./inc/disjoint_set.hpp"

using namespace std;

pair<int, int> findCurrent(int current, const vector<vector<int>> &);
vector<vector<int>> sortVertices(vector<vector<int>>);

int compareHeights(const void *a, const void *b) 
{ 
    int i = *(((pair<int, int*>*)a)->second);
    int j = *(((pair<int, int*>*)b)->second);

    int k = Data::compare(((pair<int, int*>*)a)->second, ((pair<int, int*>*)b)->second);

    //cout << "Comparing " << " " << i << " and " << j << " = " << k << endl;
    return k;
}

vector<vector<int>> getJoinTree(const vector<vector<int>> &, const vector<vector<int>> &);
vector<vector<int>> getSplitTree(const vector<vector<int>> &);

bool isThere(vector<pair<int, int>>, pair<int, int>);

int getIndex(vector<vector<int>> vertices, vector<vector<int>> contourTree, int i)
{
    if (-1 == i)
    {
        return -1;
    }

    pair<int, int> p = findCurrent(i, vertices);

    return (p.first * vertices[0].size() + p.second);
}

int getUpDegree(const vector<vector<int>> &tree, int vertex)
{
    int counter = 0;
    for (auto n : tree[vertex])
    {
        if (vertex < n)
        {
            counter++;
        }
    }

    return counter;
}

int getDownDegree(const vector<vector<int>> &tree, int vertex)
{
    return tree[vertex].size() - getUpDegree(tree, vertex);
}

void contractVertex(vector<vector<int>> &tree, int vertex)
{
    // Remove from adj list of neighbours
    for (auto i : tree[vertex])
    {
        tree[i].erase(std::remove(tree[i].begin(), tree[i].end(), vertex), tree[i].end());
    }

    // Add all neighbours to each other
    for (auto i : tree[vertex])
    {
        for (auto j : tree[vertex])
        {
            if (i != j && find(tree[i].begin(), tree[i].end(), j) == tree[i].end())
            {
                tree[i].push_back(j);
            }
        }
    }

    tree[vertex].clear();
}

void generateRandomData(int maxI, int maxJ, int maxVal)
{
    srand (time(NULL));

    cout << maxI << " " << maxJ << endl;

    for (int i = 0 ; i < maxI ; i++)
    {
        for (int j = 0 ; j < maxJ ; j++)
        {
            cout << rand() % maxVal << " ";
        }

        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    //generateRandomData(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

    bool debug = false;

    vector<vector<int>> data = Data::read();

    vector<vector<int>> vertices = sortVertices(data);

    // @TODO Write a routine that checks if sorting is done correctly
    // @TODO Get the Join/Split tree to work

    // cout << "\nData : ";
    // Data::print(data);
    // cout << "\nVertices : ";
    // Data::print(vertices);

    auto joinTree = getJoinTree(vertices, data);
    auto splitTree = getSplitTree(vertices);

    //for (const auto row : joinTree)
    //{
        //for (const auto element : row)
        //{
            //long a = element;
            //long blq = getIndex(vertices, vertices, a);
            //printf("%9ld", blq);
        //}
        //std::cout << std::endl;
    //}

    //return 0;

    // cout << "\nJoin Tree :";
    // Data::printTree(joinTree);
    // cout << "\nSplit Tree :";
    // Data::printTree(splitTree);

    vector<int> q;

    // Queue in all leaves
    for (int i = 0; i < joinTree.size(); i++)
    {
        if (1 == getUpDegree(joinTree, i) + getDownDegree(splitTree, i))
        {
            q.push_back(i);
        }
    }

    vector<vector<int>> contourTree(joinTree.size());

    while (!q.empty())
    {
        // Pop head
        int i = q[0];
        q.erase(q.begin());

        if (debug)
        {
            cout << "\n---------------------------" << endl;

            cout << "Queue       : ";
            for (auto a : q)
            {
                cout << a << " ";
            }
            cout << "\nPopped      : " << i << endl;
            cout << "Up          : " << getUpDegree(joinTree, i) << endl;
            cout << "Down        : " << getDownDegree(splitTree, i) << " ";
        }

        int j = -1;

        if (0 == getUpDegree(joinTree, i) && 1 == getDownDegree(splitTree, i))
        {
            assert(joinTree[i].size() == 1);

            if (debug)
            {
                cout << "+ Up   leaf" << endl;
            }
            j = joinTree[i][0];
        }
        else if (1 == getUpDegree(joinTree, i) && 0 == getDownDegree(splitTree, i))
        {
            assert(splitTree[i].size() == 1);

            if (debug)
            {
                cout << "- Down leaf" << endl;
            }
            j = splitTree[i][0];
        }
        else if (0 == getUpDegree(joinTree, i) && 0 == getDownDegree(splitTree, i))
        {
            break;
        }
        else
        {
            assert(false);
        }

        if (debug)
        {
            cout << "Neighbour   : " << j << endl;
        }

        // Add edge to controur tree
        contourTree[i].push_back(j);
        contourTree[j].push_back(i);

        // Contract used leaf
        contractVertex(joinTree, i);
        contractVertex(splitTree, i);

        if (debug)
        {
            cout << "\nJoin Tree :";
            Data::printTree(joinTree);
            cout << "\nSplit Tree :";
            Data::printTree(splitTree);
            cout << "\nContour Tree :";
            Data::printTree(contourTree);
        }

        if (1 == getUpDegree(joinTree, j) + getDownDegree(splitTree, j))
        {
            q.push_back(j);
        }
    }

    // Remove augmentation
    for (int i = 0; i < contourTree.size(); i++)
    {
        if (1 == getUpDegree(contourTree, i) && 1 == getDownDegree(contourTree, i))
        {
            contractVertex(contourTree, i);
        }
    }

    // cout << "\n\nContour Tree : " << endl;
    // Data::printTreeNonempty(contourTree);

    vector<pair<int, int>> edges;
    for (int i = 0; i < contourTree.size(); i++)
    {
        for (auto j : contourTree[i])
        {
            if (!isThere(edges, make_pair(i, j)))
            {
                edges.push_back(make_pair(i, j));
            }
        }
    }

    for (const auto e : edges)
    {
        long low = getIndex(vertices, contourTree, e.first);
        long high = getIndex(vertices, contourTree, e.second);
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


    //for (int x = 0; x < elements.size(); x++)
    //{
        //cout << x << " , " << elements[x].first << " - " << *elements[x].second << endl;
    //}

    //sort(elements.begin(), elements.end(), [](pair<int, int *> a, pair<int, int *> b) { return (Data::compare(a.second, b.second) < 0); });
    qsort(&elements[0], elements.size(), sizeof elements[0], compareHeights);

    //cout << endl;

    //for (int x = 0; x < elements.size(); x++)
    //{
        //cout << x << " , " << elements[x].first << " - " << *elements[x].second << endl;
    //}

    for (int x = 0; x < elements.size(); x++)
    {
        int i = elements[x].first / data[0].size();
        int j = elements[x].first % data[0].size();

        vertices[i][j] = x;
    }

    return vertices;


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

vector<vector<int>> getJoinTree(const vector<vector<int>> &vertices, const vector<vector<int>> &data)
{
    DisjointSet ds;
    vector<int> lowestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        lowestVertex.push_back(i);
    }

     vector<vector<int>> joinTree(vertices.size() * vertices[0].size());
    //vector<vector<int>> joinTree(vertices.size(), vector<int>(vertices[0].size(), -1));

    for (int i = vertices.size() * vertices[0].size() - 1; i >= 0; i--)
    {
        // Get coordinates of the current vertex
        pair<int, int> currentPosition = findCurrent(i, vertices);

        double value = (double) (data[currentPosition.first][currentPosition.second] * 1.0);

        //cout << "(" << currentPosition.first << ", " << currentPosition.second << ") - " << value << endl;
        //printf("(%d, %d) - %f\n", currentPosition.first, currentPosition.second, value);

        // Get the neighbours of the current vertex
        auto n = Data::getAdjacent(currentPosition.first, currentPosition.second, vertices.size(), vertices[0].size());

        // cout << endl;
        for (pair<int, int> a : n)
        {
            // Get value at vertex
            int j = vertices[a.first][a.second];

            // Check if it's lower or if they are in the same component already
            if (j < i || ds.find(i) == ds.find(j))
            {
                continue;
            }

            // Merge components
            ds.merge(i, j);

            // Add edge to join tree
            //pair<int, int> pos = findCurrent(lowestVertex[ds.find(i)], vertices);
            //joinTree[pos.first][pos.second] = i;

            joinTree[i].push_back(lowestVertex[ds.find(j)]);
            joinTree[lowestVertex[ds.find(j)]].push_back(i);
        }

        // Lowest vertex in the whole new component is now i
        lowestVertex[ds.find(i)] = i;
    }

    return joinTree;

    // joinTree[i].push_back(lowestVertex[ds.find(j)]);
    // joinTree[lowestVertex[ds.find(j)]].push_back(i);
    // return joinTree;
}

vector<vector<int>> getSplitTree(const vector<vector<int>> &vertices)
{
    DisjointSet ds;
    vector<int> highestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        highestVertex.push_back(i);
    }

    vector<vector<int>> splitTree(vertices.size() * vertices[0].size());
    // vector<vector<int>> splitTree(vertices.size(), vector<int>(vertices[0].size()));

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        // Get coordinates of the current vertex
        pair<int, int> currentPosition = findCurrent(i, vertices);

        highestVertex[i] = i;

        // Get the neighbours of the current vertex
        auto n = Data::getAdjacent(currentPosition.first, currentPosition.second, vertices.size(), vertices[0].size());

        for (pair<int, int> a : n)
        {
            int j = vertices[a.first][a.second];

            if (j > i || ds.find(i) == ds.find(j))
            {
                continue;
            }

            ds.merge(i, j);

            splitTree[i].push_back(highestVertex[ds.find(j)]);
            splitTree[highestVertex[ds.find(j)]].push_back(i);

            // pair<int, int> pos = findCurrent(highestVertex[ds.find(j)], vertices);
            // splitTree[pos.first][pos.second] = i;

            // Highest vertex in the whole component is now i
            highestVertex[ds.find(j)] = i;
        }
    }

    return splitTree;
}

bool isThere(vector<pair<int, int>> edges, pair<int, int> p)
{
    for (auto e : edges)
    {
        if ((e.first == p.first && e.second == p.second) || (e.first == p.second && e.second == p.first))
        {
            return true;
        }
    }

    return false;
}
