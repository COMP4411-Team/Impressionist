//
// CricleBrush.h
//

#pragma once
#ifndef CIRCLE_BRUSH_H
#define CIRCLE_BRUSH_H

#include "ImpBrush.h"

class CircleBrush : public ImpBrush
{
public:
    CircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

    void BrushBegin(const Point source, const Point target);
    void BrushMove(const Point source, const Point target);
    void BrushEnd(const Point source, const Point target);
    char* BrushName(void);

protected:
    void drawCircle(const Point source, const Point target, double radius);
    int numEdges{360};
};

#endif

