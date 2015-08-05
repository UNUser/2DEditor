#include "scene.h"

Scene::Scene()
{
    newEdge = NULL;
}

void Scene::setMode(Scene::Mode mode)
{
    sceneMode = mode;

    if(newEdge)
    {
        removeItem(newEdge);
        newEdge = NULL;
    }
}

bool Scene::isChanged()
{
    return !changesFrame.newItems.isEmpty() ||
           !changesFrame.removedItems.isEmpty() ||
           !changesFrame.changedItems.isEmpty();
}

void Scene::updateSelectedItems(QGraphicsSceneMouseEvent *event)
{
    foreach (QGraphicsItem *selectedItem, selectedItems())
    {
        Vertex *selectedVertex = qgraphicsitem_cast <Vertex *>(selectedItem);

        if(selectedVertex)
        {
            selectedVertex->updateEdgesPos();

            if(!changesFrame.newItems.contains(selectedVertex))
            {
//                QHash <Vertex *, QPointF>::iterator changedVertex = changesFrame.changedItems.find(selectedVertex);
                QPointF &offset = changesFrame.changedItems[selectedVertex];
//                if(changedVertex == changesFrame.changedItems.end())
//                    changedVertex = changesFrame.changedItems.insert(selectedVertex, QPointF(0, 0));


//                changedVertex.value() += event->scenePos() - event->lastScenePos();
                offset += event->scenePos() - event->lastScenePos();

//                if(changedVertex.value() == QPointF(0, 0))
                if(offset == QPointF(0, 0))
                    changesFrame.changedItems.remove(selectedVertex);
            }
        }
    }
}

void Scene::bindNewEdgeTo(Vertex *end)
{
    if(end && end != newEdge->getBeginning())
    {
        Edge *oldEdge = newEdge->getBeginning()->getEdgeByNeighbor(end);

        if(oldEdge)
        {
            if(!oldEdge->isVisible())
            {
                oldEdge->setVisible(true);
                changesFrame.newItems.append(oldEdge);
                oldEdge->setRemovable(false);
                oldEdge->updateLinePos();
            }
            removeItem(newEdge);
        }
        else
        {
            newEdge->setEnd(end);

            end->addEdge(newEdge);
            newEdge->getBeginning()->addEdge(newEdge);
        }
    }
    else
        removeItem(newEdge);

    newEdge = NULL;
}

void Scene::addItem(QGraphicsItem *item)
{
    if(!changesFrame.newItems.contains(item) &&
       !changesFrame.removedItems.removeOne(item))

        changesFrame.newItems.append(item);

    QGraphicsScene::addItem(item);
}

void Scene::removeItem(QGraphicsItem *item)
{
//We don't actually remove item, if it has been added on a previous frame. Just set it invisible.

    Edge *removedEdge = qgraphicsitem_cast <Edge *>(item);
    bool usedItem = false;

    if(removedEdge)
    {
        if(changesFrame.newItems.removeOne(removedEdge))
        {
            if(!removedEdge->isRemovable())
                usedItem = true;
            else
            {
                removedEdge->getBeginning()->getEdges().removeOne(removedEdge);
                removedEdge->getEnd()->getEdges().removeOne(removedEdge);
            }
        }
        else
            usedItem = true;
    }
    else
    {
        Vertex *removedVertex = qgraphicsitem_cast <Vertex *>(item);

        foreach (Edge *edge, removedVertex->getEdges())
            removeItem(edge);

        if(!changesFrame.newItems.removeOne(removedVertex))
        {
            usedItem = true;

            QHash <Vertex *, QPointF>::iterator changedVertex = changesFrame.changedItems.find(removedVertex);

            if(changedVertex != changesFrame.changedItems.end())
            {
                removedVertex->setPos(removedVertex->scenePos() - changedVertex.value());
                changesFrame.changedItems.remove(removedVertex);
            }
        }
    }

    if(usedItem)
    {
        if((!removedEdge || removedEdge->isRemovable()) && item->isVisible())
        {
            item->setVisible(false);
            changesFrame.removedItems.append(item);
        }
        else
            removedEdge->setRemovable(true);
    }
    else
    {
        QGraphicsScene::removeItem(item);
        delete item;
    }
}

Frame Scene::getChangesFrame()
{
    return changesFrame;
}

void Scene::setChangesFrame(Frame newChangesFrame)
{
    this->changesFrame = newChangesFrame;
}


void Scene::clearChangesFrame()
{
    foreach (QGraphicsItem *newItem, changesFrame.newItems)
    {
        Edge *newEdge = qgraphicsitem_cast <Edge *>(newItem);
        if(newEdge && !newEdge->isRemovable())
            newEdge->setRemovable(true);
    }

    changesFrame.newItems.clear();
    changesFrame.removedItems.clear();
    changesFrame.changedItems.clear();
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Vertex *vertexAtCursor = qgraphicsitem_cast <Vertex *>(itemAt(event->scenePos(), QTransform()));

    if(event->button() == Qt::RightButton)
    {
        if(sceneMode == PolylineMode && newEdge) // End the polyline
        {
            removeItem(newEdge);
            newEdge = NULL;
        }
        else if(vertexAtCursor) // Start connection of two vertices
        {
            newEdge = new Edge(vertexAtCursor);
            addItem(newEdge);
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        if(sceneMode == PolylineMode)
        {
            if(vertexAtCursor) // Connection of the polyline with existed vertex
            {
                if(newEdge)
                    bindNewEdgeTo(vertexAtCursor);

                newEdge = new Edge(vertexAtCursor);
                addItem(newEdge);
            }
            else // Creation of a new vertex of the polyline
            {
                Vertex *newVertex = new Vertex(event->scenePos());

                addItem(newVertex);

                if(newEdge)
                {
                    newEdge->setEnd(newVertex);

                    newEdge->getBeginning()->addEdge(newEdge);
                    newEdge->getEnd()->addEdge(newEdge);
                }

                newEdge = new Edge(newVertex);
                addItem(newEdge);
            }
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton || sceneMode == PolylineMode) // Creation of a new edge
    {
        if(newEdge)
            newEdge->setLine(QLineF(QPointF(0, 0), event->scenePos() - newEdge->getBeginning()->scenePos()));
    }
    else if(sceneMode == SelectionMode) // Moving of selected objects
    {
        if(!selectedItems().empty() && event->buttons() & Qt::LeftButton)
        {
            QGraphicsScene::mouseMoveEvent(event);
            updateSelectedItems(event);
        }
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton && newEdge) // Selection of the end of a new edge
    {
        Vertex *vertexAtCursor = qgraphicsitem_cast <Vertex *>(itemAt(event->scenePos(), QTransform()));
        bindNewEdgeTo(vertexAtCursor);
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete && !selectedItems().empty() && sceneMode == SelectionMode)
    {
        foreach (QGraphicsItem *selectedItem, selectedItems()) // Remove edges at first to avoid removing an edge twice (with removed vertex)
        {
            Edge *selectedEdge = qgraphicsitem_cast <Edge *>(selectedItem);

            if(selectedEdge)
            {
                removeItem(selectedEdge);
                selectedItems().removeOne(selectedEdge);
            }
        }
        foreach (QGraphicsItem *selectedItem, selectedItems())
            removeItem(selectedItem);
    }
}
