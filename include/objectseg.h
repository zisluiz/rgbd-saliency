#include "pointseg.h"

class ObjectSeg
{
    public:
        ObjectSeg();
        ObjectSeg(int id, int pointsLength, PointSeg *points);
        int id;
        int pointsLength;
        PointSeg *points; 
};