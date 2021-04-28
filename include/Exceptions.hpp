#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include "Junction.hpp"
#include "Vertex.hpp" 
#include "Edge.hpp" 

using namespace std;

class Junction;
class Vertex;
class Edge;

class DuplicateJunctionException: public exception {
    private:
        Junction * mJunction;
        string whatMsg;
    public:
        DuplicateJunctionException(Junction * aJunction);
        virtual const char * what() const throw();
};

class SegmentDoesNotExistException: public exception {
    private:
        int mSegId;
        string whatMsg;
    public:
        SegmentDoesNotExistException(int aSegId);
        virtual const char * what() const throw();
};


#endif
