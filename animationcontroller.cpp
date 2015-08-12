#include "animationcontroller.h"


bool AnimationController::isGlobalChangesMode()
{
    return globalChangesMode;
}

void AnimationController::setGlobalChangesMode(bool value)
{
    globalChangesMode = value;
}
AnimationController::AnimationController(Scene *scene)
{
    this->scene = scene;
    framesList.append(scene->getChangesFrame());
    currentFrameIndex = 0;
    globalChangesMode = false;
    changed = false;
}

void AnimationController::nextFrame()
{
    if(currentFrameIndex == framesList.size() - 1)
        return;

    applyChanges();

    Frame nextFrame = framesList[++currentFrameIndex];

    foreach (QGraphicsItem *newItem, nextFrame.newItems)
    {
        newItem->setVisible(true);
        Vertex *newVertex = qgraphicsitem_cast <Vertex *>(newItem);

        if(newVertex)
            newVertex->updateEdgesPos();
    }

    foreach (QGraphicsItem *removedItem, nextFrame.removedItems)
        removedItem->setVisible(false);

    foreach (Vertex * changedVertex, nextFrame.changedItems.keys())
    {
        changedVertex->setPos(changedVertex->scenePos() + nextFrame.changedItems[changedVertex]);
        changedVertex->updateEdgesPos();
    }
}

void AnimationController::previousFrame()
{
    if(currentFrameIndex == 0)
        return;

    applyChanges();

    Frame currentFrame = framesList[currentFrameIndex--];

    foreach (QGraphicsItem *newItem, currentFrame.newItems)
        newItem->setVisible(false);

    foreach (QGraphicsItem *removedItem, currentFrame.removedItems)
    {
        removedItem->setVisible(true);
        Vertex *removedVertex = qgraphicsitem_cast <Vertex *>(removedItem);

        if(removedVertex)
            removedVertex->updateEdgesPos();
    }

    foreach (Vertex *changedVertex, currentFrame.changedItems.keys())
    {
        changedVertex->setPos(changedVertex->scenePos() - currentFrame.changedItems[changedVertex]);
        changedVertex->updateEdgesPos();
    }
}

void AnimationController::addFrame()
{
    applyChanges();
    framesList.insert(currentFrameIndex + 1, Frame());
    changed = true;
}

void AnimationController::deleteFrame()
{
    if(framesList.size() == 1)
    {
        scene->clear();
        scene->clearChangesFrame();
        return;
    }

    applyChanges();

    if(currentFrameIndex != framesList.size() - 1)
    {
        nextFrame();
        scene->setChangesFrame(framesList[currentFrameIndex - 1]);
        applyChanges();
        currentFrameIndex--;
    }

    framesList.removeAt(currentFrameIndex);
    changed = true;
}

void AnimationController::saveAnimation(QString fileName)
{
    QHash <QGraphicsItem *, ItemID> lookupTable;
    ItemID newID = 0;
    #define GET_ID(item) lookupTable.contains(item) ? lookupTable[item] : lookupTable[item] = newID++

    QFile file(fileName);

    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    QList <SavedFrameData> dataToSave;

    if(scene->isChanged())
        applyChanges();

    foreach (Frame currentFrame, framesList)
    {
        SavedFrameData frameDataToSave;

        foreach (QGraphicsItem *newItem, currentFrame.newItems)
        {
            Vertex *newVertex = qgraphicsitem_cast <Vertex *>(newItem);

            if(newVertex)
            {
                frameDataToSave.newVertices.insert(GET_ID(newVertex), newVertex->scenePos());
            }
            else
            {
                Edge *newEdge = qgraphicsitem_cast <Edge *>(newItem);
                QPair <ItemID, ItemID> ends(GET_ID(newEdge->getBeginning()), GET_ID(newEdge->getEnd()));

                frameDataToSave.newEdges.insert(GET_ID(newEdge), ends);
            }
        }

        foreach (QGraphicsItem *removedItem, currentFrame.removedItems)
            frameDataToSave.removedItems.append(GET_ID(removedItem));

        foreach (Vertex *changedVertex, currentFrame.changedItems.keys())
        {
            QPointF offset = currentFrame.changedItems[changedVertex];
            frameDataToSave.changedItems.insert(GET_ID(changedVertex), offset);
        }

        dataToSave.append(frameDataToSave);
    }

    out << dataToSave;

    file.close();

    changed = false;
}

void AnimationController::loadAnimation(QString fileName)
{
    QFile file(fileName);

    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    QList <SavedFrameData> dataToLoad;

    in >> dataToLoad;

    file.close();

    scene->clear();
    scene->clearChangesFrame();
    framesList.clear();

    QHash <ItemID, QGraphicsItem *> lookupTable;
    bool firstFrame = true;

    foreach (SavedFrameData frameDataToLoad, dataToLoad)
    {
        Frame newFrame;

        foreach (ItemID newVertexID, frameDataToLoad.newVertices.keys())
        {
            if(lookupTable.contains(newVertexID))
            {
                newFrame.newItems.append(lookupTable[newVertexID]);
            }
            else
            {
                Vertex *newVertex = new Vertex(frameDataToLoad.newVertices[newVertexID]);

                scene->addItem(newVertex);
                if(!firstFrame)
                    newVertex->setVisible(false);
                lookupTable.insert(newVertexID, newVertex);
            }
        }
        foreach (ItemID newEdgeID, frameDataToLoad.newEdges.keys())
        {
            if(lookupTable.contains(newEdgeID))
            {
                newFrame.newItems.append(lookupTable[newEdgeID]);
            }
            else
            {
                QPair <ItemID, ItemID> ends = frameDataToLoad.newEdges[newEdgeID];
                Vertex *beginning = qgraphicsitem_cast <Vertex *>(lookupTable[ends.first]);
                Vertex *end = qgraphicsitem_cast <Vertex *>(lookupTable[ends.second]);
                Edge *newEdge = new Edge(beginning, end);

                scene->addItem(newEdge);
                if(!firstFrame)
                    newEdge->setVisible(false);
                lookupTable.insert(newEdgeID, newEdge);
            }
        }

        foreach (ItemID removedItemID, frameDataToLoad.removedItems)
            newFrame.removedItems.append(lookupTable[removedItemID]);

        foreach (ItemID changedVertexID, frameDataToLoad.changedItems.keys())
        {
            QPointF offset = frameDataToLoad.changedItems[changedVertexID];
            Vertex *changedVertex = qgraphicsitem_cast <Vertex *>(lookupTable[changedVertexID]);
            newFrame.changedItems.insert(changedVertex, offset);
        }

        framesList.append(newFrame);
        currentFrameIndex = framesList.size() - 1;
        applyChanges();

        if(firstFrame)
            firstFrame = false;
    }

    currentFrameIndex = 0;

    changed = false;
}

void AnimationController::applyChanges()
{
    Frame  changesFrame = scene->getChangesFrame();
    Frame &currentFrame = framesList[currentFrameIndex];
    Frame *nextFrame;

    if(currentFrameIndex != framesList.size() - 1)
        nextFrame = &framesList[currentFrameIndex + 1];
    else
        nextFrame = NULL;

    applyNewItems(changesFrame, currentFrame, nextFrame);
    applyRemovedItems(changesFrame, currentFrame, nextFrame);
    applyChangedItems(changesFrame, currentFrame, nextFrame);

    scene->clearChangesFrame();
}

void AnimationController::addOffsetToFirstUse(Vertex *currentFrameChangedVertex, QPointF offset)
{
    Frame *frameWithFirstUse;

    if(currentFrameIndex == framesList.size() - 1)
        return;
    else
        frameWithFirstUse = &framesList[currentFrameIndex + 1];

    if(frameWithFirstUse->removedItems.contains(currentFrameChangedVertex))
    {
        int i;
        for(i = currentFrameIndex + 2;
            i < framesList.size() &&!framesList[i].newItems.contains(currentFrameChangedVertex);
            i++);
        if(i < framesList.size())
            frameWithFirstUse = &framesList[i];
        else
            frameWithFirstUse = NULL;
    }

    if(frameWithFirstUse)
    {
        QPointF &firstUseOffset = frameWithFirstUse->changedItems[currentFrameChangedVertex];
        firstUseOffset += offset;
        if(firstUseOffset.isNull())
            frameWithFirstUse->changedItems.remove(currentFrameChangedVertex);
    }
}

void AnimationController::applyNewItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame)
{
    foreach (QGraphicsItem *newItem, changesFrame.newItems)
    {
        if(!currentFrame.removedItems.removeOne(newItem))
            currentFrame.newItems.append(newItem);

        if(nextFrame)
        {
            if(!globalChangesMode)
            {
                if(!nextFrame->newItems.contains(newItem) &&
                   !nextFrame->removedItems.contains(newItem))
                {
                    nextFrame->removedItems.append(newItem);
                }
            }
            else
            {
                Edge *newEdge = qgraphicsitem_cast <Edge *>(newItem);

                if(newEdge)
                    for(int i = currentFrameIndex + 1; i < framesList.size(); i++)
                    {
                        framesList[i].newItems.removeOne(newEdge);
                        framesList[i].removedItems.removeOne(newEdge);

                        framesList[i].newItems.removeOne(newEdge->getBeginning());
                        framesList[i].removedItems.removeOne(newEdge->getBeginning());
                        framesList[i].newItems.removeOne(newEdge->getEnd());
                        framesList[i].removedItems.removeOne(newEdge->getEnd());
                    }
            }
        }
    }
}

void AnimationController::applyRemovedItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame)
{
    foreach (QGraphicsItem *removedItem, changesFrame.removedItems)
    {
        Vertex *removedVertex = qgraphicsitem_cast <Vertex *>(removedItem);
        QPointF offset;

        if(!currentFrame.newItems.removeOne(removedItem))
        {
            if(removedVertex)
            {
                QHash <Vertex *, QPointF>::iterator currentFrameChangedVertex = currentFrame.changedItems.find(removedVertex);

                if(currentFrameChangedVertex != currentFrame.changedItems.end())
                {
                    offset = currentFrameChangedVertex.value();
                    removedVertex->setPos(removedVertex->scenePos() - currentFrameChangedVertex.value());
                    currentFrame.changedItems.remove(removedVertex);
                }
            }
            currentFrame.removedItems.append(removedItem);
        }

        if(nextFrame)
        {
            if(!globalChangesMode)
            {
                if(!nextFrame->newItems.contains(removedItem) &&
                   !nextFrame->removedItems.removeOne(removedItem))
                {
                    nextFrame->newItems.append(removedItem);
                }
                if(!offset.isNull())
                    addOffsetToFirstUse(removedVertex, offset);
            }
            else
                for(int i = currentFrameIndex + 1; i < framesList.size(); i++)
                {
                    framesList[i].newItems.removeOne(removedItem);
                    framesList[i].removedItems.removeOne(removedItem);

                    if(removedVertex)
                    {
                        framesList[i].changedItems.remove(removedVertex);

                        foreach (Edge *removedEdge, removedVertex->getEdges())
                        {
                            framesList[i].newItems.removeOne(removedEdge);
                            framesList[i].removedItems.removeOne(removedEdge);
                        }
                    }
                }
        }
    }
}

void AnimationController::applyChangedItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame)
{
    foreach (Vertex *changedVertex, changesFrame.changedItems.keys())
    {
        QLinkedList <QGraphicsItem *>::iterator currentFrameNewVertex = std::find(currentFrame.newItems.begin(),
                                                                                  currentFrame.newItems.end(), changedVertex);
        if(currentFrameNewVertex == currentFrame.newItems.end())
        {
            QPointF &offset = currentFrame.changedItems[changedVertex];
            offset += changesFrame.changedItems[changedVertex];
            if(offset.isNull())
                currentFrame.changedItems.remove(changedVertex);
        }

        if(!globalChangesMode && nextFrame)
        {
            addOffsetToFirstUse(changedVertex, -changesFrame.changedItems[changedVertex]);
        }
    }
}

int AnimationController::getCurrentFrameIndex()
{
    return currentFrameIndex;
}

int AnimationController::totalFrames()
{
    return framesList.size();
}

bool AnimationController::isChanged()
{
    return changed;
}
