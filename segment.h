#ifndef SEGMENT_H
#define SEGMENT_H

#include <QVector3D>
#include <QVector>

class Segment {

public:
    Segment();
    Segment(QVector<QVector3D> &p);

    QVector<QVector3D> points;
    float minDist(Segment &s);
    void converge(Segment &s);
    QVector3D front();
    QVector3D back();
};

#endif // SEGMENT_H
