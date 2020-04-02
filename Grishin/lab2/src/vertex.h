#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

struct Edge;

struct Vertex{
    friend class Graph;

    char name;
    std::vector<Edge*> neighbours;

    ~Vertex();

    void addNeighbour(Vertex *other, float w);

private:
    Vertex(char name);
};

#endif
