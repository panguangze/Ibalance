#include <iostream>

#include "Junction.hpp"

using namespace std;

Junction::Junction(Segment * aSource, Segment * aTarget, char aSourceDir, char aTargetDir, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction) {
    mSourceDir = aSourceDir;
    mTargetDir = aTargetDir;

    mCredibility = aCredibility;
    
    mIsInferred = aInferred;

    mWeight = new Weight(aCoverage);
    mWeight->setCopyNum(aCopy);
    aIsSourceSinkJunction ? mWeight->setInferred() : mWeight->resetInferred();

    mSource = aSource;
    mTarget = aTarget;
    
    if (aSourceDir == '+' && aTargetDir == '+') {
        mEdgeA = new Edge(mSource->getPositiveVertex(), mTarget->getPositiveVertex(), mWeight, mCredibility);
        mEdgeB = new Edge(mTarget->getNegativeVertex(), mSource->getNegativeVertex(), mWeight, mCredibility);
    } else if (aSourceDir == '-' && aTargetDir == '-') {
        mEdgeA = new Edge(mSource->getNegativeVertex(), mTarget->getNegativeVertex(), mWeight, mCredibility);
        mEdgeB = new Edge(mTarget->getPositiveVertex(), mSource->getPositiveVertex(), mWeight, mCredibility);
    } else if (aSourceDir == '+' && aTargetDir == '-') {
        mEdgeA = new Edge(mSource->getPositiveVertex(), mTarget->getNegativeVertex(), mWeight, mCredibility);
        mEdgeB = new Edge(mTarget->getPositiveVertex(), mSource->getNegativeVertex(), mWeight, mCredibility);
    } else if (aSourceDir == '-' && aTargetDir == '+') {
        mEdgeA = new Edge(mSource->getNegativeVertex(), mTarget->getPositiveVertex(), mWeight, mCredibility);
        mEdgeB = new Edge(mTarget->getNegativeVertex(), mSource->getPositiveVertex(), mWeight, mCredibility);
    }

    mEdgeA->setJunction(this);
    mEdgeB->setJunction(this);
}

Junction::~Junction() {
    delete mEdgeA;
    delete mEdgeB;
    delete mWeight;
}

vector<string> Junction::getInfo() {
    vector<string> info;
    info.push_back(mEdgeA->getInfo());
    info.push_back(mEdgeB->getInfo());
    return info;
}


bool Junction::isInferred() { return mIsInferred; }

Weight * Junction::getWeight() { return mWeight; }

void Junction::insertEdgesToVertices() {
    if (mSourceDir == '+' && mTargetDir == '+') {
        mSource->getPositiveVertex()->insertEdgeAsSource(mEdgeA);
        mTarget->getPositiveVertex()->insertEdgeAsTarget(mEdgeA);
        mSource->getNegativeVertex()->insertEdgeAsTarget(mEdgeB);
        mTarget->getNegativeVertex()->insertEdgeAsSource(mEdgeB);
    } else if (mSourceDir == '-' && mTargetDir == '-') {
        mSource->getNegativeVertex()->insertEdgeAsSource(mEdgeA);
        mTarget->getNegativeVertex()->insertEdgeAsTarget(mEdgeA);
        mSource->getPositiveVertex()->insertEdgeAsTarget(mEdgeB);
        mTarget->getPositiveVertex()->insertEdgeAsSource(mEdgeB);
    } else if (mSourceDir == '+' && mTargetDir == '-') {
        mSource->getPositiveVertex()->insertEdgeAsSource(mEdgeA);
        mTarget->getNegativeVertex()->insertEdgeAsTarget(mEdgeA);
        mSource->getNegativeVertex()->insertEdgeAsTarget(mEdgeB);
        mTarget->getPositiveVertex()->insertEdgeAsSource(mEdgeB);
    } else if (mSourceDir == '-' && mTargetDir == '+') {
        mSource->getNegativeVertex()->insertEdgeAsSource(mEdgeA);
        mTarget->getPositiveVertex()->insertEdgeAsTarget(mEdgeA);
        mSource->getPositiveVertex()->insertEdgeAsTarget(mEdgeB);
        mTarget->getNegativeVertex()->insertEdgeAsSource(mEdgeB);
    }
}
