#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsItem>
#include <QPainter>
#include <QLinkedList>

#include "edge.h"

class Edge;

class Vertex : public QGraphicsEllipseItem
{
    QLinkedList <Edge *> edges;

public:
    enum {Type = UserType + 1};
    int type() const;

    Vertex(QPointF pos, QGraphicsItem *parent = 0);

    void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    Edge *getEdgeByNeighbor(Vertex *neighborVertex);
    void addEdge(Edge *newEdge);
    QLinkedList <Edge *> getEdges();
    void updateEdgesPos();
};

#endif // VERTEX_H
