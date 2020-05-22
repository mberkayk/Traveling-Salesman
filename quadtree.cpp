#include "quadtree.h"

int QuadTree::limit;

QuadTree::QuadTree(float x, float y, float w, float h) {
    this->x = x;
    this->y = y;
    this->width = w; // the width that's spanned by this branch
    this->height = h;
    this->divided = false;
    this->points = QVector<QVector3D>();
    this->children[0] = nullptr;
    this->children[1] = nullptr;
    this->children[2] = nullptr;
    this->children[3] = nullptr;
}

bool QuadTree::contains(QVector3D &p){
    if(p.x() >= this->x && p.x() < this->x + this->width && p.y() >= this->y && p.y() < this->y + this->height) return true;
    else return false;
}

void QuadTree::subdivide(){
    this->children[0] = new QuadTree(x, y, width/2, height/2);
    this->children[1] = new QuadTree(x+width/2, y, width/2, height/2);
    this->children[2] = new QuadTree(x+width/2, y+height/2, width/2, height/2);
    this->children[3] = new QuadTree(x, y+height/2, width/2, height/2);

    for(int i = 0; i < this->points.length(); i++){
        QVector3D p = this->points.at(i);
        children[0]->insert(p);
        children[1]->insert(p);
        children[2]->insert(p);
        children[3]->insert(p);
    }
    this->points.clear();
    this->divided = true;
}

void QuadTree::insert(QVector3D &p){
    if (!(this->contains(p))) return;
    if(divided){
        this->children[0]->insert(p);
        this->children[1]->insert(p);
        this->children[2]->insert(p);
        this->children[3]->insert(p);
    }else{
        if(points.length() >= QuadTree::limit){
            this->subdivide();
            this->children[0]->insert(p);
            this->children[1]->insert(p);
            this->children[2]->insert(p);
            this->children[3]->insert(p);
        }else{
            points.append(p);
        }
    }
}

void QuadTree::getAllNonEmptySections(QVector<QuadTree> &t){
    if(this->divided){
        children[0]->getAllNonEmptySections(t);
        children[1]->getAllNonEmptySections(t);
        children[2]->getAllNonEmptySections(t);
        children[3]->getAllNonEmptySections(t);
    }else if (this->points.length() > 0){
        t.append(*this);
    }
}

QuadTree::~QuadTree(){
    delete children[0];
    delete children[1];
    delete children[2];
    delete children[3];
}
