#include <iostream>

#include "Segment.hpp"
#include "Weight.hpp"

using namespace std;

Segment::Segment(int aId, string aChrom, int aStart, int aEnd, double aCoverage, double aCredibility, double aCopy) {
    mId = aId;
    mChrom = aChrom;
    mStart = aStart;
    mEnd = aEnd;

    mHasLowerBoundLimit = true;
    mIsOrphan = false;

    mWeight = new Weight(aCoverage);
    mWeight->setCopyNum(aCopy);
    mPositiveVertex = new Vertex(aId, '+', mWeight, aCredibility);
    mNegativeVertex = new Vertex(aId, '-', mWeight, aCredibility);
    mPositiveVertex->setSegment(this);
    mNegativeVertex->setSegment(this);
}
Segment::~Segment() {;}

int Segment::getId() { return mId; }


bool Segment::hasLowerBoundLimit() { return mHasLowerBoundLimit; }
void Segment::checkOrphan() {
        mPositiveVertex->checkOrphan();
        mNegativeVertex->checkOrphan();
        mIsOrphan = mPositiveVertex->isOrphan() && mNegativeVertex->isOrphan();
}
Weight * Segment::getWeight() { return mWeight; }
Vertex * Segment::getPositiveVertex() { return mPositiveVertex; }
Vertex * Segment::getNegativeVertex() { return mNegativeVertex; }
