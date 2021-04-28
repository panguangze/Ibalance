#include <iostream>
#include "Exceptions.hpp"

using namespace std;

DuplicateJunctionException::DuplicateJunctionException(Junction * aJunction) {
    mJunction = aJunction;
    whatMsg = "DuplicateJunctionException: " + mJunction->getInfo()[0] + ", " + mJunction->getInfo()[1];
}

const char * DuplicateJunctionException::what() const throw() {
    return whatMsg.c_str();
}

SegmentDoesNotExistException::SegmentDoesNotExistException(int aSegId) {
    mSegId = aSegId;
    whatMsg = "SegmentDoesNotExistException: Segment with ID " + to_string(mSegId) + " does not exist";
}

const char * SegmentDoesNotExistException::what() const throw() {
    return whatMsg.c_str();
}
