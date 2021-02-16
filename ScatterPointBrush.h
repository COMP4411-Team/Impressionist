//
// ScatterPointBrush.h
//

#pragma once
#ifndef SCATTER_POINT_BRUSH_H
#define SCATTER_POINT_BRUSH_H

#include "ImpBrush.h"
class ScatterPointBrush : public ImpBrush
{
public:
    ScatterPointBrush( ImpressionistDoc* pDoc = NULL, char* name = NULL );

    void BrushBegin( const Point source, const Point target );
    void BrushMove( const Point source, const Point target );
    void BrushEnd( const Point source, const Point target );
    char* BrushName( void );

private:
    int numPoints{10};
    double pointSize{1.0};
};

#endif

