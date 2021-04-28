#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>

#include "Segment.hpp"
#include "Junction.hpp"

using namespace std;

class Segment;
class Junction;

class Graph {
    protected:
        int mExpectedPloidy;     // expected ploidy

        double mHaploidDepth;
        double mHaploidDepthJunc;
        double mPurity;   // tumor purity
        double mAvgPloidy;   // the ploidy of whole sequencing region, including tumor if possible
        double mAvgTumorPloidy;   // the ploidy of tumor
        double mAvgCoverage;
        double mAvgCoverageRaw;
        double mAvgCoverageJunc;

        Segment * mSource;
        Segment * mSink;

        vector<Segment *> * mSegments;
        vector<Junction *> * mJunctions;

    public:
        Graph();
        Graph(const char * aFilename);
        ~Graph();

        int getJunctionIndexByEdge(Edge * aEdge);

        int getExpectedPloidy();

        double getHaploidDepth();
        Segment * getSource();
        Segment * getSink();
        
        vector<Segment *> * getSegments();
        vector<Junction *> * getJunctions();

        bool doesJunctionExist(Junction * aJunction);

        void readGraph(const char * aFilename);
        void checkOrphan();
        void calculateHapDepth();
        void calculateCopyNum();

        Segment * getSegmentById(int aSegId);

        Segment * addSegment(int aId, string aChrom, int aStart, int aEnd, double aCoverage, double aCredibility, double aCopy);
        Junction * addJunction(Vertex * aSource, Vertex * aTarget, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction);
        Junction * addJunction(int aSourceId, char aSourceDir, int aTargetId, char aTargetDir, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction);
};

#endif
