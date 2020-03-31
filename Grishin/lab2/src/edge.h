#ifndef EDGE_H
#define EDGE_H


struct Vertex;

struct Edge{
    friend struct Vertex;

    float w;
    class Vertex* from;
    class Vertex* to;

    ~Edge();

private:
    Edge(Vertex *from, Vertex *to, float w);
};

#endif
