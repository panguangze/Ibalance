#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <unistd.h>
#include <deque>

#include "Balance.hpp"
#include "Exceptions.hpp"
#include "coin/CbcModel.hpp"
#include "coin/OsiClpSolverInterface.hpp"

using namespace std;

Balance::Balance(Graph * aGraph) {
    mGraph = aGraph;
}

Balance::~Balance() {;}

vector<double> Balance::scaleILPCoef(vector<double> aCovs) {
    double mean = accumulate(aCovs.begin(), aCovs.end(), 0.0) / aCovs.size();
    vector<double> diff(aCovs.size());
    transform(aCovs.begin(), aCovs.end(), diff.begin(),
              [mean](double v) {return v - mean + 1;});
    vector<double> diff_s(diff.size());
    transform(diff.begin(), diff.end(), diff_s.begin(),
              [](double v) {return pow(v, 2);});
    double stdev = sqrt(accumulate(diff_s.begin(), diff_s.end(), 0.0) / diff_s.size());
    vector<double> scaled_cov(aCovs.size());
    transform(diff.begin(), diff.end(), scaled_cov.begin(),
              [stdev](double v) {return abs(v / stdev);});
    return scaled_cov;
}

int Balance::balancerILP(const char * lpFn) {
    OsiClpSolverInterface * si = new OsiClpSolverInterface();
    vector<Segment *> * segs = mGraph->getSegments();
    vector<Junction *> * juncs = mGraph->getJunctions();

    int numSegsJuncs = segs->size() + juncs->size();
    int numEpsilons = numSegsJuncs + 2;    // e(t(seg), c(seg)) 
                                           // e(t(junc), c(junc)) 
                                           // e(source, 2) 

    int numVariables = numSegsJuncs + juncs->size() + numEpsilons;
    int numConstrains = segs->size() * 4 + 4 * juncs->size() + 2;  // "2" for source & sink      ------  t(seg)+c(seg), t(seg)-c(seg), in=t(seg), out=t(seg, t(junc)+c(junc), t(junc)-c(junc), e, source, sink

    double * objective = new double[numVariables];
    double * variableLowerBound = new double[numVariables];
    double * variableUpperBound = new double[numVariables];
    double * constrainLowerBound = new double[numConstrains];
    double * constrainUpperBound = new double[numConstrains];
    cout << "Declare done" << endl;

    CoinPackedMatrix * matrix = new CoinPackedMatrix(false, 0, 0);

    double hap_cov = mGraph->getHaploidDepth();
    int maxCopy = 999999;
    double max_cov = -1;
    double max_cp = -1;
    double min_cov = 999999;
    vector<double> covs;
    // constrains for segments
    for (int i = 0; i < segs->size(); i++) {
        covs.push_back((*segs)[i]->getWeight()->getCoverage());
        if ((*segs)[i]->getWeight()->getCoverage() > max_cov) {
            max_cov = (*segs)[i]->getWeight()->getCoverage();
        }
        if ((*segs)[i]->getWeight()->getCopyNum() > max_cp) {
            max_cp = (*segs)[i]->getWeight()->getCopyNum();
        }
        if ((*segs)[i]->getWeight()->getCoverage() < min_cov) {
            min_cov = (*segs)[i]->getWeight()->getCoverage();
        }
        // t_i + e_i_1 >= c_i
        CoinPackedVector constrain1;
        constrain1.insert(i, hap_cov);  // t_i
        constrain1.insert(numSegsJuncs + juncs->size() + i, 1);  // e_i_1
        constrainLowerBound[4 * i] = (*segs)[i]->getWeight()->getCoverage();
        constrainUpperBound[4 * i] = si->getInfinity();
        matrix->appendRow(constrain1);

        // t_i - e_i_1 <= c_i
        CoinPackedVector constrain2;
        constrain2.insert(i, hap_cov);
        constrain2.insert(numSegsJuncs + juncs->size() + i, -1);
        constrainLowerBound[4 * i + 1] = -1 * si->getInfinity();
        constrainUpperBound[4 * i + 1] = (*segs)[i]->getWeight()->getCoverage();
        matrix->appendRow(constrain2);

        // // t_i + 0.4*c_i*e_i_2 >= c_i
        vector<int> visitedIdx;
        vector<int> visitedCount;
        vector<int>::iterator found;
        CoinPackedVector constrain7;
        constrain7.insert(i, 1);
        for (Edge * e : *((*segs)[i]->getPositiveVertex()->getEdgesAsTarget())) {
            // cout << e->getInfo() << endl;
            int juncIndex = mGraph->getJunctionIndexByEdge(e);
            found = find(visitedIdx.begin(), visitedIdx.end(), juncIndex);
            if (found == visitedIdx.end()) {
                // constrain7.insert(segs->size() + juncIndex, -1);
                visitedIdx.push_back(juncIndex);
                visitedCount.push_back(1);
            } else {
                if (e->getTarget() != e->getSource()) {
                    visitedCount[found - visitedIdx.begin()]++;
                }
            }
        }
        for (int j = 0; j < visitedIdx.size(); j++) {
            cout << visitedIdx[j] << ":" << visitedCount[j] << " ";
            constrain7.insert(segs->size() + visitedIdx[j], -visitedCount[j]);
        }
        cout << endl;
        constrainLowerBound[4 * i + 2] = 0;
        constrainUpperBound[4 * i + 2] = 0;
        matrix->appendRow(constrain7);
        visitedIdx.clear();
        visitedCount.clear();

        CoinPackedVector constrain8;
        constrain8.insert(i, 1);
        for (Edge * e : *((*segs)[i]->getPositiveVertex()->getEdgesAsSource())) {
            int juncIndex = mGraph->getJunctionIndexByEdge(e);
            found = find(visitedIdx.begin(), visitedIdx.end(), juncIndex);
            if (found == visitedIdx.end()) {
                // constrain8.insert(segs->size() + juncIndex, -1);
                visitedIdx.push_back(juncIndex);
                visitedCount.push_back(1);
            } else {
                if (e->getTarget() != e->getSource()) {
                    visitedCount[found - visitedIdx.begin()]++;
                }
            }
        }
        for (int j = 0; j < visitedIdx.size(); j++) {
            cout << visitedIdx[j] << ":" << visitedCount[j] << " ";
            constrain8.insert(segs->size() + visitedIdx[j], -visitedCount[j]);
        }
        cout << endl;
        constrainLowerBound[4 * i + 3] = 0;
        constrainUpperBound[4 * i + 3] = 0;
        matrix->appendRow(constrain8);
        visitedIdx.clear();
        visitedCount.clear();
    }
    cout << "Segment done" << endl;

    // constrains for junctions
    for (int i = 0; i < juncs->size(); i++) {
        covs.push_back((*juncs)[i]->getWeight()->getCoverage());
        if ((*juncs)[i]->getWeight()->getCoverage() > max_cov) {
            max_cov = (*juncs)[i]->getWeight()->getCoverage();
        }
        if ((*juncs)[i]->getWeight()->getCopyNum() > max_cp) {
            max_cp = (*juncs)[i]->getWeight()->getCopyNum();
        }
        if ((*juncs)[i]->getWeight()->getCoverage() < min_cov) {
            min_cov = (*juncs)[i]->getWeight()->getCoverage();
        }
        // t_i - c_ix_i + e_i_1 >= 0
        double cov = (*juncs)[i]->getWeight()->getCoverage() + 0.05;
        CoinPackedVector constrain1;
        constrain1.insert(segs->size() + i, hap_cov);
        constrain1.insert(numSegsJuncs + i, -cov);
        constrain1.insert(numSegsJuncs + juncs->size() + segs->size() + i, 1);
        constrainLowerBound[4 * segs->size() + 4 * i] = 0;
        constrainUpperBound[4 * segs->size() + 4 * i] = si->getInfinity();
        matrix->appendRow(constrain1);

        // t_i - c_ix_i - e_i_1 <= 0
        CoinPackedVector constrain2;
        constrain2.insert(segs->size() + i, hap_cov);
        constrain2.insert(numSegsJuncs + i, -cov);
        constrain2.insert(numSegsJuncs + juncs->size() + segs->size() + i, -1);
        constrainLowerBound[4 * segs->size() + 4 * i + 1] = -1 * si->getInfinity();
        constrainUpperBound[4 * segs->size() + 4 * i + 1] = 0;
        matrix->appendRow(constrain2);

        // t_i - x_iM <= 0
        CoinPackedVector constrain3;
        constrain3.insert(segs->size() + i, 1);
        constrain3.insert(numSegsJuncs + i, -maxCopy);
        constrainLowerBound[4 * segs->size() + 4 * i + 2] = -1 * si->getInfinity();
        constrainUpperBound[4 * segs->size() + 4 * i + 2] = 0;
        matrix->appendRow(constrain3);

        // t_i - x_i >= 0
        CoinPackedVector constrain4;
        constrain4.insert(segs->size() + i, 1);
        constrain4.insert(numSegsJuncs + i, -1);
        constrainLowerBound[4 * segs->size() + 4 * i + 3] = 0;
        constrainUpperBound[4 * segs->size() + 4 * i + 3] = si->getInfinity();
        matrix->appendRow(constrain4);

    }
    cout << "Junc done" << endl;

    // constrains for source and sink
    CoinPackedVector constrainSource;
    constrainSource.insert(mGraph->getSource()->getId() - 1, 1);
    constrainSource.insert(numVariables - 2, -1);
    // cout << mGraph->getExpectedPloidy() << endl;
    constrainLowerBound[numConstrains - 2] = mGraph->getExpectedPloidy();
    constrainUpperBound[numConstrains - 2] = mGraph->getExpectedPloidy();
    matrix->appendRow(constrainSource);

    CoinPackedVector constrainSink;
    constrainSink.insert(mGraph->getSink()->getId() - 1, 1);
    constrainSink.insert(numVariables - 1, -1);
    constrainLowerBound[numConstrains - 1] = mGraph->getExpectedPloidy();
    constrainUpperBound[numConstrains - 1] = mGraph->getExpectedPloidy();
    matrix->appendRow(constrainSink);
    cout << "Source sink done" << endl;

    vector<double> coefs = scaleILPCoef(covs);
    double max_coef = *max_element(coefs.begin(), coefs.end());
    double min_coef = *min_element(coefs.begin(), coefs.end());
    min_coef = (min_coef > 0) ? min_coef : 0.1;
    max_cov += 1000;
    for (int i = 0; i < numVariables; i++) {
        if (i >= numSegsJuncs) {
            if (i < numVariables - 2) {
                // double cred;
                if (i < numSegsJuncs + juncs->size()) {
                    if ((*juncs)[i - numSegsJuncs]->isInferred()) {
                        objective[i] = max_coef;

                    } else {
                        objective[i] = 0;
                    }
                } else if (i < numSegsJuncs + juncs->size() + segs->size()) {
                    objective[i] = coefs[i - numSegsJuncs - juncs->size()];
                } else {

                    objective[i] = min_coef;
                }
            } else {
                objective[i] = max_coef;
            }
        } else {
            objective[i] = 0;
        }
    }
    cout << "Objective done" << endl;
    // lower & upper bounds for segments and corresponding epsilons variables
    for (int i = 0; i < segs->size(); i++) {
        if ((*segs)[i]->hasLowerBoundLimit()) {  // inferred junctions have lower bound 1
            variableLowerBound[i] = 1;
        } else {
            variableLowerBound[i] = 0;
        }
        variableUpperBound[i] = si->getInfinity();

        // for epsilons
        variableLowerBound[numSegsJuncs + juncs->size() + i] = 0;
        variableUpperBound[numSegsJuncs + juncs->size() + i] = si->getInfinity();

    }
    cout << "LU seg done" << endl;

    // lower & upper bounds for junctions and corresponding epsilons variables
    for (int i = 0; i < juncs->size(); i++) {

        variableLowerBound[segs->size() + i] = 0;
        variableUpperBound[segs->size() + i] = si->getInfinity();

        // for x
        if ((*juncs)[i]->isInferred()) {
            variableLowerBound[numSegsJuncs + i] = 0;
            cout << "infer info - 0: " << i << " " << segs->size() + i << " " << numSegsJuncs + i << " " << (*juncs)[i]->getInfo()[0] << " " << endl;
        } else {
            variableLowerBound[numSegsJuncs + i] = 1;
            cout << "infer info - 1: " << i << " " << segs->size() + i << " " << numSegsJuncs + i << " " << (*juncs)[i]->getInfo()[0] << " " << endl;
        }
        variableUpperBound[numSegsJuncs + i] = 1;

        // for epsilons
        variableLowerBound[numSegsJuncs + juncs->size() + segs->size() + i] = 0;
        variableUpperBound[numSegsJuncs + juncs->size() + segs->size() + i] = si->getInfinity();
    }
    cout << "LU junc done" << endl;
    variableLowerBound[numVariables - 2] = 0;
    variableUpperBound[numVariables - 2] = si->getInfinity();
    variableLowerBound[numVariables - 1] = 0;
    variableUpperBound[numVariables - 1] = si->getInfinity();
    si->loadProblem(*matrix, variableLowerBound, variableUpperBound, objective, constrainLowerBound, constrainUpperBound);
    for (int i = 0; i < numSegsJuncs + juncs->size(); i++) {
        si->setInteger(i);
    }
    si->writeMps(lpFn);
    si->writeLp(lpFn);

    CbcModel model(*si);

    return 0;
}

void Balance::connectSourceSink() {
    mGraph->addJunction(mGraph->getSink()->getPositiveVertex(), mGraph->getSource()->getPositiveVertex(), (mGraph->getSource()->getWeight()->getCoverage() + mGraph->getSink()->getWeight()->getCoverage()) / 2, 1.0, -1, true, false, true);
}