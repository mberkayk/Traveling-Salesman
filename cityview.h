#ifndef CITYVIEW_H
#define CITYVIEW_H

#include <QGraphicsScene>
#include <QVector3D>

class CityView : public QGraphicsScene {

private:
    QVector3D *cities;

    void parseFile();
    const static int numOfCities = 48;
    int tour[numOfCities];

    void nearestNeighbour();
    void greedy();
    void divAndConq();

    QPolygon tourPolygon;
    void visualizeTour();
public:
    CityView();
    ~CityView();
};

#endif // CITYVIEW_H
