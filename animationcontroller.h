#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <QList>

#include "scene.h"
#include "frame.h"

class AnimationController
{
    Scene *scene;
    int currentFrameIndex;
    bool globalChangesMode; // If it is true, then apply changes to all following frames,
                            // otherwise discard all changes in the next frame
    bool changed;

    QList <Frame> framesList;

    void applyChanges();
    void addOffsetToFirstUse(Vertex *currentFrameChangedVertex, QPointF offset);
    void applyNewItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame);
    void applyRemovedItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame);
    void applyChangedItems(const Frame &changesFrame, Frame &currentFrame, Frame *nextFrame);

public:
    AnimationController(Scene *scene);

    void nextFrame();
    void previousFrame();

    void addFrame();
    void deleteFrame();

    void saveAnimation(QString fileName);
    void loadAnimation(QString fileName);

    bool isGlobalChangesMode();
    void setGlobalChangesMode(bool value);

    int getCurrentFrameIndex();
    int totalFrames();
    bool isChanged();

};

#endif // ANIMATIONCONTROLLER_H
