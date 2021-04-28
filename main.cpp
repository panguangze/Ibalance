#include <iostream>
#include <cstring>

#include "Graph.hpp"
#include "Balance.hpp"
#include "Exceptions.hpp"

using namespace std;

int main(int argc, char * argv[]) {
    if (strcmp(argv[1], "check") == 0) {
        const char *lhRawFn = argv[2];
        const char *lpFn = argv[3];
        Graph *g = new Graph(lhRawFn);
        g->calculateHapDepth();
        Balance *lgm = new Balance(g);
        g->calculateCopyNum();
        try {
            lgm->connectSourceSink();
        } catch (DuplicateJunctionException &e) {
            cout << e.what() << endl;
        }
        g->calculateCopyNum();
        lgm->balancerILP(lpFn);
        cout << "Done" << endl;
    }
}
