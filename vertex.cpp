#include "vertex.h"

Vertex::Vertex(QPointF pos, QGraphicsItem *parent)
      : QGraphicsEllipseItem(QRectF(-VERTEX_SIZE, -VERTEX_SIZE, 2 * VERTEX_SIZE, 2 * VERTEX_SIZE), parent)
{
    setFlags(ItemIsMovable | ItemIsSelectable);
    setZValue(VERTEX_SIZE);
    setPos(pos);
}

void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->drawEllipse(boundingRect());
}

int Vertex::type() const
{
    return Type;
}

Edge *Vertex::getEdgeByNeighbor(Vertex *neighborVertex)
{
    if(!neighborVertex)
        return NULL;

    foreach (Edge *edge, edges)
    {
        if(edge->getBeginning() == neighborVertex || edge->getEnd() == neighborVertex)
            return edge;
    }

    return NULL;
}

void Vertex::addEdge(Edge *newEdge)
{
    edges.append(newEdge);
}

QLinkedList <Edge *> Vertex::getEdges()
{
    return edges;
}

void Vertex::updateEdgesPos()
{
    foreach (Edge *edge, edges)
        edge->updateLinePos();
}
