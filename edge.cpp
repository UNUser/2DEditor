#include "edge.h"

Edge::Edge(Vertex *beginning, QGraphicsItem *parent)
    : QGraphicsLineItem(QLineF(beginning->scenePos(), beginning->scenePos()), parent)
{
    this->beginning = beginning;
    this->end = beginning;

    removable = true;
    setPos(beginning->scenePos());
    setFlags(ItemIsSelectable);
}

Edge::Edge(Vertex *beginning, Vertex *end, QGraphicsItem *parent)
    : QGraphicsLineItem(QLineF(beginning->scenePos(), end->scenePos()), parent)
{
    this->beginning = beginning;
    this->end = end;

    beginning->addEdge(this);
    end->addEdge(this);

    removable = true;
    setPos(beginning->scenePos());
    setLine(QLineF(QPointF(0, 0), end->scenePos() - beginning->scenePos()));
    setFlags(ItemIsSelectable);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->drawLine(this->line());
}

void Edge::setEnd(Vertex *end)
{
    this->end = end;
    updateLinePos();
}

Vertex *Edge::getBeginning()
{
    return beginning;
}

Vertex *Edge::getEnd()
{
    return end;
}

void Edge::setRemovable(bool value)
{
    removable = value;
}

bool Edge::isRemovable()
{
    return removable;
}

int Edge::type() const
{
    return Type;
}

void Edge::updateLinePos()
{
    setPos(beginning->scenePos());
    setLine(QLineF(QPointF(0, 0), end->scenePos() - beginning->scenePos()));
}
