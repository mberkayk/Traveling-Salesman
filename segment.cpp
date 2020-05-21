#include "segment.h"
#include <QString>
#include <QDebug>

Segment::Segment(){
    this->points = QVector<QVector3D>();
}

Segment::Segment(QVector<QVector3D> &p) {
    foreach(QVector3D point, p){
        this->points.append(point);
    }
}

// return the minimum distance from this segment to the segment s(which is passed as an argument)
float Segment::minDist(Segment &s) {
    // every segment has a front and a back end
    // there are four possible combinations (front-front, front-back, back-front, back-back)
    // check every combination to find the one with the least distance
    float minDist = this->front().distanceToPoint(s.front());
    float dist = this->back().distanceToPoint(s.front());
    if(dist < minDist) minDist = dist;
    dist = this->front().distanceToPoint(s.back());
    if(dist < minDist) minDist = dist;
    dist = this->back().distanceToPoint(s.back());
    if(dist < minDist) minDist = dist;

    return minDist; // return the distance
}

//joins the segment (passed as a paremeter to this function) this segment in the shortest way
void Segment::converge(Segment &s){
    if(this->points.isEmpty()){
        points.append(s.points);
        return;
    }
    //First half of this function is just like the minDist function but instead of being concerned with the distance
    //we're concerned with the orientation
    QString orientation = "ftof"; //front to front
    float minDist = this->front().distanceToPoint(s.front());
    float dist = this->back().distanceToPoint(s.front());
    if(dist < minDist) {
        minDist = dist;
        orientation = "btof"; //back to front
    }
    dist = this->front().distanceToPoint(s.back());
    if(dist < minDist) {
        minDist = dist;
        orientation = "ftob"; //front to back
    }
    dist = this->back().distanceToPoint(s.back());
    if(dist < minDist) {
        minDist = dist;
        orientation = "btob"; //back to back
    }

    //after finding the orientation join to segments end to end accordingly
    if(orientation == "ftof"){
        for(int i = 0; i < s.points.length(); i++){
            points.prepend(s.points[i]);
        }
    }else if(orientation == "btof"){
        points.append(s.points);
    }else if (orientation == "ftob"){
        for(int i = 0; i < s.points.length(); i++){
            points.prepend(s.points[s.points.length() - 1 - i]);
        }
    }else if(orientation == "btob"){
        for(int i = 0; i < s.points.length(); i++){
            points.append(s.points[s.points.length() -1 - i]);
        }
    }

}

float Segment::tourLength(){
    float result = 0;
    for(int i = 0; i < this->points.length() - 1; i++){
        result += this->points.at(i).distanceToPoint(this->points.at(i+1));
    }
    result += this->points.last().distanceToPoint(this->points.first());
    return result;
}
// return the city at the front end of the segment
QVector3D Segment::front(){
    return this->points.front();
}

// return the city at the front end of the segment
QVector3D Segment::back(){
    return this->points.back();
}
