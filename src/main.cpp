// Copyright 2017 Peter Hristov
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include <assert.h>

#include "./inc/data.hpp"
#include "./inc/disjoint_set.hpp"

using namespace std;

pair<int, int> findCurrent(int current, const vector<vector<int>> &);
vector<vector<int>> sortVertices(vector<vector<int>>);

vector<vector<int>> getJoinTree(const vector<vector<int>> &);
vector<vector<int>> getSplitTree(const vector<vector<int>> &);

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

int main()
{
    bool debug = false;

    vector<vector<int>> data = Data::read();
    vector<vector<int>> vertices = sortVertices(data);

    auto joinTree = getJoinTree(vertices);
    auto splitTree = getSplitTree(vertices);

    cout << "\nData : ";
    Data::print(data);
    cout << "\nVertices : ";
    Data::print(vertices);

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

            if (debug) { cout << "+ Up   leaf" << endl; }
            j = joinTree[i][0];

        }
        else if (1 == getUpDegree(joinTree, i) && 0 == getDownDegree(splitTree, i))
        {
            assert(splitTree[i].size() == 1);

            if (debug) { cout << "- Down leaf" << endl; }
            j = splitTree[i][0];
        }
        else
        {
            assert(false);
        }

        if (debug) { cout << "Neighbour   : " << j << endl; }

        // @TODO remove this hack
        if (j == -1)
        {
            break;
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


        // If the queue is empty add new leaves
        if (q.empty())
        {
            for (int i = 0; i < joinTree.size(); i++)
            {
                if (1 == getUpDegree(joinTree, i) + getDownDegree(splitTree, i))
                {
                    q.push_back(i);
                }
            }
        }
    }

    Data::printTree(contourTree);
    Data::printExtremeTree(contourTree);

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

    for (int x = 0; x < data.size(); x++)
    {
        for (int y = 0; y < data[x].size(); y++)
        {
            pair<int, int> max(x, y);

            for (int i = 0; i < data.size(); i++)
            {
                for (int j = 0; j < data[i].size(); j++)
                {
                    if (data[i][j] < data[max.first][max.second])
                    {
                        max = make_pair(i, j);
                    }
                }
            }

            // @TODO this is the hack
            vertices[max.first][max.second] = x * data.size() + y;
            data[max.first][max.second] = std::numeric_limits<int>::max();
        }
    }

    return vertices;
}

vector<vector<int>> getJoinTree(const vector<vector<int>> &vertices)
{
    DisjointSet ds;
    vector<int> lowestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        lowestVertex.push_back(i);
    }

    vector<vector<int>> joinTree(vertices.size() * vertices[0].size());

    for (int i = vertices.size() * vertices[0].size() - 1; i >= 0; i--)
    {
        // Get coordinates of the current vertex
        pair<int, int> currentPosition = findCurrent(i, vertices);

        // Get the neighbours of the current vertex
        auto n = Data::getAdjacent(currentPosition.first, currentPosition.second, vertices.size(), vertices[0].size());
        for (pair<int, int> a : n)
        {
            int j = vertices[a.first][a.second];

            if (j < i || ds.find(i) == ds.find(j))
            {
                continue;
            }

            ds.merge(i, j);

            joinTree[i].push_back(lowestVertex[ds.find(j)]);
            joinTree[lowestVertex[ds.find(j)]].push_back(i);

            // Lowest vertex in the whole component is now i
            lowestVertex[ds.find(j)] = i;
        }
    }

    // return joinTree;
    return joinTree;
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

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        // Get coordinates of the current vertex
        pair<int, int> currentPosition = findCurrent(i, vertices);

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

            // Highest vertex in the whole component is now i
            highestVertex[ds.find(j)] = i;
        }
    }

    return splitTree;
}
