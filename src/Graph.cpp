#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <queue>
#include <iomanip>

#include "Graph.hpp"
#include "Exceptions.hpp"

using namespace std;

Graph::Graph() {
    mPurity = -1;
    mAvgPloidy = -1;
    mAvgTumorPloidy = -1;

    mSegments = new vector<Segment *>();
    mJunctions = new vector<Junction *>();
}
Graph::Graph(const char * aFilename) {
    mPurity = -1;
    mAvgPloidy = -1;
    mAvgTumorPloidy = -1;

    mSource = NULL;
    mSink = NULL;

    mSegments = new vector<Segment *>();
    mJunctions = new vector<Junction *>();

    this->readGraph(aFilename);
}
Graph::~Graph() {;}

int Graph::getJunctionIndexByEdge(Edge * aEdge) {
    int idx = 0;
    while (true) {
        if ((*mJunctions)[idx] == aEdge->getJunction())
            return idx;
        idx++;
    }
}

int Graph::getExpectedPloidy() { return mExpectedPloidy; }

double Graph::getHaploidDepth() { return mHaploidDepth; }

Segment * Graph::getSource() { return mSource; }
Segment * Graph::getSink() { return mSink; }

vector<Segment *> * Graph::getSegments() { return mSegments; }
vector<Junction *> * Graph::getJunctions() { return mJunctions; }

// functionality
void Graph::readGraph(const char * aFilename) {
    ifstream graphFile(aFilename);
    if (!graphFile) {
        cerr << "Cannot open file " << aFilename << endl;
        exit(1);
    }
    
    cout << "Reading graph..." << endl;
    char line[8192];
    char * token;
    int sourceId, sinkId;
    while (!graphFile.eof()) {
        graphFile.getline(line, 8192);
        
        char * line_p = line;
        while (*line_p != '\0') {
            if (*line_p != '\t' && *line_p != ' ') {
                break;
            }
            line_p++;
        }
        if (*line_p == '#') {
            continue;
        }

        token = strtok(line, " \t");
        if (token == NULL) {
            continue;
        }
        if (strcmp(token, "SAMPLE") == 0) {
        } else if (strcmp(token, "AVG_SEG_DP") == 0) {
            mAvgCoverage = atof(strtok(NULL, " "));
            mAvgCoverageRaw = mAvgCoverage;
        } else if (strcmp(token, "AVG_JUNC_DP") == 0) {
            mAvgCoverageJunc = atof(strtok(NULL, " "));
        } else if (strcmp(token, "PURITY") == 0) {
            mPurity = atof(strtok(NULL, " "));
        } else if (strcmp(token, "AVG_TUMOR_PLOIDY") == 0) {
            mAvgTumorPloidy = atof(strtok(NULL, " "));
        } else if (strcmp(token, "AVG_PLOIDY") == 0) {
            mAvgPloidy = atof(strtok(NULL, " "));
        } else if (strcmp(token, "PLOIDY") == 0) {
            token = strtok(NULL, " ");
            mExpectedPloidy = atoi(strtok(token, "m"));
        } else if (strcmp(token, "SOURCE") == 0) {
            token = strtok(NULL, " ");
            strtok(token, ":");
            sourceId = atoi(strtok(NULL, ":"));
        } else if (strcmp(token, "SINK") == 0) {
            token = strtok(NULL, " ");
            strtok(token, ":");
            sinkId = atoi(strtok(NULL, ":"));
        } else if (strcmp(token, "SEG") == 0) {
            char * node = strtok(NULL, " ");
            double segCoverage = max(atof(strtok(NULL, " ")), 0.0);
            double segCopy = atof(strtok(NULL, " "));

            strtok(node, ":");
            int segId = atoi(strtok(NULL, ":"));
            string chrom = strtok(NULL, ":");
            int start = atoi(strtok(NULL, ":"));
            int end = atoi(strtok(NULL, ":"));
            double segCred = 1.0;
            
            this->addSegment(segId, chrom, start, end, segCoverage, segCred, segCopy);
        } else if (strcmp(token, "JUNC") == 0) {
            char * sourceNode = strtok(NULL, " ");
            char * targetNode = strtok(NULL, " ");
            double junCoverage = atof(strtok(NULL, " "));
            double junCopy = atof(strtok(NULL, " "));
            token = strtok(NULL, " ");
            // cout << token << endl;
            bool isInferred = (token[0] == 'I') ? true : false;
            token = strtok(NULL, " ");
            // cout << token << endl;
            bool isBounded = (token[0] == 'B') ? true : false;
            // cout << (isBounded ? "yes" : "no") << endl;
            if (junCoverage <= 0 && junCopy <= 0) continue;
            
            strtok(sourceNode, ":");
            int sourceId = atoi(strtok(NULL, ":"));
            char sourceDir = strtok(NULL, ":")[0];

            strtok(targetNode, ":");
            int targetId = atoi(strtok(NULL, ":"));
            char targetDir = strtok(NULL, ":")[0];

            double junCred = 1.0;
            
            this->addJunction(sourceId, sourceDir, targetId, targetDir, junCoverage, junCred, junCopy, isInferred, isBounded, false);
        }
    }
    mSource = this->getSegmentById(sourceId);
    mSink = this->getSegmentById(sinkId);
}

void Graph::checkOrphan() {
    for (Segment * seg : *mSegments) {
        seg->checkOrphan();
    }
}

void Graph::calculateHapDepth() {
    
    if (mAvgPloidy < 0) {
        if (mAvgTumorPloidy < 0) {
            cerr << "input error: there is no ploidy information provided. There must be at least one of \"AVG_PLOIDY\" and \"AVG_TUMOR_PLOIDY\"." << endl;
            exit(1);
        } else {
            if (mPurity < 0) {
                cerr << "input error: no purity information provided." << endl;
                exit(1);
            } else {
                mAvgPloidy = mPurity * mAvgTumorPloidy + (1 - mPurity) * 2;
            }
        }
    } else {
        if (mAvgTumorPloidy >= 0) {

            if (mPurity < 0) {
            } else {
                double avgPloidy = mPurity * mAvgTumorPloidy + (1 - mPurity) * 2;
                if (abs(mAvgPloidy - avgPloidy) <= 0.1) {
                } else {
                    mAvgPloidy = avgPloidy;
                }
            }
        } else {
        }
    }

    mHaploidDepth = mAvgCoverageRaw / mAvgPloidy;
    mHaploidDepthJunc = mHaploidDepth;
    mAvgCoverage = mAvgPloidy * mHaploidDepth;
    mAvgCoverageJunc = mAvgPloidy * mHaploidDepthJunc;

}

void Graph::calculateCopyNum() {
    for (Segment * seg : *mSegments) {
        double segCopy = (seg->getWeight()->getCoverage() - (1 - mPurity) * mAvgCoverage) / (mPurity * mHaploidDepth);
        seg->getWeight()->setCopyNum(max(segCopy, 0.0));
    }

    for (Junction * junc : *mJunctions) {
        double juncCopy = junc->getWeight()->getCoverage() / mHaploidDepthJunc;
        junc->getWeight()->setCopyNum(max(juncCopy, 0.0));
    }
}

bool Graph::doesJunctionExist(Junction * aJunction) {
    vector<string> aJuncInfo = aJunction->getInfo();
    for (Junction * junc : *mJunctions) {
        vector<string> juncInfo = junc->getInfo();
        if ((juncInfo[0] == aJuncInfo[0] && juncInfo[1] == aJuncInfo[1]) || (juncInfo[0] == aJuncInfo[1] && juncInfo[1] == aJuncInfo[0])) {
            return true;
        }
    }
    return false;
}
Segment * Graph::getSegmentById(int aSegId) {
    for (Segment * seg : *mSegments) {
        if (seg->getId() == aSegId) {
            return seg;
        }
    }
    throw SegmentDoesNotExistException(aSegId);
}

Segment * Graph::addSegment(int aId, string aChrom, int aStart, int aEnd, double aCoverage, double aCredibility, double aCopy) {
    Segment * seg = new Segment(aId, aChrom, aStart, aEnd, aCoverage, aCredibility, aCopy);
    mSegments->push_back(seg);
    return seg;
}

Junction * Graph::addJunction(Vertex * aSource, Vertex * aTarget, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction) {
    Segment * sourceSeg = aSource->getSegment();
    Segment * targetSeg = aTarget->getSegment();
    if (!sourceSeg->hasLowerBoundLimit() || !targetSeg->hasLowerBoundLimit()) {
        return NULL;
    }

    Junction * junc = new Junction(sourceSeg, targetSeg, aSource->getDir(), aTarget->getDir(), aCoverage, aCredibility, aCopy, aInferred, aIsBounded, aIsSourceSinkJunction);
    if (this->doesJunctionExist(junc)) {
        throw DuplicateJunctionException(junc);
    }
    junc->insertEdgesToVertices();
    mJunctions->push_back(junc);
    return junc;
}
Junction * Graph::addJunction(int aSourceId, char aSourceDir, int aTargetId, char aTargetDir, double aCoverage, double aCredibility, double aCopy, bool aInferred, bool aIsBounded, bool aIsSourceSinkJunction) {
    Segment * sourceSeg = this->getSegmentById(aSourceId);
    Segment * targetSeg = this->getSegmentById(aTargetId);

    if (!sourceSeg->hasLowerBoundLimit() || !targetSeg->hasLowerBoundLimit()) {
        return NULL;
    }

    Junction * junc = new Junction(sourceSeg, targetSeg, aSourceDir, aTargetDir, aCoverage, aCredibility, aCopy, aInferred, aIsBounded, aIsSourceSinkJunction);
    if (this->doesJunctionExist(junc)) {
        return junc;
    }
    junc->insertEdgesToVertices();
    mJunctions->push_back(junc);
    return junc;
}
