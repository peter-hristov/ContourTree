#ifndef CONTOUR_TREE_PARALLEL
#define CONTOUR_TREE

#include <algorithm>
#include <assert.h>
#include <iostream>

#include "./data.hpp"
#include "./disjoint_set.hpp"

namespace ContourTree
{

void contractVertex(std::vector<std::vector<int>> &tree, int vertex)
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
            // If they are not the same and j is not already a neighbour.
            if (i != j && find(tree[i].begin(), tree[i].end(), j) == tree[i].end())
            {
                tree[i].push_back(j);
            }
        }
    }

    tree[vertex].clear();
}

int getUpDegree(const std::vector<std::vector<int>> &tree, int vertex)
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

int getDownDegree(const std::vector<std::vector<int>> &tree, int vertex)
{
    return tree[vertex].size() - getUpDegree(tree, vertex);
}

void removeAugmentation(std::vector<std::vector<int>> &contourTree)
{
    for (int i = 0; i < contourTree.size(); i++)
    {
        if (1 == getUpDegree(contourTree, i) && 1 == getDownDegree(contourTree, i))
        {
            contractVertex(contourTree, i);
        }
    }
}

bool isThere(std::vector<std::pair<int, int>> edges, std::pair<int, int> p)
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

std::vector<std::pair<int, int>> convertToEdges(const std::vector<std::vector<int>> &contourTree)
{
    std::vector<std::pair<int, int>> edges;

    for (int i = 0; i < contourTree.size(); i++)
    {
        for (auto j : contourTree[i])
        {
            if (!isThere(edges, std::make_pair(i, j)))
            {
                edges.push_back(std::make_pair(i, j));
            }
        }
    }

    return edges;
}

std::vector<std::vector<int>> getJoinTree(const std::vector<std::vector<int>> &vertices, const std::vector<std::pair<int, int>> &sortedVertices)
{
    DisjointSet ds;
    std::vector<int> lowestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        lowestVertex.push_back(i);
    }

    std::vector<std::vector<int>> joinTree(vertices.size() * vertices[0].size());
    // vector<vector<int>> joinTree(vertices.size(), vector<int>(vertices[0].size(), -1));

    for (int i = vertices.size() * vertices[0].size() - 1; i >= 0; i--)
    {
        // Get coordinates of the current vertex
        // pair<int, int> currentPosition = findCurrent(i, vertices);
        std::pair<int, int> currentPosition = sortedVertices[i];

        // double value = (double)(data[currentPosition.first][currentPosition.second] * 1.0);

        // cout << "(" << currentPosition.first << ", " << currentPosition.second << ") - " << value << endl;
        // printf("(%d, %d) - %f\n", currentPosition.first, currentPosition.second, value);

        // Get the neighbours of the current vertex
        auto n = Data::getAdjacent(currentPosition.first, currentPosition.second, vertices.size(), vertices[0].size());

        // cout << "Currently at : " << i << endl;

        // cout << endl;
        for (std::pair<int, int> a : n)
        {
            // Get value at vertex
            int j = vertices[a.first][a.second];

            // Check if it's lower or if they are in the same component already
            if (j < i || ds.find(i) == ds.find(j))
            {
                continue;
            }

            // Add edge to join tree
            // pair<int, int> pos = findCurrent(lowestVertex[ds.find(i)], vertices);
            // joinTree[pos.first][pos.second] = i;

            joinTree[i].push_back(lowestVertex[ds.find(j)]);
            joinTree[lowestVertex[ds.find(j)]].push_back(i);

            // Merge components
            ds.merge(i, j);
        }

        // Lowest vertex in the whole new component is now i
        lowestVertex[ds.find(i)] = i;
    }

    return joinTree;

    // joinTree[i].push_back(lowestVertex[ds.find(j)]);
    // joinTree[lowestVertex[ds.find(j)]].push_back(i);
    // return joinTree;
}

std::vector<std::vector<int>> getSplitTree(const std::vector<std::vector<int>> &vertices, const std::vector<std::pair<int, int>> &sortedVertices)
{
    DisjointSet ds;
    std::vector<int> highestVertex;

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        ds.add(i);
        highestVertex.push_back(i);
    }

    std::vector<std::vector<int>> splitTree(vertices.size() * vertices[0].size());
    // vector<vector<int>> splitTree(vertices.size(), vector<int>(vertices[0].size()));

    for (int i = 0; i < vertices.size() * vertices[0].size(); i++)
    {
        // Get coordinates of the current vertex
        // pair<int, int> currentPosition = findCurrent(i, vertices);
        std::pair<int, int> currentPosition = sortedVertices[i];

        highestVertex[i] = i;

        // Get the neighbours of the current vertex
        auto n = Data::getAdjacent(currentPosition.first, currentPosition.second, vertices.size(), vertices[0].size());

        for (std::pair<int, int> a : n)
        {
            int j = vertices[a.first][a.second];

            if (j > i || ds.find(i) == ds.find(j))
            {
                continue;
            }

            splitTree[i].push_back(highestVertex[ds.find(j)]);
            splitTree[highestVertex[ds.find(j)]].push_back(i);

            // pair<int, int> pos = findCurrent(highestVertex[ds.find(j)], vertices);
            // splitTree[pos.first][pos.second] = i;

            // Highest vertex in the whole component is now i

            ds.merge(i, j);
        }

        highestVertex[ds.find(i)] = i;
    }

    return splitTree;
}

std::vector<std::vector<int>> getContourTree(std::vector<std::vector<int>> joinTree, std::vector<std::vector<int>> splitTree, bool debug)
{
    std::vector<int> q;

    // Queue in all leaves
    for (int i = 0; i < joinTree.size(); i++)
    {
        if (1 == getUpDegree(joinTree, i) + getDownDegree(splitTree, i))
        {
            q.push_back(i);
        }
    }

    std::vector<std::vector<int>> contourTree(joinTree.size());

    while (!q.empty())
    {
        // Pop head
        int i = q[0];
        q.erase(q.begin());

        if (debug)
        {
            std::cout << "\n---------------------------" << std::endl;

            std::cout << "Queue       : ";
            for (auto a : q)
            {
                std::cout << a << " ";
            }
            std::cout << "\nPopped      : " << i << std::endl;
            std::cout << "Up          : " << getUpDegree(joinTree, i) << std::endl;
            std::cout << "Down        : " << getDownDegree(splitTree, i) << " ";
        }

        int j = -1;

        if (0 == getUpDegree(joinTree, i) && 1 == getDownDegree(splitTree, i))
        {
            assert(joinTree[i].size() == 1);

            if (debug)
            {
                std::cout << "+ Up   leaf" << std::endl;
            }
            j = joinTree[i][0];
        }
        else if (1 == getUpDegree(joinTree, i) && 0 == getDownDegree(splitTree, i))
        {
            assert(splitTree[i].size() == 1);

            if (debug)
            {
                std::cout << "- Down leaf" << std::endl;
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
            std::cout << "Neighbour   : " << j << std::endl;
        }

        // Add edge to controur tree
        contourTree[i].push_back(j);
        contourTree[j].push_back(i);

        // Contract used leaf
        contractVertex(joinTree, i);
        contractVertex(splitTree, i);

        if (debug)
        {
            std::cout << "\nJoin Tree :";
            Data::printTree(joinTree);
            std::cout << "\nSplit Tree :";
            Data::printTree(splitTree);
            std::cout << "\nContour Tree :";
            Data::printTree(contourTree);
        }

        if (1 == getUpDegree(joinTree, j) + getDownDegree(splitTree, j))
        {
            q.push_back(j);
        }
    }

    return contourTree;
}

} // Namespace ContourTree

#endif // CONTOUR_TREE
