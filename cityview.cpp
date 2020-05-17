#include "cityview.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "segment.h"
#include "quadtree.h"

CityView::CityView() : QGraphicsScene() {
    this->cities = new QVector3D[numOfCities]; // z coordinate of the QVector3D is the index or the "id" of the city
    this->tourPolygon = QPolygon(); // polygon object for visualization
    this->parseFile(); // put the data in the text file to cities[]

    //initialize tour[]
    for(int i = 0; i < numOfCities; i++) {
        tour[i] = 0; // tour[0] = id of the first visited city, tour[1] = id of the second visited city etc.
    }

    //add the polygon to the list of drawn items (polygon is empty for now)
    this->addPolygon(this->tourPolygon, QColor(0,0,255));

    // Draw red dots for every city
    QPen pen = QPen(QColor(255, 0, 0));
    for(int i = 0; i < numOfCities; i++){
        int x = static_cast<int>(cities[i].x() / 25.0f);
        int y = static_cast<int>(cities[i].y() / 25.0f);
        this->addEllipse(x, y, 5, 5, pen, pen.brush());
    }

//    nearestNeighbour();
//    greedy();
    divAndConq();
//    visualizeTour();

}

void CityView::parseFile(){
    // open the file
    QFile file("./att48_xy.txt");
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    int cityCounter = 0;
    while(!in.atEnd()) { // read every line until the end
        QString line = in.readLine();
        QStringList fields = line.split(" "); // split the x and y values
        // sometimes returns empty strings in addition x and y values
        if(fields.length() > 2){ //if there are more string than just x and y coordinates
            for (int i = 0; i < fields.length(); i++) {
                if(fields[i].length() == 0){
                    fields.removeAt(i); // remove the unnecessary ones
                    i--;
                }
            }
        }

        cities[cityCounter].setX(fields[0].toInt()); //put x value into the cities[i]
        cities[cityCounter].setY(fields[1].toInt()); // put y value into the cities[i]
        cities[cityCounter].setZ(cityCounter);       // use the z value as an "id" for the city
        cityCounter++;
    }

    file.close(); // close the file
}

void CityView::nearestNeighbour(){
    QVector<int> citiesLeft = QVector<int>(); // indices of cities that haven't been visited
    for(int i = 0; i < numOfCities; i++) {
        tour[i] = 0; // the final tour (48  indices in order)
        citiesLeft.append(i);
    }

    int numOfCitiesVisited = 0;
    tour[0] = 0;
    citiesLeft.removeFirst(); //start with the first city
    numOfCitiesVisited++;
    int currentCity = 0; // cities[0]

    while(citiesLeft.length() > 0){
        int nearestCity = -1;
        float minDist = 10000;
        foreach(int i, citiesLeft){
            float dist = cities[i].distanceToPoint(cities[currentCity]);
            if(minDist > dist) {
                minDist = dist;
                nearestCity = i;
            }
        }
        tour[numOfCitiesVisited] = nearestCity;
        numOfCitiesVisited++;
        currentCity = nearestCity;
        for(int i = 0; i < citiesLeft.length(); i++){
            if(citiesLeft[i] == nearestCity){
                citiesLeft.removeAt(i);
            }
        }
    }
}

void CityView::greedy(){
    QVector<Segment> segments = QVector<Segment>(); // list of segments
    // add a segment to the list above for every city
    for(int i = 0; i < numOfCities; i++){
        QVector<QVector3D> points = QVector<QVector3D>();
        points.append(cities[i]); // add a single city to the list of cities
        //Segment class holds a list of 3D points(city coordinates and city id's)
        Segment s = Segment(points); // add the list with the single city to the segment s
        segments.append(s); // add the segment s to the list of segments
    }

    while(segments.length() > 1){ // iterate until there is only one big segment
        //get the two closest segments
        float leastDist = 100000;
        int seg1;
        int seg2;
        for(int i = 0; i < segments.length() ; i++){
            for(int j = i+1; j < segments.length(); j++){
                Segment t1 = segments.at(i);
                Segment t2 = segments.at(j);
                float currDist = t1.minDist(t2);
                if(currDist < leastDist){
                    leastDist = currDist;
                    seg1 = i;
                    seg2 = j;
                }
            }
        }
        //after getting the two closest segments
        Segment t1 = segments.at(seg1);
        Segment t2 = segments.at(seg2);
        t1.converge(t2); // converge them
        segments.remove(seg1); //remove the two seperate segments from the list
        if(seg1 < seg2){
            segments.remove(seg2-1);
        }else{
            segments.remove(seg2);
        }
        segments.append(t1); // add the new converged longer segment to the list
    }

    //put everything in tour[]
    for(int i = 0; i < segments.at(0).points.length(); i++){
        tour[i] = static_cast<int>(segments.at(0).points.at(i).z());
    }
}

void CityView::divAndConq(){
    QuadTree qTree = QuadTree(0, 0, 8000, 5000);
    QuadTree::limit = 5;
    for(int i = 0; i < numOfCities; i++){
        qTree.insert(cities[i]);
    }
    QVector<QuadTree> sections = QVector<QuadTree>();
    qTree.getAllNonEmptySections(sections);


}

void CityView::visualizeTour(){
    int points[numOfCities * 2];
    for(int i = 0; i < numOfCities; i++){
        points[2*i] = static_cast<int>(cities[tour[i]].x() / 25.0f);
        points[2*i+1] = static_cast<int>(cities[tour[i]].y() / 25.0f);
    }

    this->tourPolygon.setPoints(numOfCities, points);
    this->items().removeLast();
    this->addPolygon(this->tourPolygon, QColor(0,0,255));
}

CityView::~CityView(){
    delete [] cities;
}
