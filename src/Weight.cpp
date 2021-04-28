#include "Weight.hpp"

Weight::Weight(double aCoverage) {
    mCoverage = aCoverage;
    mCopyNum = 0;
    mCopyNumOriginal = 0;
    mCopyNumBackup = 0;
    
    mIsInferred = false;
}

Weight::~Weight() {;}

double Weight::getCoverage() { return mCoverage; }
double Weight::getCopyNum() { return mCopyNum; }
void Weight::setCopyNum(double aCopyNum) { mCopyNum = mCopyNumOriginal = mCopyNumBackup = aCopyNum; }

void Weight::setInferred() { mIsInferred = true; }
void Weight::resetInferred() { mIsInferred = false; }
