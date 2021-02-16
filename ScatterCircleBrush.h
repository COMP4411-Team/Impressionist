//
// ScatterCircleBrush.h
//

#pragma once
#ifndef SCATTER_CIRCLE_BRUSH_H
#define SCATTER_CIRCLE_BRUSH_H

#include "CircleBrush.h"

class ScatterCircleBrush : public CircleBrush
{
public:
    ScatterCircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

    void BrushBegin(const Point source, const Point target);
    void BrushMove(const Point source, const Point target);
    void BrushEnd(const Point source, const Point target);
    char* BrushName(void);

private:
    int numCircles{3};
};

#endif

