#include "frame.h"

QDataStream& operator <<(QDataStream& out, SavedFrameData savedFrameData)
{
    out << savedFrameData.newVertices;
    out << savedFrameData.newEdges;
    out << savedFrameData.removedItems;
    out << savedFrameData.changedItems;

    return out;
}

QDataStream& operator >>(QDataStream& in, SavedFrameData &savedFrameData)
{
    in >> savedFrameData.newVertices;
    in >> savedFrameData.newEdges;
    in >> savedFrameData.removedItems;
    in >> savedFrameData.changedItems;

    return in;
}


