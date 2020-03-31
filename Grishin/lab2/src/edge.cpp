#include "edge.h"
#include "vertex.h" 

Edge::~Edge()
{
    for(auto it = from->neighbours.begin();
        it < from->neighbours.end(); it++)
    {
        if(*it == this){
            from->neighbours.erase(it);
            break;
        }
    }
}


Edge::Edge(Vertex *from, Vertex *to, float w):
    from(from), to(to), w(w)
{
    from->neighbours.push_back(this);
}
