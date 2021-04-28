#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "Vertex.hpp"
#include "Weight.hpp"

using namespace std;

class Vertex;
class Weight;

class Segment {
    protected:
        int mId;    // segment id
        string mChrom;
        int mStart;
        int mEnd;
        
        double mCredibility;

        bool mHasLowerBoundLimit;  // used in ILP processing, if true then lower bound is 1, otherwise 0
        bool mIsOrphan;            // record whether segment is orphan originally

        Weight * mWeight;
        Vertex * mPositiveVertex;
        Vertex * mNegativeVertex;
        
    public:
        // constructor and destructor
        Segment(int aId, string aChrom, int aStart, int aEnd, double aCoverage, double aCredibility, double aCopy);
        ~Segment();
        
        int getId();
        bool hasLowerBoundLimit();
        void checkOrphan();

        Weight * getWeight();
        Vertex * getPositiveVertex();
        Vertex * getNegativeVertex();

};
#endif
