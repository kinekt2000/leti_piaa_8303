#include "vertex.h"
#include "edge.h"

Vertex::~Vertex()
{
    for(auto edge: neighbours){
        delete edge;
    }
}


Vertex::Vertex(char name):
    name(name)
{}


void Vertex::addNeighbour(Vertex *other, float w)
{
    new Edge(this, other, w);
}
