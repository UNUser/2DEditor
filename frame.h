#ifndef FRAME_H
#define FRAME_H

#include <QLinkedList>
#include <QHash>
#include <QGraphicsItem>
#include <QDataStream>

#include "vertex.h"

struct Frame
{
    QLinkedList <QGraphicsItem *> newItems;
    QLinkedList <QGraphicsItem *> removedItems;
    QHash <Vertex *, QPointF> changedItems;
};

typedef qint32 ItemID;

struct SavedFrameData
{
    QHash <ItemID, QPointF> newVertices;
    QHash <ItemID, QPair <ItemID, ItemID> > newEdges;
    QList <ItemID> removedItems;
    QHash <ItemID, QPointF> changedItems;
};

QDataStream& operator <<(QDataStream& out, SavedFrameData savedFrameData);
QDataStream& operator >>(QDataStream& in, SavedFrameData &savedFrameData);

#endif // FRAME_H
