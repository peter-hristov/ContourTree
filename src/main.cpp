// Copyright 2017 Peter Hristov
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include "./inc/data.hpp"
#include "./inc/disjoint_set.hpp"

using namespace std;

pair<int, int> findCurrent(int current, const vector<vector<int>> &);
vector<vector<int>> sortVertices(vector<vector<int>>);

vector<pair<int, int>> getJoinTree(const vector<vector<int>> &);
vector<pair<int, int>> getSplitTree(const vector<vector<int>> &);

vector<int> getNeighbours(vector<pair<int, int>> edges, int vertex)
{
    vector<int> neighbours;

    for (auto edge : edges)
    {
        if (edge.first == vertex || edge.second == vertex)
        {
            neighbours.push_back(edge.second);
        }
    }

    return neighbours;
}

int getDegree(vector<pair<int, int>> edges, int vertex)
{
    int degree = 0;

    for (auto edge : edges)
    {
        if (edge.first == vertex)
        {
            degree++;
        }
    }

    return degree;
}

vector<int> getLeaves(const vector<pair<int, int>> &joinTree, const vector<pair<int, int>> &splitTree, int n)
{
    vector<int> leaves;
    for (int i = 0; i < n; i++)
    {
        if (1 == getDegree(joinTree, i) + getDegree(splitTree, i))
        {
            leaves.push_back(i);
        }
    }

    return leaves;
}

vector<pair<int, int>> removeVertex(const vector<pair<int, int>> &tree, int vertex)
{
    vector<pair<int, int>> edges;

    for (auto edge: tree)
    {
        if (edge.first != vertex && edge.second != vertex)
        {
            edges.push_back(edge);
        }
    }


    return edges;
}

int main()
{
    vector<vector<int>> data = Data::read();
    vector<vector<int>> vertices = sortVertices(data);

    Data::print(data);
    Data::print(vertices);

    auto joinTree = getJoinTree(vertices);
    auto splitTree = getSplitTree(vertices);

    Data::printEdges(joinTree);
    Data::printEdges(splitTree);

    auto leaves = getLeaves(joinTree, splitTree, data.size() * data[0].size());

    //cout << "Leaves" << endl << endl;
    //for (auto l : leaves)
    //{
        //cout << l << " ";
    //}
    //
    vector<pair<int, int>> contourTree;

    while (leaves.size() > 1)
    {
        // Pop element
        auto i = *leaves.begin();
        leaves.erase(leaves.begin());
        cout << "---------------------------------------" << endl;
        cout << "Popped    : " << i << endl;
        cout << "Up Deg    : " << getDegree(joinTree, i) << endl;
        cout << "Down Deg  : " << getDegree(splitTree, i) << endl;

        cout << "Queue     : ";
        for(auto i: leaves)
        {
            cout << i << " ";
        }

        cout << endl;

        int j;
        if (0 == getDegree(joinTree, i))
        {
            cout << "UP   LEAF" << endl;
            j = *(getNeighbours(joinTree, i).begin());
        }
        else
        {
            cout << "DOWN LEAF" << endl;
            j = *(getNeighbours(splitTree, i).begin());
        }

        cout << "Neighbour : " << j << endl << endl;
        contourTree.push_back(make_pair(i, j));


        // Remove vertex from trees
        joinTree = removeVertex(joinTree, i);
        splitTree = removeVertex(splitTree, i);

        cout << "\n\nJoint Tree : ";
        Data::printEdges(joinTree);
        cout << "\n\nSplit Tree : ";
        Data::printEdges(splitTree);
        cout << "\n\nContour Tree : ";
        Data::printEdges(contourTree);

        // Add to queue if it is not in there
        leaves.push_back(j);
    }

    // cout << endl << "Here : " << getDegree(joinTree, 15);
    // cout << endl << "Here : " << getNeighbours(joinTree, 15)[2];

    return 0;
}

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

vector<pair<int, int>> getJoinTree(const vector<vector<int>> &vertices)
{
    DisjointSet ds;
    vector<int> lowestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        lowestVertex.push_back(i);
    }

    vector<vector<int>> joinTree(vertices.size(), std::vector<int>(vertices[0].size(), -1));
    vector<pair<int, int>> joinEdges;

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

            // Add to join tree
            // auto b = findCurrent(lowestVertex[ds.find(j)], vertices);
            // joinTree[b.first][b.second] = i;
            joinEdges.push_back(make_pair(i, lowestVertex[ds.find(j)]));

            // Lowest vertex in the whole component is now i
            lowestVertex[ds.find(j)] = i;
        }
    }

    // return joinTree;
    return joinEdges;
}

vector<pair<int, int>> getSplitTree(const vector<vector<int>> &vertices)
{
    DisjointSet ds;
    vector<int> highestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        highestVertex.push_back(i);
    }

    vector<vector<int>> splitTree(vertices.size(), std::vector<int>(vertices[0].size(), -1));
    vector<pair<int, int>> splitEdges;

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

            // Add to split tree
            // auto b = findCurrent(highestVertex[ds.find(j)], vertices);
            // splitTree[b.first][b.second] = i;
            splitEdges.push_back(make_pair(i, highestVertex[ds.find(j)]));

            // Highest vertex in the whole component is now i
            highestVertex[ds.find(j)] = i;
        }
    }

    return splitEdges;
}
