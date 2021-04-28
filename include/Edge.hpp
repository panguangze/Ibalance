#ifndef _EDGE_H_
#define _EDGE_H_

#include "Vertex.hpp"
#include "Junction.hpp"

using namespace std;

class Vertex;
class Weight;
class Junction;

class Edge {
    protected:
        Weight * mWeight;
        Vertex * mSource;
        Vertex * mTarget;
        Junction * mJunction;

    public:
        Edge(Vertex * aSource, Vertex * aTarget, Weight * aWeight, double aCredibility);
        ~Edge();
        string getInfo();
        Vertex * getSource();
        Vertex * getTarget();
        Junction * getJunction();
        void setJunction(Junction * aJunction);
};
#endif
