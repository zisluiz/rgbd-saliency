#include "objectseg.h"

ObjectSeg::ObjectSeg() {

}

ObjectSeg::ObjectSeg(int id, int pointsLength, boost::python::list points)
{
    this->id = id;
    this->pointsLength = pointsLength;
    this->points = points;
}