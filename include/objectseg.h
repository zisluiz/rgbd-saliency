#include "pointseg.h"
#include <boost/python.hpp>

class ObjectSeg
{
    public:
        ObjectSeg();
        ObjectSeg(int id, int pointsLength, boost::python::list points);
        int id;
        int pointsLength;
        boost::python::list points; 
};