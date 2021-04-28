#ifndef _JUNCTION_H_
#define _JUNCTION_H_

#include <iostream>

#include "Segment.hpp"
#include "Edge.hpp"
#include "Weight.hpp"

class Segment;
class Edge;

using namespace std;

class Junction {
    protected:
        char mSourceDir;
        char mTargetDir;
        double mCredibility;
        bool mIsInferred;

        Weight * mWeight;
        Segment * mSource;
        Segment * mTarget;

        Edge * mEdgeA;         //
        Edge * mEdgeB;

    public:
        Junction(Segment * aSource, Segment * aTarget, char aSourceDir, char aTargetDir, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction);
        ~Junction();
        vector<string> getInfo();
        bool isInferred();
        Weight * getWeight();

        void insertEdgesToVertices();
};

#endif
