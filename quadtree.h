#ifndef QUADTREE_H
#define QUADTREE_H

#include <QVector>
#include <QVector3D>

class QuadTree {
public:
    QuadTree(float x, float y, float w, float h);
    ~QuadTree();

    static int limit;

    float x, y, width, height;
    bool divided;
    QVector<QVector3D> points;
    QuadTree *children[4]; //top left, top right, bottom right, bottom left

    bool contains(QVector3D&);
    void subdivide();
    void insert(QVector3D&);
    void getAllNonEmptySections(QVector<QuadTree> &t);

};

#endif // QUADTREE_H
