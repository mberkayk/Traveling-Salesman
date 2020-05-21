#include "cityview.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
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
    visualizeTour();

//    for(int i = 0 ; i < numOfCities; i++){
//        qDebug() << tour[i];
//    }
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

QVector<QVector<int>> CityView::perms(QVector<int> & orig, QVector<int> &perm){
    if(orig.length() > 13) {
        qDebug() << "Way too big";
        return QVector<QVector<int>>();
    }
    QVector<QVector<int>> result = QVector<QVector<int>>();
    for(int i = 0; i < orig.length(); i++){
        QVector<int> tempPerm = perm;
        tempPerm.append(orig.at(i));

        QVector<int> tempOrig = orig;
        tempOrig.removeAt(i);

        if(tempOrig.length() == 0){
            result.append(tempPerm);
            return result;
        }else{
            result.append(perms(tempOrig, tempPerm));
        }
    }
    return result;
}

void CityView::divAndConq(){
    QuadTree qTree = QuadTree(0, 0, 8000, 5000); // construct a quad tree that spans the range of(0,0,8000,5000) rectangle
    QuadTree::limit = 9; // the number of cities a branch can hold before getting divided into four new branches
    for(int i = 0; i < numOfCities; i++){// insert all the cities into the quadtree
        qTree.insert(cities[i]);
    }
    QVector<QuadTree> sections = QVector<QuadTree>();
    qTree.getAllNonEmptySections(sections); // get all the non-empty branches of the quadTree into the sections Vector

    QVector<Segment> segs = QVector<Segment>(); // segments of shortest paths
    //iterate through all the non-empty branches to construct a set of segments
    for(int i = 0; i < sections.length(); i++){
        QuadTree qTree = sections.at(i);
        if(qTree.points.length() == 1){ // if the branch has 1 city
            continue;
        }else if(qTree.points.length() == 2 || qTree.points.length() == 3){ // if the brach has 2 cities
            segs.append(qTree.points);
        }else{ //if the branch has more than 3 cities use exhaustive search
            QVector<int> orig = QVector<int>();
            for(int j = 0; j < qTree.points.length(); j++){
                orig.append(j);
            }
            QVector<int> tempPerm = QVector<int>();
            QVector<QVector<int>> perm = perms(orig, tempPerm); //all the permutations of indices of points in the section
            if(perm.empty()) return;
            //Find the perm with the shortest tour length
            Segment shortest = Segment(qTree.points);
            float minDist = shortest.tourLength();
            int indexOfSegWithMinDist = 0; //index in the perm vector
            for(int j = 1; j < perm.length(); j++){
                //construct the segment
                Segment s  = Segment();
                for(int k = 0; k < qTree.points.length(); k++){
                    s.points.append(qTree.points.at(perm.at(j).at(k)));
                }
                //compare it to the shortest segment
                float currentTourLength = s.tourLength();
                if(currentTourLength < minDist){ // if it is shorter than the shortest segment
                    minDist = currentTourLength; //then make this the shortest segment
                    indexOfSegWithMinDist = j; // index in the perm vector
                }
            }

            shortest.points.clear();
            for(int j = 0; j < qTree.points.length(); j++){ //construct the segment from shortest perm
                shortest.points.append(qTree.points.at(perm.at(indexOfSegWithMinDist).at(j)));
            }
            segs.append(shortest); // append the tour with the shortest length to segs
        }
    }

    //TODO: Now find the shortest tour among the shortest segments
    QVector<int> orig = QVector<int>();
    for(int i = 0; i < segs.length(); i++) orig.append(i);
    QVector<int> permsArg = QVector<int>();
    QVector<QVector<int>> perm = perms(orig, permsArg); //get the permutations of segments
    if(perm.empty()) return;

    //find the shortest overall tour
    float minDist = 0;
    for(int i = 0; i < perm.at(0).length()-1; i++){
        Segment s = segs.at(perm.at(0).at(i));
        Segment s2 = segs.at(perm.at(0).at(i+1));
        minDist += s.minDist(s2);
    }
    Segment s = segs.at(perm.at(0).front());
    Segment s2 = segs.at(perm.at(0).back());
    minDist += s.minDist(s2);

    int indexOfShortestPerm = 0;
    for(int i = 1; i < perm.length(); i++){ //iterate through permutations of segments
        float current = 0;
        for(int j = 0; j < perm.at(i).length()-1; j++){ // iterate through indices of the permutation
            Segment s = segs.at(perm.at(i).at(j));
            Segment s2 = segs.at(perm.at(i).at(j+1));
            current += s.minDist(s2);
        }
        Segment s = segs.at(perm.at(i).front());
        Segment s2 = segs.at(perm.at(i).back());
        current += s.minDist(s2);
        if(current < minDist){
            minDist = current;
            indexOfShortestPerm = i;
        }
    }

    Segment final = Segment();
    for(int i = 0; i < perm.at(indexOfShortestPerm).length(); i++){
        Segment s = segs.at(perm.at(indexOfShortestPerm).at(i));
        final.converge(s);
    }

    for(int i = 0; i < numOfCities; i++){
        tour[i] = final.points.at(i).z();
    }

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
