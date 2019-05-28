#include "objectseg.h"

ObjectSeg::ObjectSeg() {

}

ObjectSeg::ObjectSeg(int id, int pointsLength, PointSeg *points)
{
    this->id = id;
    this->pointsLength = pointsLength;
    this->points = points;
}