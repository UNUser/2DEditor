#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>
#include "vertex.h"

class Vertex;

class Edge : public QGraphicsLineItem
{
    Vertex *beginning;
    Vertex *end;

    bool removable; // This flag is true if deleted on a previous frame Edge was added on the new frame and then deleted again
                    // It is used in order to do not delete this Edge as an item added and deleted on the new frame.
public:
    enum {Type = UserType + 2};
    int type() const;

    Edge(Vertex *beginning, QGraphicsItem *parent = 0);
    Edge(Vertex *beginning, Vertex* end, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setEnd(Vertex *end);
    Vertex *getBeginning();
    Vertex *getEnd();
    void setRemovable(bool value);
    bool isRemovable();
    void updateLinePos();
};

#endif // EDGE_H
