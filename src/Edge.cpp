#include <iostream>

#include "Edge.hpp"

using namespace std;

Edge::Edge(Vertex * aSource, Vertex * aTarget, Weight * aWeight, double aCredibility) {

    mWeight = aWeight;
    mSource = aSource;
    mTarget = aTarget;
}

Edge::~Edge() {
    vector<Edge *>::iterator removedIter;
    for (vector<Edge *>::iterator i = mSource->getEdgesAsSource()->begin(); i != mSource->getEdgesAsSource()->end(); i++) {
        if (*i == this) {
            removedIter = i;
            break;
        }
    }
    mSource->getEdgesAsSource()->erase(removedIter);

    for (vector<Edge *>::iterator i = mTarget->getEdgesAsTarget()->begin(); i != mSource->getEdgesAsTarget()->end(); i++) {
        if (*i == this) {
            removedIter = i;
            break;
        }
    }
    mTarget->getEdgesAsTarget()->erase(removedIter);
}

string Edge::getInfo() {
    return mSource->getInfo() + "=>" + mTarget->getInfo();
}


Vertex * Edge::getSource() { return mSource; }
Vertex * Edge::getTarget() { return mTarget; }
Junction * Edge::getJunction() { return mJunction; }
void Edge::setJunction(Junction * aJunction) { mJunction = aJunction; }

