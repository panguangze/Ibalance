#ifndef _LOCAL_GENOMIC_MAP_H_
#define _LOCAL_GENOMIC_MAP_H_

#include "Graph.hpp"
class Graph;

class Balance {
    protected:
        Graph * mGraph;

    public:
        Balance(Graph * aGraph);
        ~Balance();
        vector<double> scaleILPCoef(vector<double> aCovs);
        int balancerILP(const char * lpFn);
        void connectSourceSink();

};

#endif
