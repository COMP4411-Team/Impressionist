//
// ScatterLineBrush.h
//

#pragma once
#ifndef SCATTER_LINE_BRUSH_H
#define SCATTER_LINE_BRUSH_H

#include "LineBrush.h"

class ScatterLineBrush : public LineBrush
{
public:
    ScatterLineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

    void BrushBegin(const Point source, const Point target);
    void BrushMove(const Point source, const Point target);
    void BrushEnd(const Point source, const Point target);
    char* BrushName(void);

private:
    int numLines{3};
};

#endif

