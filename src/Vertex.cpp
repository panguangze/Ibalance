#include <iostream>

#include "Vertex.hpp"

using namespace std;

Vertex::Vertex(int aId, char aDir, Weight * aWeight, double aCredibility) {
    mId = aId;
    mDir = aDir;
    mWeight = aWeight;

    mIsOrphan = false;
    mHasCheckedOrphan = false;
    mShortestPrevEdge = NULL;
    mSegment = NULL;

    mEdgesAsSource = new vector<Edge *>();
    mEdgesAsTarget = new vector<Edge *>();
}

char Vertex::getDir() { return mDir; }
string Vertex::getInfo() { return to_string(mId) + mDir; }
bool Vertex::isOrphan(bool aIsOriginal) {

    return (!this->hasEdgeAsSource()) && (!this->hasEdgeAsTarget());
}
void Vertex::checkOrphan() {
        mIsOrphan = this->isOrphan(false);
}

bool Vertex::hasEdgeAsSource() { return mEdgesAsSource->size() > 0; }
bool Vertex::hasEdgeAsTarget() { return mEdgesAsTarget->size() > 0; }
vector<Edge *> * Vertex::getEdgesAsSource() { return mEdgesAsSource; }
vector<Edge *> * Vertex::getEdgesAsTarget() { return mEdgesAsTarget; }

Segment * Vertex::getSegment() { return mSegment; }
void Vertex::setSegment(Segment * aSeg) { mSegment = aSeg; }

void Vertex::insertEdgeAsSource(Edge * aEdge) { mEdgesAsSource->push_back(aEdge); }
void Vertex::insertEdgeAsTarget(Edge * aEdge) { mEdgesAsTarget->push_back(aEdge); }
