#ifndef _COVERAGE_H_
#define _COVERAGE_H_

using namespace std;

class Weight {
    protected:
        double mCoverage;           // coverage as adjusted by LP
        double mCopyNum;             // the copy number, coverage / average coverage of one copy
        double mCopyNumOriginal;
        double mCopyNumBackup;
        bool mIsInferred;


    public:
        Weight(double aCoverage);
        ~Weight();
        double getCoverage();
        double getCopyNum();
        void setCopyNum(double aCopyNum);
        void setInferred();
        void resetInferred();
};

#endif
