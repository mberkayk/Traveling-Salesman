#ifndef CITYVIEW_H
#define CITYVIEW_H

#include <QGraphicsScene>
#include <QVector3D>
#include "segment.h"

class CityView : public QGraphicsScene {

private:
    QVector3D *cities;

    void parseFile();
    const static int numOfCities = 48;
    int tour[numOfCities];

    void nearestNeighbour();
    void greedy();
    QVector<QVector<int>> perms(QVector<int> &, QVector<int> &);
    void divAndConq();

    QPolygon tourPolygon;
    void visualizeTour();
public:
    CityView();
    ~CityView();
};

#endif // CITYVIEW_H
