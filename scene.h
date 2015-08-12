#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "vertex.h"
#include "edge.h"
#include "frame.h"

class Scene : public QGraphicsScene
{
public:
    enum Mode {SelectionMode, PolylineMode};

private:
    Mode sceneMode;
    Edge *newEdge;
    Frame changesFrame;

    void checkBoundingRect(QGraphicsItem *item);
    void updateSelectedItems(QGraphicsSceneMouseEvent *event);
    void bindNewEdgeTo(Vertex *end);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

public:

    Scene();

    Frame getChangesFrame();
    void setChangesFrame(Frame newChangesFrame);
    void clearChangesFrame();

    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);

    void setMode(Mode mode);
    bool isChanged();
};

#endif // SCENE_H
